#!/bin/sh

# Create a local environment for all includes, libs, ... needed
source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

mkdir $LOCAL_ENV
mkdir $LOCAL_ENV/bin
mkdir $LOCAL_ENV/lib
mkdir $LOCAL_ENV/include
mkdir $LOCAL_ENV/share
mkdir $LOCAL_ENV/xml
