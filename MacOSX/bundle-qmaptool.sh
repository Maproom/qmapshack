#!/bin/sh

DIR_SCRIPT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"  # absolute path to the dir of this script
source $DIR_SCRIPT/config.sh   # check for important paramters

echo "${INFO}Bundling QMapTool.app${NC}"

set -a
APP_NAME=QMapTool
set +a

source $DIR_SCRIPT/bundle-env-path.sh
source $DIR_SCRIPT/bundle-common-func.sh

function extendAppStructure {
    mkdir $BUILD_BUNDLE_RES_GDAL_DIR
    mkdir $BUILD_BUNDLE_RES_BIN_DIR
}

function linkToQMapShack {
	# link to QMapShack application bundle
    cd $BUILD_RELEASE_DIR
    ln -s ../../QMapShack.app/Contents/Frameworks $BUILD_BUNDLE_FRW_DIR
    ln -s ../../QMapShack.app/Contents/PlugIns $BUILD_BUNDLE_PLUGIN_DIR
    ln -s ../../QMapShack.app/Contents/Tools/ $BUILD_BUNDLE_RES_BIN_DIR
    ln -s ../../QMapShack.app/Contents/lib/ $BUILD_BUNDLE_EXTLIB_DIR
    ln -s ../../../QMapShack.app/Contents/Resources/gdal $BUILD_BUNDLE_RES_GDAL_DIR
    ln -s ../../../QMapShack.app/Contents/Resources/proj $BUILD_BUNDLE_RES_PROJ_DIR

    #ln -s ../../../QMapShack.app/Contents/Resources/translations $BUILD_BUNDLE_RES_QM_DIR
    cd ..
}


function copyAdditionalLibraries {
    if [ -z "$BREW_PACKAGE_BUILD"]; then
        # copy only if built as standalone package (QMS not as a brew pkg)
        echo "---copy additional libs into bundle ------------------"
        cp -v    $HOMEBREW_PREFIX/lib/libgeos*.dylib $BUILD_BUNDLE_EXTLIB_DIR
        cp -v    $GDAL_DIR/lib/libgdal*.dylib $BUILD_BUNDLE_FRW_DIR
        cp -v -R $QT_DIR/lib/QtOpenGL.framework $BUILD_BUNDLE_FRW_DIR
        cp -v -R $QT_DIR/lib/QtQuick.framework $BUILD_BUNDLE_FRW_DIR
        cp -v -R $QT_DIR/lib/QtQml.framework $BUILD_BUNDLE_FRW_DIR
    fi

       # remove debug libraries
    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/* -type f -name '*_debug*'`
    do
        echo $F
        rm $F
    done
}


function copyExtTools {
    if [ -z "$BREW_PACKAGE_BUILD"]; then
        # copy only if built as standalone package (QMS not as a brew pkg)
        echo "---copy additional tools into bundle ------------------"
        cp -v $GDAL_DIR/bin/gdalbuildvrt            $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL_DIR/bin/gdaladdo                $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL_DIR/bin/gdal_translate          $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL_DIR/bin/gdalwarp               $BUILD_BUNDLE_RES_BIN_DIR
    fi

     # currently only used by QMapTool.
    cp -v $BUILD_BIN_DIR/qmt_rgb2pct            $BUILD_BUNDLE_RES_BIN_DIR
    cp -v $BUILD_BIN_DIR/qmt_map2jnx            $BUILD_BUNDLE_RES_BIN_DIR
}

function copyExternalHelpFiles_QMT {
    cp -v $HELP_QMT_DIR/QMTHelp.qch $BUILD_BUNDLE_RES_HELP_DIR
    cp -v $HELP_QMT_DIR/QMTHelp.qhc $BUILD_BUNDLE_RES_HELP_DIR
}


function removeDuplicatedQtLibs {
    rm -rf $BUILD_BUNDLE_FRW_DIR
    rm -rf $BUILD_BUNDLE_PLUGIN_DIR
}


if [[ "$1" == "" ]]; then
    echo "---extract version -----------------"
    extractVersion
    readRevisionHash
    echo "---build bundle --------------------"
    buildAppStructure
    extendAppStructure
    echo "---replace version string ----------"
    updateInfoPlist
    
    if [ -z "$BREW_PACKAGE_BUILD"]; then
        # copy only if built as standalone package (QMS not as a brew pkg)
        echo "---qt deploy tool ------------------"
        qtDeploy
    fi
    echo "---copy libraries ------------------"
    copyAdditionalLibraries
    echo "---copy external files -------------"
    copyQtTrqnslations
    copyExternalFiles
    copyExternalHelpFiles_QMT
    if [ -z "$BREW_PACKAGE_BUILD" ]; then
        # copy only if built as standalone package (QMS not as a brew pkg)
        echo "---adjust linking ------------------"
        adjustLinking
    fi
    copyExtTools
    echo "---external tools ------------------"
    if [ -z "$BREW_PACKAGE_BUILD" ]; then
        # copy only if built as standalone package (QMS not as a brew pkg)
        echo "---adjust linking ext tools ---------"
        adjustLinkingExtTools
    fi
    printLinkingExtTools
    echo "------------------------------------"
    # Codesign the apps (on arm64 mandatory):
    echo "${INFO}Signing app bundles${NC}"

    # 1. remove all empty directories, otherwiese verification of signing will fail
    find $BUILD_BUNDLE_CONTENTS_DIR -type d -empty -delete

    # 2. codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapTool.app
    # codesign -s <Apple Dev Account> --force --deep --sign - $BUILD_RELEASE_DIR/QMapTool.app
    codesign -s manfred.kern@gmail.com --force --deep --sign - $BUILD_RELEASE_DIR/QMapTool.app
    # codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapTool.app
fi
