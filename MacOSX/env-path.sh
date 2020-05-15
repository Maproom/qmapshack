#!/bin/bash

if [[ "$QT_DIR" == "" ]]; then
	echo "QT_DIR not set"
fi
if [[ "$GDAL_DIR" == "" ]]; then
	echo "GDAL_DIR not set"
fi
if [[ "$PROJ_DIR" == "" ]]; then
	echo "PROJ_DIR not set"
fi
if [[ "$ROUTINO_LIB_DIR" == "" ]]; then
	echo "ROUTINO_LIB_DIR not set"
fi
if [[ "$QUAZIP_LIB_DIR" == "" ]]; then
	echo "QUAZIP_LIB_DIR not set"
fi
if [[ "$BUILD_DIR" == "" ]]; then
	echo "BUILD_DIR not set"
fi
if [[ "$QMS_SRC_DIR" == "" ]]; then
	echo "QMS_SRC_DIR not set"
fi

if [[ "$BUILD_BIN_DIR" == "" ]]; then
	echo "BUILD_BIN_DIR not set"
fi
if [[ "$BUILD_RELEASE_DIR" == "" ]]; then
	echo "BUILD_RELEASE_DIR not set"
fi

set -a
declare APP_LANG=("ca" "cs" "de" "en" "es" "fr" "nl" "ru")
APP_NAME_LOWER="$(tr [A-Z] [a-z] <<< "$APP_NAME")"
APP_BUNDLE=$APP_NAME.app

APP_BUNDLE_QMAPTOOL=QMapTool.app

SRC_OSX_DIR=$QMS_SRC_DIR/MacOSX
SRC_RESOURCES_DIR=$SRC_OSX_DIR/resources

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
BUILD_BUNDLE_RES_HELP_DIR=$BUILD_BUNDLE_RES_DIR/help
BUILD_BUNDLE_RES_BIN_DIR=$BUILD_BUNDLE_CONTENTS_DIR/Tools

ROUTINO_LIB_LIB_DIR=$ROUTINO_LIB_DIR/lib
ROUTINO_LIB_H_DIR=$ROUTINO_LIB_DIR/include
ROUTINO_LIB_XML_DIR=$ROUTINO_LIB_DIR/xml

HELP_QMS_DIR=$QMS_SRC_DIR/src/qmapshack/doc
HELP_QMT_DIR=$QMS_SRC_DIR/src/qmaptool/doc

QUAZIP_LIB_LIB_DIR=$QUAZIP_LIB_DIR/lib
QUAZIP_LIB_H_DIR=$QUAZIP_LIB_DIR/include
set +a
