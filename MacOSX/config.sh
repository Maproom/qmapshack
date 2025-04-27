#!/bin/sh

######################################################################## 
#
# Check prerequisites for building (parts of) QMS
#
# Also set some vars for coloring output
######################################################################## 

# only include once
if [ -z "$INCLUDED" ]; then
    export INCLUDED="yes"
else
    return
fi

# Some vars
######################################################################## 
# Color echo output (only to emphasize the stages in the build process)
export INFO=$(tput setaf 2)   # green
export ATTN=$(tput setaf 1)  # red
export NC=$(tput sgr0)

export QMSDEVDIR=${QMSDEVDIR%/} # remove trailing slash, if there is any
export BUILD_QMAPSHACK_DIR="$QMSDEVDIR/build_QMapShack"

# ARM64 or Intel(?)
export CMAKE_OSX_ARCHITECTURES=$(uname -m)

echo ${ATTN}
echo "-------------------------------------------------"
echo "This shell script dir (DIR_OF_SCRIPT) =           $DIR_SCRIPT"
echo "Source dir of qmapshack (cloned from GitHub) =    ${DIR_SCRIPT%/*/*}/qmapshack"
echo "You're in                                         $PWD"
echo "QMSDEVDIR (build directory) shell var =           $QMSDEVDIR"
echo "-------------------------------------------------"
echo ${NC}

# Check if the main shell var QMSDEVDIR is set
# Check if you build qmapshack outside of the source dir (cloned from GitHub)
########################################################################

if [ "$QMSDEVDIR" = "" ]; then
    echo $ATTN
    echo "shell var QMSDEVDIR is not set"
    echo "Please export QMSDEVDIR to a folder outside of qmapshack"
    echo "Proposal - copy this in your terminal and hit enter:"
    echo "export QMSDEVDIR=${DIR_SCRIPT%/*/*}"
    echo $NC
    exit
fi

# echo $DIR_SCRIPT
echo ${DIR_SCRIPT%/*}
# echo ${DIR_SCRIPT%/*}/*
if [ "$QMSDEVDIR" = "$DIR_SCRIPT" ] || [ "$QMSDEVDIR" = "${DIR_SCRIPT%/*}" ]; then
    echo $ATTN
    echo "Your shell var QMSDEVDIR points to a driectory inside qmapshack"
    echo "Please export QMSDEVDIR to a folder outside of qmapshack"
    echo "Proposal - copy this in your terminal and hit enter:"
    echo "export QMSDEVDIR=${DIR_SCRIPT%/*/*}"
    echo $NC
    exit
fi

# Parameters for the build
########################################################################

# Default: Build with homebrew packages and copz the libs and bins into the bundle

# build with macports
# if set with any value -> build with macports
# if an empty string -> build with homebrew
# export MACPORTS_BUILD="x"
export MACPORTS_BUILD=

# build with brew package, but do not copy libs and bins from the brew package
# into the bundle, i.e. the brew packages are nedded at runtime 
# if set with any value -> do not copy brew packages into the bundle
# if an empty string -> create self-contained bundle
export BREW_PACKAGE_BUILD=

# build Xcode project
# if set with any value -> create an Xcode proj
# if an empty string -> compile, build and nundle w/o XCode
export XCODE_PROJECT=


# GDAL: if set to "x", it will be built from source. 
# If not set (i.e. blank), GDAL will be taken from the package manager
export BUILD_GDAL="x"
# PROJ (still experimental): if set to "x", it will be built from source. 
# If not set (i.e. blank), PROJ will be taken from the package manager
export BUILD_PROJ="x"


# checking arguments: intested in -x (Xcode), -m (MacPorts), -b (Homebrew)
while getopts ":bmx" opt; do
  case $opt in
    b)
        export BREW_PACKAGE_BUILD="x"
        export MACPORTS_BUILD=
        shift
        ;;
    m)
        export MACPORTS_BUILD="x"
        export BREW_PACKAGE_BUILD=
        shift
        ;;
    x)
        export XCODE_PROJECT="x"
        shift
        ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
        exit
        ;;
  esac
done

########################################################################
# Paramters driving the build process
echo $INFO
echo "Parameters driving the build process:"
echo "-------------------------------------"
echo "Brew pkg needed at run time = ${BREW_PACKAGE_BUILD}"
echo "Build with MacPorts = ${MACPORTS_BUILD}"
echo "Xcode build = ${XCODE_PROJECT}"
echo "-------------------------------------"
echo ${NC}


########################################################################
# Addtional Vars / Paramters for building

export LOCAL_ENV=$QMSDEVDIR/local  # folder for building pkgs from source

# package manager
eval "$(brew shellenv)"   # set HOMEBREW_PREFIX
export PACKAGES_PATH=$HOMEBREW_PREFIX
if [ "$MACPORTS_BUILD" = "x" ]; then
    export MACPORTS=/opt/local          # Macports package manager
    export HOMEBREW_PREFIX=
    export BREW_PACKAGE_BUILD=
    export PACKAGES_PATH=$MACPORTS
fi

# Directories for QMS
export BUILD_RELEASE_DIR=$QMSDEVDIR/release # app bundles will be put
export QMS_SRC_DIR=$QMSDEVDIR/qmapshack # QMS source dir (clone from GitHub)
export SRC_OSX_DIR=$QMSDEVDIR/qmapshack/MacOSX # Sources only for MacOS

# QT6
if [ "$MACPORTS_BUILD" = "x" ]; then
    export QT_DEV_PATH=$PACKAGES_PATH/libexec/qt6
else
    export QT_DEV_PATH=$PACKAGES_PATH/opt/qt@6
fi
export Qt6_DIR=$QT_DEV_PATH/lib/cmake

# Other packages needed
if [ "$MACPORTS_BUILD" = "x" ]; then
    export GDAL=$PACKAGES_PATH
    export ROUTINO_DEV_PATH=$PACKAGES_PATH
    export PROJ_DEV_PATH=$PACKAGES_PATH/lib/proj9
    export QuaZip_Qt6_DIR=$PACKAGES_PATH/lib/cmake/QuaZip-Qt6-1.5
else
    # GDAL, ROUTINO, QUAZIP, PROJ are compiled from source
    if [ "$BUILD_GDAL" = "x" ]; then
        export GDAL_RELEASE="3.10"
        export GDAL=$LOCAL_ENV
    else
        export GDAL=$PACKAGES_PATH
    fi
    export ROUTINO_DEV_PATH=$LOCAL_ENV
    if [ "$BUILD_PROJ" = "x" ]; then
        export PROJ_DEV_PATH=$LOCAL_ENV
    else
        export PROJ_DEV_PATH=$PACKAGES_PATH
    fi
export QuaZip_Qt6_DIR=$LOCAL_ENV/lib/cmake/QuaZip-Qt6-1.5
fi

# env vars for building QMS
export OSX_DEPLOYMENT_TARGET=12.0  # MacOS build target

# prefer local development environment:
export PATH=$LOCAL_ENV/bin:$PATH

########################################################################
# print all config variables
echo $INFO
echo "Config variables:"
echo "-------------------------------------"
echo "MACPORTS = $MACPORTS"
echo "HOMEBREW_PREFIX = $HOMEBREW_PREFIX"
echo "PACKAGES_PATH = $PACKAGES_PATH"
echo "LOCAL_ENV = $LOCAL_ENV"
echo "BREW_PACKAGE_BUILD = $BREW_PACKAGE_BUILD"
echo "MACPORTS_BUILD = $MACPORTS_BUILD"
echo "BUILD_RELEASE_DIR = $BUILD_RELEASE_DIR"
echo "QMS_SRC_DIR = $QMS_SRC_DIR"
echo "SRC_OSX_DIR = $SRC_OSX_DIR"
echo "QT_DEV_PATH = $QT_DEV_PATH"
echo "Qt6_DIR = $Qt6_DIR"
echo "BUILD_GDAL = $BUILD_GDAL"
echo "GDAL = $GDAL"
echo "GDAL_RELEASE = $GDAL_RELEASE"
echo "ROUTINO_DEV_PATH = $ROUTINO_DEV_PATH"
echo "BUILD_PROJ = $BUILD_PROJ"
echo "PROJ_DEV_PATH = $PROJ_DEV_PATH"
echo "QuaZip_Qt6_DIR = $QuaZip_Qt6_DIR"
echo "OSX_DEPLOYMENT_TARGET = $OSX_DEPLOYMENT_TARGET"
echo "-------------------------------------"
echo ${NC}

cd $QMSDEVDIR
