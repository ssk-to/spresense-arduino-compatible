#!/bin/bash

set -u

if [ $# != 1 ]; then
	echo "Usage: $0 <SDK_TOPDIR>"
	echo ""
	echo "Example:"
	echo "$1 your/path/to/spresense"
	echo ""
	exit 1
fi

echo "Export SDK from build."
echo "SDK_VERSION=${SDK_VERSION}"
echo "VARIANT_NAME=${VARIANT_NAME}"
NUTTX_EXPORT="sdk-export"
PACKAGE_NAME="${NUTTX_EXPORT}.zip"

if [ $(uname -o) = "Cygwin" ]; then
  SDK_DIR=`cd $1 && pwd | cygpath -m -f - $SDK_DIR`
else
  SDK_DIR=`cd $1 && pwd`
fi
TMP_DIR=`mktemp -d`

# versioning
cd $SDK_DIR
sh sdk/tools/mkversion.sh && mv .version nuttx/.version

# create sdk-export.zip
cd $SDK_DIR/sdk

## clean
echo "Clean SDK objects..."
make distcleankernel &>/dev/null
make cleankernel &>/dev/null
make distclean &>/dev/null

## Configuration
echo "Configure SDK components..."
echo "Kernel : ${SDL_KERNEL_CONF}"
echo "SDK    : ${SDK_CONFIG}"
./tools/config.py --kernel ${SDL_KERNEL_CONF}
./tools/config.py ${SDK_CONFIG}

## Build kernel
echo "Build kernel..."
make buildkernel &>/dev/null

echo "Export to Arduino..."
make export >/dev/null
if [ $? != 0 ]; then
	echo "make export failed"
fi

# work directory
unzip $PACKAGE_NAME -d $TMP_DIR >/dev/null
rm $PACKAGE_NAME

# create sdk directory
mkdir -p $TMP_DIR/sdk/${SDK_VERSION}/${VARIANT_NAME}

cd $TMP_DIR/sdk/${SDK_VERSION}/${VARIANT_NAME}

# create arch, include, startup
mv $TMP_DIR/${NUTTX_EXPORT}/nuttx/* .

mkdir -p ./include/apps
mv $TMP_DIR/${NUTTX_EXPORT}/sdk/modules/include/* ./include/apps
mv $TMP_DIR/${NUTTX_EXPORT}/sdk/bsp/include/sdk ./include

mkdir -p ./libs
mv $TMP_DIR/${NUTTX_EXPORT}/sdk/libs/* ./libs
mv ./libs/libsdk.a ./libs/libnuttx.a

# create debug, release
mkdir -p debug release
cp -a build libs release
cp -a build libs debug
rm -rf build libs

# copy firmwares
FW_LOADER="${SDK_DIR}/proprietary/spresense/bin/loader.espk"
FW_GNSSFW="${SDK_DIR}/proprietary/spresense/bin/gnssfw.espk"
FW_DIR="firmware"
mkdir -p ${FW_DIR}
cp ${FW_LOADER} ${FW_DIR}
cp ${FW_GNSSFW} ${FW_DIR}

cd $TMP_DIR
find . -name .gitignore | xargs rm
cd sdk
zip -r sdk-export.zip ${SDK_VERSION}/${VARIANT_NAME} >/dev/null
mv sdk-export.zip $SDK_DIR
rm -rf $TMP_DIR

PACKAGE_NAME=`readlink -f $SDK_DIR/sdk-export.zip`
echo "SDK exported to $PACKAGE_NAME"
