#!/bin/sh

######################################################################## 
#
# Check prerequisites for building (parts of) QMS
#
# Also set some vars for coloring output
######################################################################## 

# only include onece
if [[ -z $INCLUDED ]]; then
export INCLUDED="yes"

# Some vars
######################################################################## 
# Color echo output (only to emphasize the stages in the build process)
export INFO=$(tput setaf 2)   # green
export ATTN=$(tput setaf 1)  # red
export NC=$(tput sgr0)

export QMSDEVDIR=${QMSDEVDIR%/} # remove trailing slash, if there is any

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

if [[ "$QMSDEVDIR" == "" ]]; then
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
if [[ "$QMSDEVDIR" == "$DIR_SCRIPT" ]] || [[ "$QMSDEVDIR"  ==  ${DIR_SCRIPT%/*}  ]]; then
    echo $ATTN
    echo "Your shell var QMSDEVDIR points to a driectory inside qmapshack"
    echo "Please export QMSDEVDIR to a folder outside of qmapshack"
    echo "Proposal - copy this in your terminal and hit enter:"
    echo "export QMSDEVDIR=${DIR_SCRIPT%/*/*}"
    echo $NC
    exit
fi

# root dir for building QMS


# Parameters for the build
########################################################################

# build GDAL from source
# if set with any value -> build GDAL from source
# if an empty string -> take GDAL from homebrew
export BUILD_GDAL=

# preparing to create a brew-package
# if set with any value -> prepare for QMS brew package
# if an empty string -> create self-contained bundle
export BREW_PACKAGE_BUILD=

# build Xcode project
# if set with any value -> create an Xcode proj
# if an empty string -> compile, build and nundle w/o XCode
export XCODE_PROJECT=

# checking arguments: intested in -x (Xcode), -g (GDAL), -b (Homebrew)
while getopts ":bgx" opt; do
  case $opt in
    b)
        export BREW_PACKAGE_BUILD="x"
        shift
        ;;
    g)
        export BUILD_GDAL="x"
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
echo "Brew pkg built = ${BREW_PACKAGE_BUILD}"
echo "Build GDAL from source = ${BUILD_GDAL}"
echo "Xcode build = ${XCODE_PROJECT}"
echo "-------------------------------------"
echo ${NC}

fi  #include once

########################################################################
# Addtional Vars / Paramters for building

export LOCAL_ENV=$QMSDEVDIR/local  # folder for building pkgs from source

# Directories for QMS
export BUILD_RELEASE_DIR=$QMSDEVDIR/release # app bundles will be put
export QMS_SRC_DIR=$QMSDEVDIR/qmapshack # QMS source dir (clone from GitHub)
export SRC_OSX_DIR=$QMSDEVDIR/qmapshack/MacOSX # Sources only for MacOS

# QT5
export QT_DIR=$HOMEBREW_PREFIX/opt/qt5
export Qt5_DIR=$QT_DIR/lib/cmake

# GDAL
if [ -z "${BUILD_GDAL}" ]; then
    export GDAL_DIR=`brew --prefix gdal`
else
    export GDAL_DIR=$LOCAL_ENV
fi

cd $QMSDEVDIR

