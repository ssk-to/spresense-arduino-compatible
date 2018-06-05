#! /bin/bash

set -u

CURRENT_DIR=`pwd`
SCRIPT_NAME=`readlink -e "${BASH_SOURCE[0]}"`
SCRIPT_DIR=`dirname "$SCRIPT_NAME"`
ARDUINO_DIR=${SCRIPT_DIR}/../Arduino15

# For debug
DEBUG=No

#
# Common description
#
JSON_FILE=package_spresense_index.json
PACKAGE_NAME=SPRESENSE
BOARD_NAME=spresense

#
# Tools description
#
SDK_NAME=spresense-sdk
GCC_NAME=gcc-arm-none-eabi

#
# Script description
#
SDK_IMPORT=${SCRIPT_DIR}/sdk_import.sh
SDK_EXPORT=${SCRIPT_DIR}/sdk_export.sh
JSN_LOADER=${SCRIPT_DIR}/python/load_tool_variable_from_json.py

#
# Function description
#

# function   : debug_print
# description: Print debug information if DEBUG is 'Yes'
function debug_print()
{
	if [ "${DEBUG}" == "Yes" ]; then
		echo $1
	fi
}

# function   : show_help
# description: Show help and exit.
function show_help()
{
	echo "  Usage:"
	echo "       $0 -H <Host Name> [-S <sdk build path> -v <variant> -k <kernel config> -g <gcc archive path> -s <sdk archive path> -H <Target Host> -p]"
	echo ""
	echo "  Mandatory argument:"
	echo "       -H: Target Arduino IDE Host name (Windows/Linux32/Linux64/Mac)"
	echo ""
	echo "  Optional arguments:"
	echo "       -S: Spresense SDK build root path (if you use local build)"
	echo "       -v: Spresense variant name (default: generic)"
	echo "       -k: Spresense kernel configuration (release/debug) (default: release)"
	echo "       -g: GCC tool archive path (if you use local archive)"
	echo "       -s: Spresense SDK archive path (if you use local archive)"
	echo "       -H: Arduino IDE Host name(Windows/Linux32/Linux64/Mac)"
	echo "       -p: Use private access (Skip GCC installation)"
	echo "       -h: Show help (This message)"
	exit
}

# function   : install_tool_from_http
# description: Install target tool from HTTP.
#              Tool version and URL is from package_*_index.json(${JSON_FILE})
# $1         : Package name
# $2         : Board name
# $3         : Json file name
# $4         : Arduino IDE Host name(Windows/Linux32/Linux64/Mac)
# $5         : Target tool name
function install_tool_from_http()
{
	PKG_NAME=$1
	BRD_NAME=$2
	JSN_NAME=${ARDUINO_DIR}/$3
	TARGET_HOST=$4
	TOOL_NAME=$5
	TOOL_VERSION=`${JSN_LOADER} -j ${JSN_NAME} -p ${PKG_NAME} -b ${BRD_NAME} -t ${TOOL_NAME} -H ${TARGET_HOST} -k version`
	TOOL_ADDR=`${JSN_LOADER} -j ${JSN_NAME} -p ${PKG_NAME} -b ${BRD_NAME} -t ${TOOL_NAME} -H ${TARGET_HOST} -k url`
	TOOL_PATH=${ARDUINO_DIR}/packages/${PKG_NAME}/tools/${TOOL_NAME}

	# Host tool check
	if [ "`which wget`" == "" ]; then
		echo "ERROR: 'wget' not installed. Please install 'wget'"
		exit
	fi

	echo "Download and Install Tool:${TOOL_NAME}, Version:${TOOL_VERSION}..."
	mkdir -p ${TOOL_PATH}

	debug_print "Download ${TOOL_NAME} into ${TOOL_PATH}"
	wget ${TOOL_ADDR} -O ${TOOL_PATH}/package.zip

	echo "Install ${TOOL_NAME}..."
	unzip -o ${TOOL_PATH}/package.zip -d ${TOOL_PATH} > /dev/null
	rm ${TOOL_PATH}/package.zip

}

# function   : install_tool_from_archive
# description: Install target tool from archive.
# $1         : Package name
# $2         : Target tool name
# $3         : Archive path
function install_tool_from_archive()
{
	PACKAGE_NAME=$1
	TOOL_NAME=$2
	ARCHIVE_PATH=$3

	# Check file exists
	if [ ! -f "${ARCHIVE_PATH}" ]; then
		echo "ERROR: No such file(${ARCHIVE_PATH})"
		exit
	fi

	TOOL_PATH=${ARDUINO_DIR}/packages/${PACKAGE_NAME}/tools/${TOOL_NAME}

	echo "Install ${TOOL_NAME} from archive..."
	debug_print "Install ${TOOL_NAME} to ${TOOL_PATH} from archive ${ARCHIVE_PATH}..."
	unzip -o ${ARCHIVE_PATH} -d ${TOOL_PATH} > /dev/null
}

# function   : install_sdk_from_build
# description: Install target SDK from build.
#              SDK version is from package_*_index.json(${JSON_FILE})
# $1         : Package name
# $2         : Board name
# $3         : Json file name
# $4         : Arduino IDE Host name(Windows/Linux32/Linux64/Mac)
# $5         : Target tool name
function install_sdk_from_build()
{
	PKG_NAME=$1
	BRD_NAME=$2
	JSN_NAME=${ARDUINO_DIR}/$3
	TARGET_HOST=$4
	SDK_NAME=$5
	VARIANT_NAME=$6

	echo "Install SDK from Spresense build..."
	debug_print "Using ${SPRESENSE_SDK_PATH}"
	if [ ! -d "${SPRESENSE_SDK_PATH}" ]; then
		echo "No such directory(${SPRESENSE_SDK_PATH})"
		exit
	fi

	# Get SDK version from json
	export SDK_VERSION=`${JSN_LOADER} -j ${JSN_NAME} -p ${PKG_NAME} -b ${BRD_NAME} -t ${SDK_NAME} -H ${AURDUINO_IDE_HOST} -k version`
	export VARIANT_NAME=${VARIANT_NAME}

	# Error handling
	if [ "${SDK_VERSION}" == "" ]; then
		echo "ERROR: Cannot detect SDK version. Please check ${JSN_NAME}."
		exit
	fi

	# Get SDK comonent configuration
	export SDL_KERNEL_CONF=${SDL_KERNEL_CONF}
	export SDK_CONFIG=`cat ${SCRIPT_DIR}/configs/${VARIANT_NAME}.conf | head -n 1`

	# Export SDK build
	${SCRIPT_DIR}/sdk_export.sh ${SPRESENSE_SDK_PATH}

	# Import SDK build into Arduino15
	${SCRIPT_DIR}/sdk_import.sh ${SPRESENSE_SDK_PATH}/sdk-export.zip
}

# Option handler
# -S: Spresense source path "your/path/to/spresense"
# -g: gcc archive path "your/path/to/gcc.zip"
# -s: sdk archive path "your/path/to/sdk.zip"
# -H: target Arduino Host (Windows/Linux32/Linux64/Mac)
# -p: Use private access (Skip GCC installation)
SPRESENSE_SDK_PATH=""
GCC_ARCHIVE_PATH=""
SDK_ARCHIVE_PATH=""
SDK_VARIANT_NAME="generic"
SDL_KERNEL_CONF="release"
AURDUINO_IDE_HOST=""
PRIVATE_ACCESS=""
while getopts S:g:s:v:k:H:ph OPT
do
	case $OPT in
		'S' ) SPRESENSE_SDK_PATH=$OPTARG;;
		'g' ) GCC_ARCHIVE_PATH=$OPTARG;;
		's' ) SDK_ARCHIVE_PATH=$OPTARG;;
		'v' ) SDK_VARIANT_NAME=$OPTARG;;
		'k' ) SDL_KERNEL_CONF=$OPTARG;;
		'H' ) AURDUINO_IDE_HOST=$OPTARG;;
		'p' ) PRIVATE_ACCESS=true;;
		'h' ) show_help;;
	esac
done

#
# Main stream
#

# Option Errror handling
if [ "${AURDUINO_IDE_HOST}" == "" ]; then
	echo "ERROR: Arduino IDE Host is invalid. Please use Windows/Linux32/Linux64/Mac"
	show_help
	exit
fi

# GCC install
if [ "${PRIVATE_ACCESS}" == "true" ]; then
	echo "Skip GCC installation."
elif [ "${GCC_ARCHIVE_PATH}" != "" ]; then
	# Using arvhive install
	install_tool_from_archive ${PACKAGE_NAME} ${GCC_NAME} ${GCC_ARCHIVE_PATH}
else
	# Using HTTP install
	install_tool_from_http ${PACKAGE_NAME} ${BOARD_NAME} ${JSON_FILE} ${AURDUINO_IDE_HOST} ${GCC_NAME}
fi

# SDK install
if [ "${SDK_ARCHIVE_PATH}" != "" ]; then
	# Using arvhive install
	install_tool_from_archive ${PACKAGE_NAME} ${SDK_NAME} ${SDK_ARCHIVE_PATH}
elif [ "${SPRESENSE_SDK_PATH}" != "" ]; then
	# Using SDK build
	install_sdk_from_build ${PACKAGE_NAME} ${BOARD_NAME} ${JSON_FILE} ${AURDUINO_IDE_HOST} ${SDK_NAME} ${SDK_VARIANT_NAME}
else
	# Using HTTP install
	install_tool_from_http ${PACKAGE_NAME} ${BOARD_NAME} ${JSON_FILE} ${AURDUINO_IDE_HOST} ${SDK_NAME}
fi

