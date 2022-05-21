#!/bin/bash

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
    return
fi
source $SRC_OSX_DIR/env-path.sh

# Bundling QMapShack and QMapTool
echo "${GREEN}Bundle QMapShack ...${NC}"
mkdir $BUILD_RELEASE_DIR
cd $QMSDEVDIR/QMapShack/MacOSX
source ./bundle-qmapshack.sh bundle
cd $QMSDEVDIR/QMapShack/MacOSX
source ./bundle-qmaptool.sh bundle
echo "${GREEN}Find QMapShack.app and QMapTool.app in $BUILD_RELEASE_DIR${NC}"

# Codesign the apps (on arm64 mandatory):
echo "${GREEN}Signing app bundles${NC}"
codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app 
codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app/Contents/Tools/gdalbuildvrt
codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapTool.app
echo "${GREEN}QMapShack.app and QMapTool.app are signed${NC}"

echo "${RED}You can clean up build artifacts by running${NC}"
echo "${RED}sh $SRC_OSX_DIR/clean.sh clean${NC}"

cd $QMSDEVDIR
