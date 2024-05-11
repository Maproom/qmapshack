#!/bin/sh

source ./testParamters-sub.sh

# Getting the fullpath dir name of the running script
Directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"  # absolute path to the dir of this script

echo $Directory
echo $PWD
echo ${Directory%/*/*}

if [[ "$Directory" == "$PWD" ]] ; then
    echo "This script is in the current dir"
else
    echo "This script is NOT in the current dir"
fi

# Check variables with the z option. -z means checking of unset (ake not set or an empty string)
# Set to xxx= / xxx="" OR xxx="<anything>"
BUILD_GDAL=
BREW_PACKAGE_BUILD=1
XCODE_PROJECT=

if [[ -z "${XCODE_PROJECT}" ]]; then
    echo "Build w/o Xcode"
else
    echo "Build with Xcode"
fi
if [[ "${XCODE_PROJECT}" == "" ]]; then
    echo "Build w/o Xcode"
else
    echo "Build with Xcode"
fi

if [[ ! -z $BUILD_GDAL ]] || [[ -z $BREW_PACKAGE_BUILD  ]]  ; then
    echo "GDAL in bundle"
fi

if [[ ! -z $BUILD_GDAL ]]  ; then # NOT (GDAL not set or empty)
    if [[ -z $BREW_PACKAGE_BUILD ]]; then
        echo "Build GDAL, self-contained bundle"
    else
            echo "Build GDAL, brew pkg"
    fi
elif [[ -z $BREW_PACKAGE_BUILD  ]] # NOT (BUILD_STANDALONE not set or empty)
then
    echo "Build Self contained Bundle"
else
    echo "Brew pkg"
fi

# Check variables comparing to an argument with an empty string or unset (mimik z option, ususally easier to understand)

if [[ "$BUILD_GDAL" != "" ]] || [[ "$BREW_PACKAGE_BUILD" == ""  ]]  ; then
    echo "GDAL in bundle"
fi

if [[ "$BUILD_GDAL" != "" ]]  ; then # NOT (GDAL not set or empty)
    if [[ "$BREW_PACKAGE_BUILD" == "" ]]; then
        echo "Build GDAL, self-contained bundle"
    else
            echo "Build GDAL, brew pkg"
    fi
elif [[ "$BREW_PACKAGE_BUILD" == ""  ]] # NOT (BUILD_STANDALONE not set or empty)
then
    echo "Build Self contained Bundle"
else
    echo "Brew pkg"
fi
