#!/bin/bash

set -a
APP_NAME=QMapTool
set +a

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/env-path.sh
source $DIR/bundle.sh




function linkToQMapShack {
	# link to QMapShack application bundle
    cd $BUILD_RELEASE_DIR
    ln -s ../../QMapShack.app/Contents/Frameworks $BUILD_BUNDLE_FRW_DIR
    ln -s ../../QMapShack.app/Contents/PlugIns $BUILD_BUNDLE_PLUGIN_DIR
    ln -s ../../QMapShack.app/Contents/Tools/ $BUILD_BUNDLE_RES_BIN_DIR
    ln -s ../../../QMapShack.app/Contents/Resources/gdal $BUILD_BUNDLE_RES_GDAL_DIR
    ln -s ../../../QMapShack.app/Contents/Resources/proj $BUILD_BUNDLE_RES_PROJ_DIR
    #ln -s ../../../QMapShack.app/Contents/Resources/translations $BUILD_BUNDLE_RES_QM_DIR
    cd ..
}

function copyExternalHelpFiles_QMT {
    cp -v $HELP_QMT_DIR/QMTHelp.qch $BUILD_BUNDLE_RES_HELP_DIR
    cp -v $HELP_QMT_DIR/QMTHelp.qhc $BUILD_BUNDLE_RES_HELP_DIR
}


function removeDuplicatedQtLibs {
    rm -rf $BUILD_BUNDLE_FRW_DIR
    rm -rf $BUILD_BUNDLE_PLUGIN_DIR
}


if [[ "$1" == "bundle" ]]; then
    echo "---extract version -----------------"
    extractVersion
    readRevisionHash
    echo "---build bundle --------------------"
    buildAppStructure
    extendAppStructure
    echo "---replace version string ----------"
    updateInfoPlist
    echo "---qt deploy tool ------------------"
    qtDeploy
    echo "---copy external files -------------"
    copyQtTrqnslations
    copyExternalFiles
    copyExternalHelpFiles_QMT
    echo "---adjust linking ------------------"
    adjustLinking
    echo "---external tools ------------------"
    copyExtTools
    adjustLinkingExtTools
    printLinkingExtTools
    echo "------------------------------------"
fi
