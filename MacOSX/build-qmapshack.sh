#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/env-path.sh


function installLibraies {
    if ! [ -x "$(command -v brew)" ]; then
        ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    fi
    brew install caskroom/cask/brew-cask
    
    brew install qt5
    brew install gdal
    brew install proj4
}


# mhg update
# mhg pull

function makeXcodePrj {
    
    if [  -d $BUILD_DIR ]; then
        rm -rf $BUILD_DIR  
    fi
    mkdir $BUILD_DIR
    cd $BUILD_DIR
    cmake -G Xcode -D CMAKE_PREFIX_PATH=$QT_DIR -D CMAKE_OSX_DEPLOYMENT_TARGET=10.5 -D ROUTINO_DEV_PATH=$LIB_ROUTINO_DIR $SRC_QMAPSHACK_DIR
}

function setFormatter {
    cp ./clang-format /~/.clang-format
}


if [[ "$1" == "format" ]]; then
    setFormatter
fi

if [[ "$1" == "xcode" ]]; then
    makeXcodePrj
fi

if [[ "$1" == "pre-install" ]]; then
    installLibraies
fi

# -d -h -c 
if [[ "$1" == "run" ]]; then
    $BUILD_BUNDLE_APP_FILE $2 $3$4 $5 $6
fi