#!/bin/sh

# Create a local environment for all includes, libs, ... needed
DIR_SCRIPT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"  # absolute path to the dir of this script
source $DIR_SCRIPT/config.sh   # check for important paramters

mkdir $LOCAL_ENV
mkdir $LOCAL_ENV/bin
mkdir $LOCAL_ENV/lib
mkdir $LOCAL_ENV/include
mkdir $LOCAL_ENV/share
mkdir $LOCAL_ENV/xml
