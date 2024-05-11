#!/bin/sh

set -a
APP_NAME=QMapTool
APP_NAME=$1
set +a

BUILD_RELEASE_DIR=$QMSDEVDIR/release


if [[ "$APP_NAME" != "QMapTool" && "$APP_NAME" != "QMapShack" ]] 
then
    echo "Parameter needs to be either QMapTool or QMapShack"
    exit
fi

# vars for bundling
set -a
declare APP_LANG=("ca" "cs" "de" "en" "es" "fr" "nl" "ru")
APP_NAME_LOWER="$(tr [A-Z] [a-z] <<< "$APP_NAME")"
APP_BUNDLE=$APP_NAME.app

APP_BUNDLE_QMAPTOOL=QMapTool.app

BUILD_BUNDLE_DIR=$BUILD_RELEASE_DIR/$APP_BUNDLE
BUILD_BUNDLE_CONTENTS_DIR=$BUILD_BUNDLE_DIR/Contents
BUILD_BUNDLE_APP_DIR=$BUILD_BUNDLE_DIR/Contents/MacOS
BUILD_BUNDLE_RES_DIR=$BUILD_BUNDLE_DIR/Contents/Resources
BUILD_BUNDLE_FRW_DIR=$BUILD_BUNDLE_DIR/Contents/Frameworks
BUILD_BUNDLE_PLUGIN_DIR=$BUILD_BUNDLE_DIR/Contents/PlugIns
BUILD_BUNDLE_EXTLIB_DIR=$BUILD_BUNDLE_DIR/Contents/lib
BUILD_BUNDLE_APP_FILE=$BUILD_BUNDLE_APP_DIR/$APP_NAME

BUILD_BUNDLE_RES_BIN_DIR=$BUILD_BUNDLE_CONTENTS_DIR/Tools

set +a


APP_VERSION=0
BUILD_TIME=$(date +"%y-%m-%dT%H:%M:%S")
BUILD_HASH_KEY=0
COMMIT_STATUS=0


function printLinkingApp {
    printLinking $BUILD_BUNDLE_APP_FILE

    for F in `find $BUILD_BUNDLE_FRW_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        printLinking $F
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/5 -type f -maxdepth 1`
    do
        printLinking $F
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/5/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess -type f -maxdepth 1`
    do
        printLinking $F
    done

    for F in `find $BUILD_BUNDLE_PLUGIN_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        printLinking $F
    done

    checkLibraries $BUILD_BUNDLE_APP_FILE

    for F in `find $BUILD_BUNDLE_FRW_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        checkLibraries $F
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/5 -type f -maxdepth 1`
    do
        checkLibraries $F
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/5/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess -type f -maxdepth 1`
    do
        echo $F
        checkLibraries $F
    done

    for F in `find $BUILD_BUNDLE_PLUGIN_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        checkLibraries $F
    done
}


function printLinkingExtTools {
    for F in `find $BUILD_BUNDLE_RES_BIN_DIR -type f ! \( -name "*.py" \)`
    do
        printLinking $F
    done
}


function checkLibraries {
	F=$1 # file
	DIR=${BUILD_BUNDLE_APP_FILE%/*}

    echo "--------------------"
    echo "das File: $F"
    echo "--------------------"

	for P in `otool -L $F | awk '{print $1}'`
    do
        echo $P
        
    	if [[ "$P" == "@executable_path"* ]]; then
    		FREL=${P##@executable_path}
    		LIB=${DIR}${FREL}
    		#echo "LIB = $LIB"
    		if [ ! -e $LIB ]; then
    			echo "referenced library not bundled: $P"
    		fi
    	fi
    	if [[ "$P" == "/"* && "$P" != "/System/Library/"* && "$P" != "/usr/lib/"* && "$P" != *":" ]]; then
    		echo "external library: $P"
    	fi
        
    done
}


function printLinking {
    echo "--------------------"
    echo "otool $1"
    otool -L $1
    echo "--------------------"
}

printLinkingApp
printLinkingExtTools

