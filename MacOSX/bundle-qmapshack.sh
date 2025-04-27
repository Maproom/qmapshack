#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

echo "${ATTN}Bundling QMapShack.app ...${NC}"
echo "${ATTN}--------------------------${NC}"

set -a
APP_NAME=QMapShack
set +a

source $SRC_OSX_DIR/bundle-common-func.sh


function extendAppStructure {
    mkdir -p "$BUILD_BUNDLE_RES_QM_DIR"
    mkdir -p "$BUILD_BUNDLE_RES_GDAL_DIR"
    mkdir -p "$BUILD_BUNDLE_RES_PROJ_DIR"
    mkdir -p "$BUILD_BUNDLE_RES_ROUTINO_DIR"
    mkdir -p "$BUILD_BUNDLE_RES_HELP_DIR"
    mkdir -p "$BUILD_BUNDLE_RES_BIN_DIR"
}


function copyAdditionalLibraries {
      if [ -z "$MACPORTS_BUILD" ]; then

        echo "---building with homebrew---"
        cp -v    $ROUTINO_DEV_PATH/lib/libroutino* $BUILD_BUNDLE_FRW_DIR
        install -m 644 $LOCAL_ENV/lib/libquazip*.dylib $BUILD_BUNDLE_FRW_DIR

        if [ -z "$BREW_PACKAGE_BUILD" ]; then
            # copy only if built as standalone package (QMS not as a brew pkg)
            echo "---build needs brew at runtime---"

            if [ "$BUILD_GDAL" = "x" ]; then
                install -m 644 `brew --prefix openjpeg`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                install -m 644 `brew --prefix libkml`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                install -m 644 `brew --prefix minizip`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                install -m 644 `brew --prefix uriparser`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                install -m 644 `brew --prefix geos`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                install -m 644 $LOCAL_ENV/lib/libgdal*.dylib $BUILD_BUNDLE_FRW_DIR
            else
                install -m 644 `brew --prefix gdal`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                install -m 644 `brew --prefix openexr`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                install -m 644 `brew --prefix geos`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
                install -m 644 `brew --prefix jpeg-xl`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
            fi
            
            $LOCAL_ENV/bin/otoolrecursive -u $BUILD_BUNDLE_FRW_DIR/libgdal.dylib | xargs -I{} cp -v {} $BUILD_BUNDLE_FRW_DIR

            if [ "$BUILD_PROJ" = "x" ]; then
                install -m 644 $LOCAL_ENV/lib/libproj*.dylib $BUILD_BUNDLE_FRW_DIR
            else
                install -m 644 `brew --prefix proj`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR
            fi
            install -m 644 `brew --prefix dbus`/lib/lib*.dylib $BUILD_BUNDLE_FRW_DIR

            cp -v -R $QT_DEV_PATH/lib/QtQuickWidgets.framework $BUILD_BUNDLE_FRW_DIR
        fi
        
    else
        echo "---building with macports---"
        echo "---copy additional libs into bundle ------------------"
        port contents routino | grep $PACKAGES_PATH/lib/libgroutino | xargs -I{} cp -vP {} $BUILD_BUNDLE_FRW_DIR
        port contents quazip1 | grep $PACKAGES_PATH/lib/libquazip1 | xargs -I{} cp -vP {} $BUILD_BUNDLE_FRW_DIR
        port contents gdal | grep $GDAL/lib/libgdal | xargs -I{} cp -vP {} $BUILD_BUNDLE_FRW_DIR

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
    
    # remove static libraries
    rm -f $BUILD_BUNDLE_FRW_DIR/lib*.a
}


function copyExternalFiles {
    if [ -z "$MACPORTS_BUILD" ]; then
        echo "---building with homebrew---"
        
        if [ "$BUILD_GDAL" = "x" ]; then
            cp -vP $LOCAL_ENV/share/gdal/* $BUILD_BUNDLE_RES_GDAL_DIR
        else
            cp -vP $(brew --prefix gdal)/share/gdal/* $BUILD_BUNDLE_RES_GDAL_DIR
        fi
        if [ "$BUILD_PROJ" = "x" ]; then
            cp -vP $LOCAL_ENV/share/proj/* $BUILD_BUNDLE_RES_PROJ_DIR
        else
            cp -vP $PACKAGES_PATH/share/proj/* $BUILD_BUNDLE_RES_PROJ_DIR
        fi
        
        cp -v $ROUTINO_DEV_PATH/xml/profiles.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
        cp -v $ROUTINO_DEV_PATH/xml/translations.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
        cp -v $ROUTINO_DEV_PATH/xml/tagging.xml $BUILD_BUNDLE_RES_ROUTINO_DIR  
    else
        echo "---building with macports---"

        port contents gdal | grep $PACKAGES_PATH/share/gdal/ | xargs -I{} cp -vP {} $BUILD_BUNDLE_RES_GDAL_DIR
        port contents proj9 | grep $PACKAGES_PATH/lib/proj9/share/proj/ | xargs -I{} cp -vP {} $BUILD_BUNDLE_RES_PROJ_DIR
        # port contents routino | grep $PACKAGES_PATH/share/routino/ | xargs -I{} cp -vP {} $BUILD_BUNDLE_RES_ROUTINO_DIR 
        cp -v $ROUTINO_DEV_PATH/xml/profiles.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
        cp -v $ROUTINO_DEV_PATH/xml/translations.xml $BUILD_BUNDLE_RES_ROUTINO_DIR
        cp -v $ROUTINO_DEV_PATH/xml/tagging.xml $BUILD_BUNDLE_RES_ROUTINO_DIR  
    fi

    # Too many files copied from proj --> delete them
    rm $BUILD_BUNDLE_RES_PROJ_DIR/*.tif
    rm $BUILD_BUNDLE_RES_PROJ_DIR/*.txt
}

function copyExternalHelpFiles_QMS {
    cp -v $HELP_QMS_DIR/QMSHelp.qch $BUILD_BUNDLE_RES_HELP_DIR
    cp -v $HELP_QMS_DIR/QMSHelp.qhc $BUILD_BUNDLE_RES_HELP_DIR
}


function copyExtTools {
    if [ -z "$MACPORTS_BUILD" ]; then
        echo "---building with homebrew---"
 
        if [ -z "$BREW_PACKAGE_BUILD" ]; then
            # copy only if built as standalone package (QMS not as a brew pkg)
            if [ "$BUILD_PROJ" = "x" ]; then
                cp -v $LOCAL_ENV/bin/proj             $BUILD_BUNDLE_RES_BIN_DIR
            else
                cp -v $PACKAGES_PATH/bin/proj             $BUILD_BUNDLE_RES_BIN_DIR
            fi
        fi
        cp -v $GDAL/bin/gdalbuildvrt                $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL/bin/gdaladdo                $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL/bin/gdal_translate          $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $GDAL/bin/gdalwarp                $BUILD_BUNDLE_RES_BIN_DIR

        cp -v $ROUTINO_DEV_PATH/bin/planetsplitter  $BUILD_BUNDLE_RES_BIN_DIR
      else
        echo "---building with macports---"
        cp -v $PACKAGES_PATH/lib/proj9/bin/proj          $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $PACKAGES_PATH/bin/gdalbuildvrt            $BUILD_BUNDLE_RES_BIN_DIR
        cp -v $ROUTINO_DEV_PATH/bin/planetsplitter       $BUILD_BUNDLE_RES_BIN_DIR
    fi
   
}


function archiveBundle {
    ARCHIVE=$(printf "%s/%s-MacOSX_%s.tar.gz" "$BUILD_RELEASE_DIR" "$APP_NAME" "$APP_VERSION")
    echo $ARCHIVE

    rm $ARCHIVE

    cd $BUILD_RELEASE_DIR
    tar -zcvf $ARCHIVE $APP_BUNDLE $APP_BUNDLE_QMAPTOOL
    cd ..
}




echo "---extract version -----------------"
extractVersion
readRevisionHash
echo "---build bundle --------------------"
buildAppStructure
extendAppStructure
echo "---replace version string ----------"
updateInfoPlist

echo "---qt deploy tool ------------------"
if [ "$BREW_PACKAGE_BUILD" = "" ] ; then
    qtDeploy
fi

echo "---copy libraries ------------------"
copyAdditionalLibraries

echo "---copy translations ------------------"
copyQtTranslations    

echo "---external data files -------------"
copyExternalFiles

echo "---external help files -------------"
copyExternalHelpFiles_QMS



echo "---external tools ------------------"
copyExtTools


if [ -z "$BREW_PACKAGE_BUILD" ]; then
    # copy only if built as standalone package (QMS not as a brew pkg)
    echo "---adjust linking ------------------"
    adjustLinking
fi

if [ -z "$BREW_PACKAGE_BUILD" ]; then
    # copy only if built as standalone package (QMS not as a brew pkg)
        echo "---adjustLinkingExtTools -------------"

    adjustLinkingExtTools

fi


printLinkingExtTools
echo "------------------------------------"

# Codesign the apps (on arm64 mandatory):
echo "${INFO}Signing app bundles${NC}"

# 1. remove all empty directories, otherwiese verification of signing will fail
find $BUILD_BUNDLE_CONTENTS_DIR -type d -empty -delete

if [ -z "$BREW_PACKAGE_BUILD" ]; then
    # copy only if built as standalone package (QMS not as a brew pkg)
# 2. sign gdalbuild (special hack), since it is an executable copied from outside into the bundle
# install_name_tool -add_rpath @executable_path/../Frameworks $BUILD_RELEASE_DIR/QMapShack.app/Contents/Tools/gdalbuildvrt
# codesign -s <Apple Dev Account> --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app/Contents/Tools/gdalbuildvrt
codesign -s manfred.kern@gmail.com --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app/Contents/Tools/gdalbuildvrt
# codesign  --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app/Contents/Tools/gdalbuildvrt
fi

# 3. sign the complete app bundle
# codesign -s <Apple Dev Account> --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app
codesign -s manfred.kern@gmail.com --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app
# codesign --force --deep --sign - $BUILD_RELEASE_DIR/QMapShack.app


if [ "$1" = "archive" ]; then
    extractVersion
    archiveBundle
fi
