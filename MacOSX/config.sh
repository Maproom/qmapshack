#!/bin/sh

########################################################################
#
# Check prerequisites for building (parts of) QMS
#
# Also set some variables for coloring output
########################################################################

# Only include once!
if [ -z "$INCLUDED" ]; then
    export INCLUDED="yes"
else
    return
fi

# Some variables
########################################################################
# Color echo output (only to emphasize the stages in the build process)
export INFO=$(tput setaf 2)  # green
export ATTN=$(tput setaf 1)  # red
export NC=$(tput sgr0)

export QMSDEVDIR=${QMSDEVDIR%/} # remove trailing slash, if there is any

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
    echo "Your shell var QMSDEVDIR points to a folder inside qmapshack"
    echo "Please export QMSDEVDIR to a folder outside of qmapshack"
    echo "Proposal - copy this in your terminal and hit enter:"
    echo "export QMSDEVDIR=${DIR_SCRIPT%/*/*}"
    echo $NC
    exit
fi

# Parameters for the build
########################################################################

# Default: Build with homebrew packages and copy the libs and bins into the bundle

# Build with macports
# If set with any value -> build with macports
# If an empty string -> build with homebrew
# export MACPORTS_BUILD="x"
export MACPORTS_BUILD=

# Build with brew package, but do not copy libs and bins from the brew package
# into the bundle, i.e. the brew packages are needed at runtime
# If set with any value -> do not copy brew packages into the bundle
# If an empty string -> create self-contained bundle
export BREW_PACKAGE_BUILD=

# Build type: one of "Release", "RelWithDebInfo" or "Debug"
# Command line argument "-d" overrides default "Release" by "RelWithDebInfo"
# to create a debuggable build
export BUILD_TYPE=Release


# GDAL: if set to "x", it will be built from source.
# If not set (i.e. blank), GDAL will be taken from the package manager
export BUILD_GDAL="x"
# PROJ: if set to "x", it will be built from source.
# If not set (i.e. blank), PROJ will be taken from the package manager
export BUILD_PROJ="x"


# Checking arguments:
# interested in -d (Debug), -x (Xcode), -m (MacPorts), -b (Homebrew)
while getopts ":bmxd" opt; do
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
        echo "${ATTN}"
        echo "Xcode project creation not supported!"
        echo "To create a debuggable build instead ..."
        echo "- run with command line argument '-d'"
        echo "- use 'lldb' debugger for debugging."
        echo "${NC}"
        exit 1
        shift
        ;;
    d)
        export BUILD_TYPE="RelWithDebInfo"
        shift
        ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
        exit
        ;;
  esac
done

########################################################################
# Parameters driving the build process
echo $INFO
echo "Parameters driving the build process:"
echo "-------------------------------------"
echo "Brew pkg needed at run time = ${BREW_PACKAGE_BUILD}"
echo "Build with MacPorts = ${MACPORTS_BUILD}"
echo "-------------------------------------"
echo ${NC}


########################################################################
# Additional variables/parameters for building

export LOCAL_DEV_PATH=$QMSDEVDIR/local  # folder for building pkgs from source

# Package manager
eval "$(brew shellenv)"   # set HOMEBREW_PREFIX
export PACKAGES_PATH=$HOMEBREW_PREFIX
if [ "$MACPORTS_BUILD" = "x" ]; then
    export MACPORTS=/opt/local          # Macports package manager
    export HOMEBREW_PREFIX=
    export BREW_PACKAGE_BUILD=
    export PACKAGES_PATH=$MACPORTS
fi

# Directories for QMS
export BUILD_QMAPSHACK_DIR="$QMSDEVDIR/build/$BUILD_TYPE"  # QMS build dir
export BUILD_RELEASE_DIR=$QMSDEVDIR/release/$BUILD_TYPE    # app bundles will be put
export QMS_SRC_DIR=$QMSDEVDIR/qmapshack                    # QMS source dir (clone from GitHub)
export QMS_OSX_DIR=$QMSDEVDIR/qmapshack/MacOSX             # Sources only for MacOS

# Qt6
if [ "$MACPORTS_BUILD" = "x" ]; then
    export QT_DEV_PATH=$PACKAGES_PATH/libexec/qt6
else
    export QT_DEV_PATH=$PACKAGES_PATH/opt/qt@6
fi
export Qt6_DIR=$QT_DEV_PATH/lib/cmake

# Other packages needed
if [ "$MACPORTS_BUILD" = "x" ]; then
    export GDAL_DEV_PATH=$PACKAGES_PATH
    export ROUTINO_DEV_PATH=$PACKAGES_PATH
    export PROJ_DEV_PATH=$PACKAGES_PATH/lib/proj9
else
    # ROUTINO, GDAL, PROJ are compiled from source
    export ROUTINO_RELEASE="3.4.4"
    export ROUTINO_DEV_PATH=$LOCAL_DEV_PATH
    if [ "$BUILD_GDAL" = "x" ]; then
        export GDAL_RELEASE="3.13"
        export GDAL_DEV_PATH=$LOCAL_DEV_PATH
    else
        export GDAL_DEV_PATH=$(brew --prefix gdal)
    fi
    if [ "$BUILD_PROJ" = "x" ]; then
        export PROJ_RELEASE="9.9"
        export PROJ_DATA_RELEASE="1.25"
        export PROJ_DEV_PATH=$LOCAL_DEV_PATH
    else
        export PROJ_DEV_PATH=$(brew --prefix proj)
    fi
fi

# Environment variables for building QMS
export OSX_DEPLOYMENT_TARGET=12.0  # MacOS build target

# Prefer local development environment:
export PATH=$LOCAL_DEV_PATH/bin:$PATH

########################################################################
# Print all config variables
echo $INFO
echo "Config variables:"
echo "-------------------------------------"
echo "MACPORTS = $MACPORTS"
echo "HOMEBREW_PREFIX = $HOMEBREW_PREFIX"
echo "PACKAGES_PATH = $PACKAGES_PATH"
echo "LOCAL_DEV_PATH = $LOCAL_DEV_PATH"
echo "BREW_PACKAGE_BUILD = $BREW_PACKAGE_BUILD"
echo "MACPORTS_BUILD = $MACPORTS_BUILD"
echo "BUILD_TYPE = $BUILD_TYPE"
echo "BUILD_QMAPSHACK_DIR = $BUILD_QMAPSHACK_DIR"
echo "BUILD_RELEASE_DIR = $BUILD_RELEASE_DIR"
echo "QMS_SRC_DIR = $QMS_SRC_DIR"
echo "QMS_OSX_DIR = $QMS_OSX_DIR"
echo "QT_DEV_PATH = $QT_DEV_PATH"
echo "Qt6_DIR = $Qt6_DIR"
echo "ROUTINO_RELEASE = $ROUTINO_RELEASE"
echo "ROUTINO_DEV_PATH = $ROUTINO_DEV_PATH"
echo "BUILD_GDAL = $BUILD_GDAL"
echo "GDAL_RELEASE = $GDAL_RELEASE"
echo "GDAL_DEV_PATH = $GDAL_DEV_PATH"
echo "BUILD_PROJ = $BUILD_PROJ"
echo "PROJ_RELEASE = $PROJ_RELEASE"
echo "PROJ_DATA_RELEASE = $PROJ_DATA_RELEASE"
echo "PROJ_DEV_PATH = $PROJ_DEV_PATH"
echo "OSX_DEPLOYMENT_TARGET = $OSX_DEPLOYMENT_TARGET"
echo "-------------------------------------"
echo ${NC}

cd $QMSDEVDIR
