#!/bin/sh

# Color echo output (only to emphasize the stages in the build process)
export GREEN=$(tput setaf 2)
export RED=$(tput setaf 1)
export NC=$(tput sgr0)

echo "${GREEN}Bundling QMapShack.app${NC}"
if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please set QMSDEVDIR var to builddir (absolute path needed)${NC}"
    echo "${RED}... OR run 1st_QMS_start.sh${NC}"
    return
fi


if [[ "$BUILD_RELEASE_DIR" == "" ]]; then
    BUILD_RELEASE_DIR=$QMSDEVDIR/release
fi

LOCAL_ENV=$QMSDEVDIR/local

set -a
APP_NAME=QMapShack
set +a

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/bundle-env-path.sh
source $DIR/bundle-common-func.sh


function extendAppStructure {
    mkdir $BUILD_BUNDLE_RES_QM_DIR
    mkdir $BUILD_BUNDLE_RES_GDAL_DIR
    mkdir $BUILD_BUNDLE_RES_PROJ_DIR
    mkdir $BUILD_BUNDLE_RES_ROUTINO_DIR
    mkdir $BUILD_BUNDLE_RES_HELP_DIR
    mkdir $BUILD_BUNDLE_RES_BIN_DIR
}


function copyAdditionalLibraries {
    cp -v    $LOCAL_ENV/lib/libroutino* $BUILD_BUNDLE_FRW_DIR
    cp -v    $LOCAL_ENV/lib/libquazip*.dylib $BUILD_BUNDLE_FRW_DIR
    # cp -v    $HOMEBREW_PREFIX/lib/libgeos*.dylib $BUILD_BUNDLE_FRW_DIR
    cp -v    $HOMEBREW_PREFIX/lib/libgeos*.dylib $BUILD_BUNDLE_EXTLIB_DIR
    cp -v    $GDAL_DIR/* $BUILD_BUNDLE_FRW_DIR

    cp -v    $HOMEBREW_PREFIX/lib/libproj*.dylib $BUILD_BUNDLE_FRW_DIR

    cp -v    $HOMEBREW_PREFIX/lib/libdbus*.dylib $BUILD_BUNDLE_FRW_DIR

    cp -v -R $QT_DIR/lib/QtOpenGL.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtQuick.framework $BUILD_BUNDLE_FRW_DIR
    cp -v -R $QT_DIR/lib/QtQml.framework $BUILD_BUNDLE_FRW_DIR

    # remove debug libraries
    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/* -type f -name '*_debug*'`
    do
        echo $F
        rm $F
    done
    
    # remove static libraries
    rm -f $BUILD_BUNDLE_FRW_DIR/lib*.a
}


function copyExternalFiles {
    cp -v $GDAL_DIR/share/gdal/* $BUILD_BUNDLE_RES_GDAL_DIR
    cp -v $HOMEBREW_PREFIX/share/proj/* $BUILD_BUNDLE_RES_PROJ_DIR
    rm $BUILD_BUNDLE_RES_PROJ_DIR/*.tif
    rm $BUILD_BUNDLE_RES_PROJ_DIR/*.txt

    cp -v $LOCAL_ENV/xml/profiles.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
    cp -v $LOCAL_ENV/xml/translations.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
    cp -v $LOCAL_ENV/xml/tagging.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
}

function copyExternalHelpFiles_QMS {
    cp -v $HELP_QMS_DIR/QMSHelp.qch $BUILD_BUNDLE_RES_HELP_DIR
    cp -v $HELP_QMS_DIR/QMSHelp.qhc $BUILD_BUNDLE_RES_HELP_DIR
}


function copyExtTools {
    # check for homebrew
    if command -v brew > /dev/null 2>&1; then
        # 1. 'brew list gdal': find all gdal files installed by homebrew
        # 2. 'grep bin": select only files in the bin folder
        # 3. copy those files to BUILD_BUNDLE_RES_BIN_DIR
        brew list gdal | grep bin | xargs -J % cp -v % $BUILD_BUNDLE_RES_BIN_DIR
    else
        #cp -v $GDAL_DIR/bin/*                       $BUILD_BUNDLE_RES_BIN_DIR
        # at least gdalbuildvrt is used
        cp -v $GDAL_DIR/bin/gdalbuildvrt            $BUILD_BUNDLE_RES_BIN_DIR
    fi
    cp -v $HOMEBREW_PREFIX/bin/proj             $BUILD_BUNDLE_RES_BIN_DIR
    cp -v $LOCAL_ENV/lib/planetsplitter         $BUILD_BUNDLE_RES_BIN_DIR


    # currently only used by QMapTool.
    cp -v $BUILD_BIN_DIR/qmt_rgb2pct            $BUILD_BUNDLE_RES_BIN_DIR
    cp -v $BUILD_BIN_DIR/qmt_map2jnx            $BUILD_BUNDLE_RES_BIN_DIR
}


function adjustLinkingExtTools {
    for F in `find $BUILD_BUNDLE_RES_BIN_DIR -type f ! \( -name "*.py" \)`
    do
        adjustLinkQt $F "$HOMEBREW_PREFIX/"
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


if [[ "$1" == "" ]]; then
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
    copyExternalHelpFiles_QMS
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
