#!/bin/sh

source $QMS_BUILD_FILES/env_build_vars.sh

# Bundling QMapShack and QMapTool
echo "${GREEN}Bundle QMapShack ...${NC}"
mkdir $BUILD_RELEASE_DIR
cd $QMSDEVDIR/QMapShack/MacOSX
sh ./bundle-all.sh bundle
echo "${GREEN}Find QMapShack.app and QMapTool.app in $BUILD_RELEASE_DIR${NC}"

# Codesign the apps (on arm64 mandatory):
echo "${GREEN}Signing app bundles${NC}"
codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app 
codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapTool.app
echo "${GREEN}QMapShack.app and QMapTool.app are signed${NC}"

echo "${RED}You can clean up build artifacts by running${NC}"
echo "${RED}sh $QMS_BUILD_FILES/clean.sh clean${NC}"
