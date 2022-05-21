#!/bin/sh

# 1. 'brew list gdal': find all gdal files installed by homebrew
# 2. 'grep bin": select only files in the bin folder
# 3. copy those files to BUILD_BUNDLE_RES_BIN_DIR

# check for homebrew
if command -v brew > /dev/null 2>&1; then
    brew list gdal | grep bin | xargs -J % cp -v % $BUILD_BUNDLE_RES_BIN_DIR
fi