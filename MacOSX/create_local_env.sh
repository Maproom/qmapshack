#!/bin/sh

# Create a local environment for all includes, libs, ... needed
source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important parameters

mkdir $LOCAL_DEV_PATH
mkdir $LOCAL_DEV_PATH/bin
mkdir $LOCAL_DEV_PATH/lib
mkdir $LOCAL_DEV_PATH/include
mkdir $LOCAL_DEV_PATH/share
mkdir $LOCAL_DEV_PATH/xml
