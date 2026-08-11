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

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

echo "${INFO}Are these parameters correct?${NC}"
echo "${INFO}Is your build dir outside of the qmapshack source dir (cloned from GitHub)?${NC}"
echo "${INFO}Are you in the correct git branch? Check with \"git status\" $PWD${NC}"
echo "${INFO}Please read $SRC_OSX_DIR/README.md${NC}"
echo "${INFO}Press key to start building process${NC}"
read -n 1 -s


######################################################################## 
# clean up
if [ "$1" = "clean" ]; then
    unset INCLUDED
    source $SRC_OSX_DIR/clean.sh
fi

########################################################################
# Create local environment
source $SRC_OSX_DIR/create_local_env.sh

########################################################################
# install homebrew (if needed) and packages / macports
cd $QMSDEVDIR
source  $SRC_OSX_DIR/install-packages.sh
cd $QMSDEVDIR

######################################################################## 
# build otoolrecursive

cd $QMSDEVDIR
source $SRC_OSX_DIR/build-otoolrecursive.sh
cd $QMSDEVDIR

######################################################################## 
# build Proj
if [ -z "$MACPORTS_BUILD" ]; then
   if [ "$BUILD_PROJ" = "x" ]; then
        cd $QMSDEVDIR
        source $SRC_OSX_DIR/build-proj.sh
        cd $QMSDEVDIR
    fi
fi

######################################################################## 
# build GDAL
if [ -z "$MACPORTS_BUILD" ]; then
   if [ "$BUILD_GDAL" = "x" ]; then
        cd $QMSDEVDIR
        source $SRC_OSX_DIR/build-gdal.sh
        cd $QMSDEVDIR
    fi
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

# Bundling QMapShack and QMapTool
source $SRC_OSX_DIR/bundle-all.sh
cd $QMSDEVDIR
