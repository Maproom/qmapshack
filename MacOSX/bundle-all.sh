#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

# Bundling QMapShack and QMapTool
echo "${INFO}Bundle QMapShack ...${NC}"
mkdir -p $BUILD_RELEASE_DIR
cd $SRC_OSX_DIR
source ./bundle-qmapshack.sh
cd $SRC_OSX_DIR
source ./bundle-qmaptool.sh
echo "${INFO}Find QMapShack.app and QMapTool.app in $BUILD_RELEASE_DIR${NC}"

echo "${ATTN}You can clean up build artifacts by running${NC}"
echo "${ATTN}sh $SRC_OSX_DIR/clean.sh clean${NC}"

cd $QMSDEVDIR
