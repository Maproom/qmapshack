#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SRC_DIR=$(dirname "$DIR")
ROOT_DIR=$(dirname "$SRC_DIR")

set -a
# Set this pathes according to your environment
# ---------------------------------------------
LIB_BREW_DIR=/usr/local/Cellar
# should use /usr/local/opt/qt5
QT_DIR=$LIB_BREW_DIR/qt5/5.5.0
GDAL_DIR=$LIB_BREW_DIR/gdal/1.11.3
PROJ_DIR=$LIB_BREW_DIR/proj/4.9.2

HG_BIN=/Applications/Dev/MacHg.app/Contents/Resources/localhg
# ---------------------------------------------

APP_NAME=QMapShack
APP_BUNDLE=$APP_NAME.app

SRC_QMAPSHACK_DIR=$ROOT_DIR/qmapshack-osx
SRC_OSX_DIR=$SRC_QMAPSHACK_DIR/MacOSX
SRC_RESOURCES_DIR=$SRC_OSX_DIR/resources

BUILD_DIR=$ROOT_DIR/build_xcode_osx
BUILD_BIN_DIR=$BUILD_DIR/bin
BUILD_RELEASE_DIR=$BUILD_BIN_DIR/Release

BUILD_BUNDLE_DIR=$BUILD_RELEASE_DIR/$APP_BUNDLE
BUILD_BUNDLE_CONTENTS_DIR=$BUILD_BUNDLE_DIR/Contents
BUILD_BUNDLE_APP_DIR=$BUILD_BUNDLE_DIR/Contents/MacOS
BUILD_BUNDLE_RES_DIR=$BUILD_BUNDLE_DIR/Contents/Resources
BUILD_BUNDLE_FRW_DIR=$BUILD_BUNDLE_DIR/Contents/Frameworks
BUILD_BUNDLE_PLUGIN_DIR=$BUILD_BUNDLE_DIR/Contents/PlugIns
BUILD_BUNDLE_APP_FILE=$BUILD_BUNDLE_APP_DIR/$APP_NAME

BUILD_BUNDLE_RES_QM_DIR=$BUILD_BUNDLE_RES_DIR/translations
BUILD_BUNDLE_RES_GDAL_DIR=$BUILD_BUNDLE_RES_DIR/gdal
BUILD_BUNDLE_RES_PROJ_DIR=$BUILD_BUNDLE_RES_DIR/proj
BUILD_BUNDLE_RES_ROUTINO_DIR=$BUILD_BUNDLE_RES_DIR/routino


LIB_ROUTINO_DIR=$ROOT_DIR/routino-lib
LIB_ROUTINO_LIB_DIR=$ROOT_DIR/routino-lib/lib
LIB_ROUTINO_H_DIR=$ROOT_DIR/routino-lib/include
LIB_ROUTINO_XML_DIR=$ROOT_DIR/routino-lib/xml

set +a

