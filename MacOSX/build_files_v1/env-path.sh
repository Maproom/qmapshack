#!/bin/sh
# source this file, like source env_build_vars

######################################################################## 
# Color echo output (only to emphasize the stages in the build process)
export GREEN=$(tput setaf 2)
export RED=$(tput setaf 1)
export NC=$(tput sgr0)

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
    return
fi

# MacOS build target
export OSX_DEPLOYMENT_TARGET=10.13

# QMapShack source dir and location of build files
export QMS_SRC_DIR=$QMSDEVDIR/QMapShack
export SRC_OSX_DIR=$QMS_SRC_DIR/MacOSX
# QMapShack build and release dirs
export BUILD_DIR=$QMSDEVDIR/build_QMapShack
export SRC_RESOURCES_DIR=$SRC_OSX_DIR/resources
export BUILD_BIN_DIR=$QMSDEVDIR/build_QMapShack/bin
export BUILD_RELEASE_DIR=$QMSDEVDIR/release

export PKGMANAGER=$HOMEBREW_PREFIX
# Check location of Qt
export QT_DIR=$PKGMANAGER/opt/qt5

# Local packages 
export LOCAL_ENV=$QMSDEVDIR/local

# Check location of Qt (from Homebrew)
export QT_DIR=$PKGMANAGER/opt/qt5
export Qt5_DIR=$QT_DIR/lib/cmake

# Routino

export ROUTINO_ORG=$QMSDEVDIR/routino
export ROUTINO_LIB_DIR=$LOCAL_ENV
export ROUTINO_DEV_PATH=$ROUTINO_LIB_DIR 

export PROJ_DIR=$PKGMANAGER
export DBUS=$PKGMANAGER/lib
if [[ $BUILD_GDAL -eq 1 ]]; then
    export GDAL_DIR=$LOCAL_ENV
else
    export GDAL_DIR=`brew --prefix gdal`
fi  
export GEOS_LIB_DIR=$PKGMANAGER/lib

# Quazip
export QUAZIP_Version=Qt5-1.3
export QUAZIP_LIB_DIR=$LOCAL_ENV

