#!/bin/sh

# Created by Git user d029940, Jul-2022
# Feel free to adjust it to your liking

########################################################################
# !!!! Attention
#
# If you only want to run this script without running the build -> source this file
# Otherwise: sh this file
#
########################################################################


#
# First steps todo before running this script
# ----------
# 1. Create a build directory for QMS (refered here as QMSDEVDIR)
######################################################################## 
# Shell variables for the build process
# During the build process a lot of shell variables are needed to facilitate the process and thereby still offering flexibility for changes
# set to a folder, where the complete build of QMapShack and all parts (refer to first step in the beginning of this script)
# cd to this folder

# Some vars
######################################################################## 
# Color echo output (only to emphasize the stages in the build process)
export GREEN=$(tput setaf 2)
export RED=$(tput setaf 1)
export NC=$(tput sgr0)

# root dir for building QMS
export QMSDEVDIR=$PWD
# location of build files
SRC_OSX_DIR=$QMSDEVDIR/qmapshack/MacOSX

# build Xcode project (=1) or release (=0)
if [[ "$XCODE_PROJECT" == "" ]]; then
    export XCODE_PROJECT=0
fi

# build GDAL (experimental: now using cmake)
if [[ "$BUILD_GDAL" == "" ]]; then
export BUILD_GDAL=0
fi

# 2. cd into QMSDEVDIR
cd $QMSDEVDIR

# 3. Fetching QMapShack from Github
export GREEN=$(tput setaf 2)
echo "${GREEN}Fetching QMapShack ...${NC}"
# Fetching my fork of QMapShack from Github (again)
echo "${GREEN}Is cloned fork up-to-date?${NC}"
git clone https://github.com/d029940/qmapshack.git qmapshack

echo "${GREEN}Press key to start building process${NC}"
echo "${GREEN}You can go to $SRC_OSX_DIR dir to check build scripts${NC}"
echo "${GREEN}Please read $SRC_OSX_DIR/README.md${NC}"
read -n 1 -s

sh $SRC_OSX_DIR/build-all.sh
