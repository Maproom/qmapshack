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
# cloned source location from git
export QMS_SRC_DIR=$QMSDEVDIR/QMapShack
# location of build files
export SRC_OSX_DIR=$QMS_SRC_DIR/MacOSX

# build Xcode project (=1) or release (=0)
export XCODE_PROJECT=0

# build GDAL (experimental: now using cmake)
export BUILD_GDAL=0

# 2. cd into QMSDEVDIR
cd $QMSDEVDIR

# 3. Fetching QMapShack from Github
export GREEN=$(tput setaf 2)
echo "${GREEN}Fetching QMapShack ...${NC}"
# Fetching my fork of QMapShack from Github (again)
echo "${GREEN}Is cloned fork up-to-date?${NC}"
git clone https://github.com/d029940/qmapshack.git $QMS_SRC_DIR

source $SRC_OSX_DIR/env-path.sh

echo "${GREEN}Press key to start building process${NC}"
echo "${GREEN}You can go to $SRC_OSX_DIR dir to check build scripts${NC}"
echo "${GREEN}Important paramters are set in $SRC_OSX_DIR/env-path.sh${NC}"
read -n 1 -s

sh $SRC_OSX_DIR/build-all.sh
