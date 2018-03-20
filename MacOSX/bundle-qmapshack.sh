#!/bin/bash

set -a
APP_NAME=QMapShack
set +a

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/env-path.sh
source $DIR/bundle.sh

function extendAppStructure {
    mkdir $BUILD_BUNDLE_RES_QM_DIR
    mkdir $BUILD_BUNDLE_RES_GDAL_DIR
    mkdir $BUILD_BUNDLE_RES_PROJ_DIR
    mkdir $BUILD_BUNDLE_RES_ROUTINO_DIR
    mkdir $BUILD_BUNDLE_RES_BIN_DIR
}


function copyAdditionalLibraries {
    cp -v    $ROUTINO_LIB_LIB_DIR/libroutino.so $BUILD_BUNDLE_FRW_DIR
    cp -v    $QUAZIP_LIB_LIB_DIR/libquazip.1.dylib $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtSensors.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtPositioning.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtMultimediaWidgets.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtMultimedia.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtWebKitWidgets.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtOpenGL.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtQuick.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtQml.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtWebChannel.framework $BUILD_BUNDLE_FRW_DIR
    # TODO remove QT Bus, is only for linux needed
    #cp -v -R $QT_DIR/lib/QtDBus.framework $BUILD_BUNDLE_FRW_DIR
}


function copyExternalFiles {
    cp -v $GDAL_DIR/share/gdal/* $BUILD_BUNDLE_RES_GDAL_DIR
    cp -v $PROJ_DIR/share/proj/* $BUILD_BUNDLE_RES_PROJ_DIR

    cp -v $ROUTINO_LIB_XML_DIR/profiles.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
    cp -v $ROUTINO_LIB_XML_DIR/translations.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
    cp -v $ROUTINO_LIB_XML_DIR/tagging.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
}


function copyExtTools {
    # at least gdalbuildvrt is used
    cp -v $GDAL_DIR/bin/*                       $BUILD_BUNDLE_RES_BIN_DIR
    cp -v $PROJ_DIR/bin/proj                    $BUILD_BUNDLE_RES_BIN_DIR
    cp -v $ROUTINO_LIB_LIB_DIR/planetsplitter   $BUILD_BUNDLE_RES_BIN_DIR

    # currently only used by QMapTool.
    cp -v $BUILD_BIN_DIR/qmt_rgb2pct            $BUILD_BUNDLE_RES_BIN_DIR
}


function adjustLinkingExtTools {
    for F in `find $BUILD_BUNDLE_RES_BIN_DIR -type f ! \( -name "*.py" \)`
    do
        adjustLinkQt $F "/usr/local/opt/"
        adjustLinkQt $F "/usr/local/Cellar/"
        adjustLinkQt $F "/usr/local/lib/"
    done
}


function printLinkingExtTools {
    for F in `find $BUILD_BUNDLE_RES_BIN_DIR -type f ! \( -name "*.py" \)`
    do
        printLinking $F
    done
}


function archiveBundle {
    ARCHIVE=$(printf "%s/%s-MacOSX_%s.tar.gz" "$BUILD_RELEASE_DIR" "$APP_NAME" "$APP_VERSION")
    echo $ARCHIVE
    rm $ARCHIVE

    cd $BUILD_RELEASE_DIR
    tar -zcvf $ARCHIVE $APP_BUNDLE $APP_BUNDLE_QMAPTOOL
    cd ..
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
    echo "---copy libraries ------------------"
    copyAdditionalLibraries
    echo "---copy external files -------------"
    copyQtTrqnslations
    copyExternalFiles
    echo "---adjust linking ------------------"
    adjustLinking
    echo "---external tools ------------------"
    copyExtTools
    adjustLinkingExtTools
    printLinkingExtTools
    echo "------------------------------------"
    # chmod a+x $BUILD_BUNDLE_DIR/Contents/Frameworks/*
fi

if [[ "$1" == "archive" ]]; then
    extractVersion
    archiveBundle
fi