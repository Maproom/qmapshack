#!/bin/sh

DIR_SCRIPT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"  # absolute path to the dir of this script
source $DIR_SCRIPT/config.sh   # check for important paramters

######################################################################## 
#
# install homebrew (if needed) and packages

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
echo "${ATTN}Fetching brew packages (cmake, qt@5, proj, gdal, geos and dependencies ...${NC}"
rm -f $QMSDEVDIR/brewlist*.txt
brew list > $QMSDEVDIR/brewlist-`date +%s`.txt
brew install cmake
brew install qt@5

brew install dbus
brew install proj
if [[ "$BUILD_GDAL" == "" ]]; then
    brew install gdal
fi
brew install geos

# # Used by routino
# brew install bzip2
brew list > $QMSDEVDIR/brewlist-`date +%s`.txt
diff $QMSDEVDIR/brewlist*.txt > $QMSDEVDIR/brewdiff-`date +%s`.txt
