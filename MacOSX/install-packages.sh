#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Installing packages ...${NC}"
echo "${ATTN}-----------------------${NC}"

######################################################################## 
#
# install homebrew (if needed) and packages / macports

if [ -z "$MACPORTS_BUILD" ]; then

    ######################################################################## 
    # install homebrew  (https://brew.sh)
    #       /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    # HomwBrew location (this can also be put into $HOME/.zshrc, if homebrew is used anyway)
    if command -v brew > /dev/null 2>&1; then
        brew update
    else
        # Install Homebrew
        /bin/sh -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    eval "$(brew shellenv)"

    ######################################################################## 
    # install all necessary packages with homebrew.
    # Attention: there will be installed additional packages due to dependencies
    # First get all currently installed brew packages to be able to find out which packahes will be installed
    # during QMS build
    echo "${ATTN}Fetching brew packages ${NC}"
    rm -f $QMSDEVDIR/brewlist*.txt
    brew list > $QMSDEVDIR/brewlist-`date +%s`.txt
    brew install cmake
    brew install qt@6

    brew install dbus

    # packages to build proj
    if [[ "$BUILD_PROJ" == "x" ]]; then
        brew install libtiff
    else
        brew install proj
    fi

    brew install geos
    # Packages to build gdal
    if [[ "$BUILD_GDAL" == "x" ]]; then
        brew install boost
        brew install openjpeg
        brew install libkml
        brew install minizip
        brew install uriparser
        brew install unixodbc
        brew install libtiff
        brew install libgeotiff
        brew install libheif
        brew install geos
    else
        brew install gdal
    fi


    # # Used by routino
    # brew install bzip2
    brew list > $QMSDEVDIR/brewlist-`date +%s`.txt
    diff $QMSDEVDIR/brewlist*.txt > $QMSDEVDIR/brewdiff-`date +%s`.txt

else
     # install packages from macports
    echo "${ATTN}Fetching mac ports ${NC}"
    rm -f $QMSDEVDIR/portlist*.txt
    port installed > $QMSDEVDIR/portlist-`date +%s`.txt

    # install mac ports
    # sudo port -v selfupdate
    # sudo port upgrade outdated
     # qt5
    sudo port install qt5
    sudo port install qt5-qtwebengine
    # if quazip is not compiled from source
    sudo port install quazip1
    # if routino is not compiled from source
    sudo port install routino
    # if gdal is not compiled from source
    sudo port install zstd
    sudo port install jpeg
    sudo port install tiff
    sudo paort install proj
    sudo port install gdal

    sudo port install cmake

    port installed > $QMSDEVDIR/portlist-`date +%s`.txt
    diff $QMSDEVDIR/portlist*.txt > $QMSDEVDIR/portsdiff-`date +%s`.txt
fi
