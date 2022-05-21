#!/bin/sh

# Color echo output (only to emphasize the stages in the build process)
export GREEN=$(tput setaf 2)
export RED=$(tput setaf 1)
export NC=$(tput sgr0)

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please set QMSDEVDIR var to builddir (absolute path needed)${NC}"
    echo "${RED}... OR run 1st_QMS_start.sh${NC}"
    return
fi

if [[ "$BUILD_RELEASE_DIR" == "" ]]; then
    BUILD_RELEASE_DIR=$QMSDEVDIR/release
fi

LOCAL_ENV=$QMSDEVDIR/local

# Check location of Qt (from Homebrew)
QT_DIR=$HOMEBREW_PREFIX/opt/qt5
Qt5_DIR=$QT_DIR/lib/cmake

if [[ $BUILD_GDAL -eq 1 ]]; then
    GDAL_DIR=$LOCAL_ENV
else
    GDAL_DIR=`brew --prefix gdal`
fi  

set -a
declare APP_LANG=("ca" "cs" "de" "en" "es" "fr" "nl" "ru")
APP_NAME_LOWER="$(tr [A-Z] [a-z] <<< "$APP_NAME")"
APP_BUNDLE=$APP_NAME.app

APP_BUNDLE_QMAPTOOL=QMapTool.app

QMS_SRC_DIR=$QMSDEVDIR/qmapshack
SRC_OSX_DIR=$QMS_SRC_DIR/MacOSX
SRC_RESOURCES_DIR=$SRC_OSX_DIR/resources
BUILD_BIN_DIR=$QMSDEVDIR/build_qmapshack/bin

BUILD_BUNDLE_DIR=$BUILD_RELEASE_DIR/$APP_BUNDLE
BUILD_BUNDLE_CONTENTS_DIR=$BUILD_BUNDLE_DIR/Contents
BUILD_BUNDLE_APP_DIR=$BUILD_BUNDLE_DIR/Contents/MacOS
BUILD_BUNDLE_RES_DIR=$BUILD_BUNDLE_DIR/Contents/Resources
BUILD_BUNDLE_FRW_DIR=$BUILD_BUNDLE_DIR/Contents/Frameworks
BUILD_BUNDLE_PLUGIN_DIR=$BUILD_BUNDLE_DIR/Contents/PlugIns
BUILD_BUNDLE_EXTLIB_DIR=$BUILD_BUNDLE_DIR/Contents/lib
BUILD_BUNDLE_APP_FILE=$BUILD_BUNDLE_APP_DIR/$APP_NAME

BUILD_BUNDLE_RES_QM_DIR=$BUILD_BUNDLE_RES_DIR/translations
BUILD_BUNDLE_RES_GDAL_DIR=$BUILD_BUNDLE_RES_DIR/gdal
BUILD_BUNDLE_RES_GEOS_DIR=$BUILD_BUNDLE_RES_DIR/geos
BUILD_BUNDLE_RES_PROJ_DIR=$BUILD_BUNDLE_RES_DIR/proj
BUILD_BUNDLE_RES_ROUTINO_DIR=$BUILD_BUNDLE_RES_DIR/routino
BUILD_BUNDLE_RES_HELP_DIR=$BUILD_BUNDLE_RES_DIR/help
BUILD_BUNDLE_RES_BIN_DIR=$BUILD_BUNDLE_CONTENTS_DIR/Tools

HELP_QMS_DIR=$QMS_SRC_DIR/src/qmapshack/doc
HELP_QMT_DIR=$QMS_SRC_DIR/src/qmaptool/doc

set +a
