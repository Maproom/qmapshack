#!/bin/sh

# Created by Git user d029940, Jul-2022
# Feel free to adjust it to your liking

######################################################################## 
#
# Prequisite: 
# 1. A build directory for QMS (refered here as QMSDEVDIR) has been created
# 2. The shell var QMSDEVDIR has been set QMSDEVDIR to this dir
#    export $QMSDEVDIR=<QMSDEVDIR>
# 2. The script 1st_QMS_start.sh has been copied to $QMSDEVDIR and ran
#       OR
# 2a. QMapShack has been downloaded from git (git clone ...)
######################################################################## 

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
    return
fi

cd $QMSDEVDIR
SRC_OSX_DIR=$QMSDEVDIR/QMapShack/MacOSX


######################################################################## 
# clean up
if [[ "$1" == "clean" ]]; then

    source $SRC_OSX_DIR/clean.sh
    exit
fi

########################################################################
# Create local environment
source $SRC_OSX_DIR/create_local_env.sh

########################################################################
# install homebrew (if needed) and packages 
cd $QMSDEVDIR
source  $SRC_OSX_DIR/install-brew-packages.sh
cd $QMSDEVDIR

######################################################################## 
# build Quazip
cd $QMSDEVDIR
source $SRC_OSX_DIR/build-quazip.sh
cd $QMSDEVDIR

######################################################################## 
# build GDAL (experimental: now using cmake)
if [[ $BUILD_GDAL -eq 1 ]]; then
    cd $QMSDEVDIR
    source $SRC_OSX_DIR/build-gdal.sh
    cd $QMSDEVDIR
fi

######################################################################## 
# build Routino
cd $QMSDEVDIR
source $SRC_OSX_DIR/build-routino.sh
cd $QMSDEVDIR

# build QMapShack
cd $QMSDEVDIR
sh $SRC_OSX_DIR/build-QMS.sh
cd $QMSDEVDIR

if [[ $XCODE_PROJECT -eq 1 ]]; then
    echo "${GREEN}Xcode project written in $QMSDEVDIR/build_QMapShack${NC}"
    exit
fi

# Bundling QMapShack and QMapTool
# source $QMS_BUILD_FILES/bundle.sh
source $SRC_OSX_DIR/bundle-all.sh
cd $QMSDEVDIR

######################################################################## 
# Last remark - Dark Mode

# If the system is running in dark mode, then buttons which appear on top of a map are not visible (e.g. while adjusting a track)
# These buttons are not dark-mode enabled.
# Solution:
# echo "${RED}Remark: QMapShack cannot show buttons on the map in dark mode.${NC}"
# Add the following key to the "info.plist" file, i.e. completely opting out from dark mode
# <key>NSRequiresAquaSystemAppearance</key> <string>true</string>
# The "info.plist" file can be found in the bundle of the app under the "Contents" folder
# echo "${GREEN}QMapShack can ignore dark mode by adding the following key to the \"info.plist\" file.${NC}"
# echo "${GREEN}<key>NSRequiresAquaSystemAppearance</key> <string>true</string>${NC}"
# echo "${GREEN}The \"info.plist\" file can be found in the bundle of the app under the \"Contents\" folder,${NC}"


