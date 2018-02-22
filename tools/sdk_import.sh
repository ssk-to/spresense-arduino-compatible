#!/bin/bash
if [ $# != 1 ]; then
	echo "Usage: $0 <SDK2.0_Package_file>"
	exit 1
fi

(echo $1 | egrep -q "zip$|ZIP$")
if [ $? != 0 -o ! -f $1 ]; then 
	echo "Please input a ZIP file as SDK package"
	exit 1
fi

T=`readlink -f $0`
SCRIPT_DIR=`dirname $T`
SDK_DIR="${SCRIPT_DIR}/../packages/spritzer/hardware/spritzer/0.1.0"
SDK_DIR=`readlink -f $SDK_DIR`

echo $SCRIPT_DIR
echo $SDK_DIR

if [ -d $SDK_DIR ]; then
rm -rf $SDK_DIR/sdk
unzip $1 -d $SDK_DIR
fi
