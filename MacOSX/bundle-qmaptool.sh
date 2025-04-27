#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Bundling QMapTool.app ...${NC}"
echo "${ATTN}-------------------------${NC}"

set -a
APP_NAME=QMapTool
set +a

source $SRC_OSX_DIR/bundle-common-func.sh

function extendAppStructure {
    mkdir -p $BUILD_BUNDLE_RES_GDAL_DIR
    mkdir -p $BUILD_BUNDLE_RES_BIN_DIR
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
    cd ..
}


function copyAdditionalLibraries {
    if [ -z "$MACPORTS_BUILD" ]; then
        echo "---building with homebrew---"

        if [ -z "$BREW_PACKAGE_BUILD" ]; then

            # copy only if built as standalone package (QMS not as a brew pkg)
            echo "---copy additional libs into bundle ------------------"
           
            if [ "$BUILD_GDAL" = "x" ]; then
                cp -vP `brew --prefix openjpeg`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vP `brew --prefix libkml`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vP `brew --prefix minizip`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vP `brew --prefix uriparser`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vP `brew --prefix geos`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vP $LOCAL_ENV/lib/libgdal*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vf `brew --prefix dbus`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
            else
                cp -vP `brew --prefix gdal`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vP `brew --prefix openexr`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vP `brew --prefix geos`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vP `brew --prefix jpeg-xl`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                cp -vf `brew --prefix dbus`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
            fi
            $LOCAL_ENV/bin/otoolrecursive -u $GDAL/lib/libgdal.dylib | xargs -I{} cp -vf {} $BUILD_BUNDLE_FRW_DIR

            cp -v -R $QT_DEV_PATH/lib/QtOpenGL.framework $BUILD_BUNDLE_FRW_DIR
            cp -v -R $QT_DEV_PATH/lib/QtDBus.framework $BUILD_BUNDLE_FRW_DIR
            cp -v -R $QT_DEV_PATH/lib/QtQuick.framework $BUILD_BUNDLE_FRW_DIR
            cp -v -R $QT_DEV_PATH/lib/QtQml.framework $BUILD_BUNDLE_FRW_DIR
            # Too many files copied from proj --> delete them
            rm $BUILD_BUNDLE_RES_PROJ_DIR/*.tif
            rm $BUILD_BUNDLE_RES_PROJ_DIR/*.txt
        fi
    else 
        echo "---building with macports---"
        echo "---copy additional libs into bundle ------------------"
        # gdal
        port contents gdal | grep $PACKAGES_PATH/lib/libgdal | xargs -I{} cp -vP {} $BUILD_BUNDLE_FRW_DIR

        # Qt
        cp -v -R $QT_DEV_PATH/lib/QtOpenGL.framework $BUILD_BUNDLE_FRW_DIR
        cp -v -R $QT_DEV_PATH/lib/QtQuick.framework $BUILD_BUNDLE_FRW_DIR
        cp -v -R $QT_DEV_PATH/lib/QtQml.framework $BUILD_BUNDLE_FRW_DIR
    fi

    # remove debug libraries
    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/* -type f -name '*_debug*'`
    do
        echo $F
        rm $F
    done
}


function copyExtTools {

    # copy only if built as standalone package (QMS does not need homebrew at runtime)
    if [ -z "$BREW_PACKAGE_BUILD" ]; then
        echo "---copy additional tools into bundle ------------------"
        cp -v $GDAL/bin/gdalbuildvrt            $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL/bin/gdaladdo                $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL/bin/gdal_translate          $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL/bin/gdalwarp                $BUILD_BUNDLE_RES_BIN_DIR
    fi
    # currently only used by QMapTool.
    cp -v $BUILD_BIN_DIR/qmt_rgb2pct            $BUILD_BUNDLE_RES_BIN_DIR
    cp -v $BUILD_BIN_DIR/qmt_map2jnx            $BUILD_BUNDLE_RES_BIN_DIR

}

function copyExternalHelpFiles_QMT {
    cp -v $HELP_QMT_DIR/QMTHelp.qch $BUILD_BUNDLE_RES_HELP_DIR
    cp -v $HELP_QMT_DIR/QMTHelp.qhc $BUILD_BUNDLE_RES_HELP_DIR
}



echo "---extract version -----------------"
extractVersion
readRevisionHash
echo "---build bundle --------------------"
buildAppStructure
extendAppStructure
echo "---replace version string ----------"
updateInfoPlist

if [ -z "$BREW_PACKAGE_BUILD" ]; then
    # copy only if built as standalone package (QMS not as a brew pkg)
    echo "---qt deploy tool ------------------"
    qtDeploy
fi
echo "---copy libraries ------------------"
copyAdditionalLibraries
echo "---copy external files -------------"
copyQtTranslations
# copyExternalFiles  (no external files for QMapTool)
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
