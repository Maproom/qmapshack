#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important parameters

# Bundling QMapShack and QMapTool
echo "${INFO}Bundle QMapShack ...${NC}"
mkdir -p $BUILD_RELEASE_DIR
cd $QMS_OSX_DIR
source ./bundle-qmapshack.sh
cd $QMS_OSX_DIR
source ./bundle-qmaptool.sh
echo "${INFO}Find QMapShack.app and QMapTool.app in $BUILD_RELEASE_DIR${NC}"

echo "${ATTN}You can clean up build artifacts by running${NC}"
echo "${ATTN}sh $QMS_OSX_DIR/clean.sh clean${NC}"

cd $QMSDEVDIR
