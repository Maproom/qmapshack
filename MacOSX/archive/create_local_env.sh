#!/bin/sh

# Create a local environment for all includes, libs, ... needed

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
    return
fi
source $SRC_OSX_DIR/env-path.sh

mkdir $LOCAL_ENV
mkdir $LOCAL_ENV/bin
mkdir $LOCAL_ENV/lib
mkdir $LOCAL_ENV/include
mkdir $LOCAL_ENV/share
mkdir $LOCAL_ENV/xml
