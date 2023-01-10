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
    if [[ "$BUILD_GDAL" != "" ]] ; then
        # GDAL should not be copied to bundle anyway
        cp -v    $GDAL_DIR/lib/libgdal*.dylib $BUILD_BUNDLE_FRW_DIR
    fi

    if [[ "$BREW_PACKAGE_BUILD" == "" ]]; then
        # QMS not as a brew pkg
        cp -v    $HOMEBREW_PREFIX/lib/libgeos*.dylib $BUILD_BUNDLE_EXTLIB_DIR
        cp -v    $GDAL_DIR/* $BUILD_BUNDLE_FRW_DIR
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
    if [[ "$BUILD_GDAL" != "" ]]; then
        # use GDAL built from source
        #cp -v $GDAL_DIR/bin/*                       $BUILD_BUNDLE_RES_BIN_DIR
        # at least gdalbuildvrt is used
        cp -v $GDAL_DIR/bin/gdalbuildvrt            $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL_DIR/bin/gdaladdo                $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL_DIR/bin/gdal_translate          $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL_DIR/bin/gdal_warp               $BUILD_BUNDLE_RES_BIN_DIR
    else
        cp -v `brew --prefix gdal`/bin/gdalbuildvrt     $BUILD_BUNDLE_RES_BIN_DIR
        cp -v `brew --prefix gdal`/bin/gdaladdo         $BUILD_BUNDLE_RES_BIN_DIR
        cp -v `brew --prefix gdal`/bin/gdal_translate   $BUILD_BUNDLE_RES_BIN_DIR
        cp -v `brew --prefix gdal`/bin/gdalwarp        $BUILD_BUNDLE_RES_BIN_DIR
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
    if [[ "$BREW_PACKAGE_BUILD" == "" ]] ; then
        echo "---qt deploy tool ------------------"
        qtDeploy
    fi
    echo "---copy libraries ------------------"
    copyAdditionalLibraries
    echo "---copy external files -------------"
    copyQtTrqnslations
    copyExternalFiles
    copyExternalHelpFiles_QMT
    if [[ "$BREW_PACKAGE_BUILD" == "" ]] ; then
        echo "---adjust linking ------------------"
        adjustLinking
    fi
    echo "---external tools ------------------"
    copyExtTools
    if [[ "$BREW_PACKAGE_BUILD" == "" ]] ; then
        adjustLinkingExtTools
    fi
    printLinkingExtTools
    echo "------------------------------------"
        if [[ "$BREW_PACKAGE_BUILD" == "" ]] ; then
        # Codesign the apps (on arm64 mandatory):
        echo "${INFO}Signing app bundles${NC}"
        codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapTool.app
    fi
fi
