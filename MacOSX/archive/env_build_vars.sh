#!/bin/sh
# source this file, like source env_build_vars

######################################################################## 
# Color echo output (only to emphasize the stages in the build process)
export GREEN=$(tput setaf 2)
export RED=$(tput setaf 1)
export NC=$(tput sgr0)

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}You need to set QMSDEVDIR shell var: export QMSDEVDIR=<root of your build directory> ${NC}"
    return
fi

# Use Macports (1) or Homebrew (0)
export MACPORTS=0

# build GDAL (experimental: now using cmake)
export BUILD_GDAL=0

# MacOS build target
export OSX_DEPLOYMENT_TARGET=10.9

# QMapShack
export BUILD_DIR=$QMSDEVDIR/build_QMapShack
export QMS_SRC_DIR=$QMSDEVDIR/QMapShack
export BUILD_BIN_DIR=$QMSDEVDIR/build_QMapShack/bin
export BUILD_RELEASE_DIR=$QMSDEVDIR/release
export QMS_BUILD_FILES=$QMS_SRC_DIR/MacOSX/QMS_build_files

if [[ MACPORTS -eq 1 ]]
then
    export PKGMANAGER=/opt/local
    # Check location of Qt
    export QT_DIR=$PKGMANAGER/libexec/qt5 
else
    export PKGMANAGER=$HOMEBREW_PREFIX
    # Check location of Qt
    export QT_DIR=$PKGMANAGER/opt/qt5
fi

# Local packages 
export LOCAL_ENV=$QMSDEVDIR/local

# Check location of Qt (from Homebrew)
export QT_DIR=$PKGMANAGER/opt/qt5
export Qt5_DIR=$QT_DIR/lib/cmake

# Routino
if [[ MACPORTS -eq 1 ]]
then 
    export ROUTINO_LIB_DIR=$PKGMANAGER
    export ROUTINO_DEV_PATH=$ROUTINO_LIB_DIR
else
    export ROUTINO_ORG=$QMSDEVDIR/routino
    export ROUTINO_LIB_DIR=$LOCAL_ENV
    export ROUTINO_DEV_PATH=$ROUTINO_LIB_DIR 
fi


# export PROJ_DIR=$PKGMANAGER
export PROJ_DIR=$LOCAL_ENV
export DBUS=$PKGMANAGER/lib
if [[ $BUILD_GDAL -eq 1 ]]; then
    export GDAL_DIR=$LOCAL_ENV
else
    export GDAL_DIR=$PKGMANAGER
fi  
export GEOS_LIB_DIR=$PKGMANAGER/lib

# Quazip
export QUAZIP_Version=Qt5-1.3
export QUAZIP_LIB_DIR=$LOCAL_ENV
