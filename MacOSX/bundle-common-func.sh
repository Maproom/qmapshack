#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

# Directories for QMS
SRC_RESOURCES_DIR=$SRC_OSX_DIR/resources
BUILD_BIN_DIR=$QMSDEVDIR/build_QMapShack/bin

# vars for bundling
set -a
declare APP_LANG=("ca" "cs" "de" "en" "es" "fr" "nl" "ru")
APP_NAME_LOWER="$(tr [A-Z] [a-z] <<< "$APP_NAME")"
APP_BUNDLE=$APP_NAME.app

APP_BUNDLE_QMAPTOOL=QMapTool.app

BUILD_BUNDLE_DIR=$BUILD_RELEASE_DIR/$APP_BUNDLE
BUILD_BUNDLE_CONTENTS_DIR=$BUILD_BUNDLE_DIR/Contents
BUILD_BUNDLE_APP_DIR=$BUILD_BUNDLE_DIR/Contents/MacOS
BUILD_BUNDLE_RES_DIR=$BUILD_BUNDLE_DIR/Contents/Resources
BUILD_BUNDLE_FRW_DIR=$BUILD_BUNDLE_DIR/Contents/Frameworks
BUILD_BUNDLE_PLUGIN_DIR=$BUILD_BUNDLE_DIR/Contents/PlugIns
BUILD_BUNDLE_EXTLIB_DIR=$BUILD_BUNDLE_DIR/Contents/lib
BUILD_BUNDLE_APP_FILE=$BUILD_BUNDLE_APP_DIR/$APP_NAME

BUILD_BUNDLE_RES_QM_DIR=$BUILD_BUNDLE_RES_DIR/translations
BUILD_BUNDLE_RES_GDAL_DIR=$BUILD_BUNDLE_RES_DIR/gdal
BUILD_BUNDLE_RES_GEOS_DIR=$BUILD_BUNDLE_RES_DIR/geos
BUILD_BUNDLE_RES_PROJ_DIR=$BUILD_BUNDLE_RES_DIR/proj
BUILD_BUNDLE_RES_ROUTINO_DIR=$BUILD_BUNDLE_RES_DIR/routino
BUILD_BUNDLE_RES_HELP_DIR=$BUILD_BUNDLE_RES_DIR/help
BUILD_BUNDLE_RES_BIN_DIR=$BUILD_BUNDLE_CONTENTS_DIR/Tools

HELP_QMS_DIR=$QMS_SRC_DIR/src/qmapshack/doc
HELP_QMT_DIR=$QMS_SRC_DIR/src/qmaptool/doc

set +a


APP_VERSION=0
BUILD_TIME=$(date +"%y-%m-%dT%H:%M:%S")
BUILD_HASH_KEY=0
COMMIT_STATUS=0

function buildIcon {
    rm -rf $BUILD_BIN_DIR/$APP_NAME.iconset
    mkdir $BUILD_BIN_DIR/$APP_NAME.iconset
    sips -z 16 16     $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_16x16.png
    sips -z 32 32     $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_16x16@2x.png
    sips -z 32 32     $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_32x32.png
    sips -z 64 64     $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_32x32@2x.png
    sips -z 128 128   $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_128x128.png
    sips -z 256 256   $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_128x128@2x.png
    sips -z 256 256   $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_256x256.png
    sips -z 512 512   $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_256x256@2x.png
    sips -z 512 512   $SRC_RESOURCES_DIR/$APP_NAME.png --out $BUILD_BIN_DIR/$APP_NAME.iconset/icon_512x512.png
    cp -v            $SRC_RESOURCES_DIR/$APP_NAME.png       $BUILD_BIN_DIR/$APP_NAME.iconset/icon_512x512@2x.png
    iconutil -c icns -o $BUILD_BIN_DIR/$APP_NAME.icns $BUILD_BIN_DIR/$APP_NAME.iconset
    # rm -r $BUILD_BIN_DIR/$APP_NAME.iconset
}


function buildAppStructure {
    # structure bundle
    # *.app/
    #   Contents/
    #      Info.plist
    #      MacOS/
    #         <binary>
    #      Resources/
    #         *.icns
    #      Frameworks/
    #         <libs>
    #      PlugIns/
    #         <libs>
    #      Tools/
    #      libs/

    rm -rf $BUILD_BUNDLE_DIR
    mkdir $BUILD_BUNDLE_DIR
    mkdir $BUILD_BUNDLE_CONTENTS_DIR
    mkdir $BUILD_BUNDLE_APP_DIR
    mkdir $BUILD_BUNDLE_RES_DIR
    mkdir $BUILD_BUNDLE_RES_QM_DIR
    mkdir $BUILD_BUNDLE_FRW_DIR
    mkdir $BUILD_BUNDLE_PLUGIN_DIR
    if [ "$BREW_PACKAGE_BUILD" = "" ] ; then
        mkdir $BUILD_BUNDLE_EXTLIB_DIR
    fi

    # TODO not all copied from predefined data is needed in every case (eg icon)
    # predefined data
    for i in "${APP_LANG[@]}"
    do
        BUILD_BUNDLE_LOCVERSION_DIR=$BUILD_BUNDLE_RES_DIR/${i}.lproj
        mkdir $BUILD_BUNDLE_LOCVERSION_DIR
        cp -v $SRC_RESOURCES_DIR/locversion.plist $BUILD_BUNDLE_LOCVERSION_DIR
        /usr/libexec/PlistBuddy -c "Set :LprojLocale $i" "$BUILD_BUNDLE_LOCVERSION_DIR/locversion.plist"
        echo "$i"
     done

    cp -v $SRC_RESOURCES_DIR/$APP_NAME.icns $BUILD_BUNDLE_RES_DIR
    cp -v $SRC_RESOURCES_DIR/*.qss $BUILD_BUNDLE_RES_DIR
    # inject PATH of PACKAGES_PATH, so that external tools can be run from the app bundles
    sed  "s|PACKAGES_PATH|$PACKAGES_PATH|" $SRC_RESOURCES_DIR/Info.plist > $QMSDEVDIR/Info.plist
    mv $QMSDEVDIR/Info.plist $BUILD_BUNDLE_CONTENTS_DIR

    # new icon, if one has been created (otherwise the one from predefined data)
    if [ -f "$BUILD_BIN_DIR/$APP_NAME.icns" ]; then
        echo "cp -v $BUILD_BIN_DIR/$APP_NAME.icns $BUILD_BUNDLE_RES_DIR/"
        cp -v $BUILD_BIN_DIR/$APP_NAME.icns $BUILD_BUNDLE_RES_DIR/
    fi

    # binary
    cp -v $BUILD_BIN_DIR/$APP_NAME_LOWER  $BUILD_BUNDLE_APP_DIR/$APP_NAME

    # app translations
    cp -v $BUILD_QMAPSHACK_DIR/src/$APP_NAME_LOWER/*.qm $BUILD_BUNDLE_RES_QM_DIR
}


function copyQtTranslations {
    QT_TRANSLATIONS_DIR=$(find $QT_DEV_PATH -type d -name translations | head -n1)
    for i in "${APP_LANG[@]}"
    do
        for MOD in qtbase qtmultimedia qtquickcontrols2 qtdeclarative
        do
            FILE="$QT_TRANSLATIONS_DIR/${MOD}_${i}.qm"
            if [ -f "$FILE" ]; then
                echo "Copying $FILE to $BUILD_BUNDLE_RES_QM_DIR/"
                cp -v "$FILE" "$BUILD_BUNDLE_RES_QM_DIR/"
            fi
        done
    done
}


function qtDeploy {
    # -no-strip
    echo "MACdeployQT  $QT_DEV_PATH/ and  $BUILD_BUNDLE_DIR/"
    $QT_DEV_PATH/bin/macdeployqt $BUILD_BUNDLE_DIR -always-overwrite -verbose=3
}


function printLinkingApp {
    echo "--------------------------------------------"
    echo "*** printLinkingApp ***"
    printLinking $BUILD_BUNDLE_APP_FILE

    for F in `find $BUILD_BUNDLE_FRW_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        printLinking $F
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/A -type f -maxdepth 1`
    do
        printLinking $F
    done
    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/A/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess -type f -maxdepth 1`
    do
        printLinking $F
    done

    for F in `find $BUILD_BUNDLE_PLUGIN_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        printLinking $F
    done

    checkLibraries $BUILD_BUNDLE_APP_FILE

    for F in `find $BUILD_BUNDLE_FRW_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        checkLibraries $F
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/A -type f -maxdepth 1`
    do
        checkLibraries $F
    done
    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/A/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess -type f -maxdepth 1`
    do
        echo $F
        checkLibraries $F
    done

    for F in `find $BUILD_BUNDLE_PLUGIN_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        checkLibraries $F
    done
}


function adjustLinking {

    for F in `find $BUILD_BUNDLE_PLUGIN_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        adjustLinkQt $F "libq"
        # adjustLinkQt $F "$PACKAGES_PATH/"
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/5 -type f -maxdepth 1`
    do
        adjustLinkQt $F "$PACKAGES_PATH/"
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`
    do
        adjustLinkQt $F "Qt"
        if [ -z "$MACPORTS_BUILD" ]; then
            adjustLinkQt $F "libroutino"
        fi
        # echo "--- Adjusting libs with references to $PACKAGES_PATH ---"
        adjustLinkDyLib $F
        
    done

    adjustLinkQt $BUILD_BUNDLE_APP_FILE "Qt"
    adjustLinkQt $BUILD_BUNDLE_APP_FILE "libroutino"

    # Special treatment for QtWebEngineCore
    # QtWebEngineProcess.app is an app within QtWebEngineCore.framework, which references other Qt frameworks
    PATH_TO_QTWEBENGINEPROCESS="QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app"
    F=$BUILD_BUNDLE_FRW_DIR/$PATH_TO_QTWEBENGINEPROCESS/Contents/MacOS/QtWebEngineProcess
    adjustLinkQt $F "$PACKAGES_PATH/"
    if [ -d "$BUILD_BUNDLE_FRW_DIR/$PATH_TO_QTWEBENGINEPROCESS/Contents" ]; then
        pushd $BUILD_BUNDLE_FRW_DIR/$PATH_TO_QTWEBENGINEPROCESS/Contents
        ln -s ../../../../../../../Frameworks .
        popd
    fi
}

function adjustLinkDyLib {
    echo "adjustLinkDyLib >>> Adjusting dylibs of `basename $1`"
    F=$1 # file
    
    # exclude symlinks
    if [ -L "$F" ]; then
        return
    fi

    for P in `otool -L $F | awk '{print $1}'`
    do
        LIB=`basename $P`
        # 1. adjust libraries from the package path
        case "$P" in
           "$PACKAGES_PATH"*) 
             PREL="@executable_path/../Frameworks/$LIB"
             echo "Changing $LIB to reference $PREL"
             sudo install_name_tool -change $P $PREL $F
             ;;

           # 2. adjust rpath libraries that are known to cause problems
           @rpath/libproj.25.dylib)
             echo "Fixing libproj @rpath in $F"
             sudo install_name_tool -change @rpath/libproj.25.dylib \
                 @executable_path/../Frameworks/libproj.25.dylib $F
             ;;
        esac
    done
}

function adjustLinkExtTool {
    # $1 = filename of exttool
    echo ">>> Adjusting ext Tools of $1"
    for P in `otoolrecursive $1`
    do

        LIB=${P##*/}    
        LIB=${LIB%%:}
        PREL="@executable_path/../Frameworks/$LIB"

        case "$P" in
            *".framework"*)
                LIB_VERSION=Versions/A
                LIB=$LIB.framework/$LIB_VERSION/$LIB
                PREL="@executable_path/../Frameworks/$LIB"
                ;;
            *)
                echo "cp -v $P ../Frameworks/"
                ;;
        esac
   
        echo "install_name_tool -change $P $PREL `basename $1`"
        install_name_tool -change $P $PREL $1
    done
}

function adjustLinkQt {
    F=$1 # file
    L=$2 # search condition
    FREL=${F##*/}

    for P in `otool -L $F | awk '{print $1}'`
    do
        # exclude symlinks
        if [ -L "$P" ]; then
            break
        fi
        #  replace double slashes
        case "$P" in
            *//*)
                PSLASH=$(echo $P | sed 's,//,/,g')
                sudo install_name_tool -change $P $PSLASH $F
                ;;
        esac

        LIB=${P##*/}    
        LIB=${LIB%%:}
        PREL="@executable_path/../Frameworks/$LIB"

        case "$P" in
            *".framework"*)
                LIB_VERSION=Versions/A
                LIB=$LIB.framework/$LIB_VERSION/$LIB
                PREL="@executable_path/../Frameworks/$LIB"
                ;;
            *"PlugIns"*)
                # subdirectory for PlugIns
                LIB=${P##*PlugIns/} # remove prepart
                PREL="@executable_path/../PlugIns/$LIB"
                ;;
        esac

        echo "-----"
        echo "F    = $F"
        echo "FREL = $FREL"
        echo "L    = $L"
        echo "P    = $P"
        echo "LIB  = $LIB"
        echo "PREL = $PREL"

        if [ "$P" = "$FREL" ]; then
            echo "no update - is a relativ id"
        elif echo "$LIB" | grep -q "$FREL"; then
            echo "name_tool: $FREL >> $PREL ($P)"
            sudo install_name_tool -id $PREL $F
        elif echo "$P" | grep -q "$L"; then
            echo "name_tool: $FREL > $PREL ($P)"
            sudo install_name_tool -change $P $PREL $F
        elif echo "$P" | grep -q "^@loader_path"; then
            echo "name_tool: $FREL > $PREL ($P)"
            sudo install_name_tool -change $P $PREL $F
        elif echo "$P" | grep -q "^@rpath"; then
            echo "name_tool: $FREL > $PREL ($P)"
            sudo install_name_tool -change $P $PREL $F
        fi
    done
}


function adjustLinkingExtTools {
    echo "--------------------------------------------"
    echo "Add rpath @executable_path/../Frameworks to "
    for F in `find $BUILD_BUNDLE_RES_BIN_DIR -type f ! \( -name "*.py" \)`
    do
        echo "F    = $F"
        install_name_tool -add_rpath @executable_path/../Frameworks $F
        adjustLinkExtTool $F
    done
    echo "--------------------------------------------"
}


function printLinkingExtTools {
    echo "--------------------------------------------"
    echo "*** printLinkingExtTools ***"
    for F in `find $BUILD_BUNDLE_RES_BIN_DIR -type f ! \( -name "*.py" \)`
    do
        printLinking $F
    done
}


function checkLibraries {
	F=$1 # file
	DIR=${BUILD_BUNDLE_APP_FILE%/*}

    echo "--------------------"
    echo "das File: $F"
    echo "--------------------"

	for P in `otool -L $F | awk '{print $1}'`
    do
        #echo $P
        
    	if [[ "$P" == "@executable_path"* ]]; then
    		FREL=${P##@executable_path}
    		LIB=${DIR}${FREL}
    		#echo "LIB = $LIB"
    		if [ ! -e $LIB ]; then
    			echo "referenced library not bundled: $P"
    		fi
    	fi
    	if [[ "$P" == "/"* && "$P" != "/System/Library/"* && "$P" != "/usr/lib/"* && "$P" != *":" ]]; then
    		echo "external library: $P"
    	fi
        
    done
}


function printLinking {
    echo "--------------------"
    echo "otool $1"
    otool -L $1
    echo "--------------------"
}


function extractVersion {
    # Version CMakeList.txt

    # set(APPLICATION_VERSION_MAJOR "1")
    # set(APPLICATION_VERSION_MINOR "3")
    # set(APPLICATION_VERSION_PATCH "0.libroutino")
    MAJOR_VERSION=$(sed -E -n 's/.*_VERSION_MAJOR.*([[:digit:]]+).*/\1/p' $QMS_SRC_DIR/src/$APP_NAME_LOWER/CMakeLists.txt)
    MINOR_VERSION=$(sed -E -n 's/.*_VERSION_MINOR.*([[:digit:]]+).*/\1/p' $QMS_SRC_DIR/src/$APP_NAME_LOWER/CMakeLists.txt)
    PATCH_VERSION=$(sed -E -n 's/.*_VERSION_PATCH.*([[:digit:]]+).*/\1/p' $QMS_SRC_DIR/src/$APP_NAME_LOWER/CMakeLists.txt)

    if [ -z "$MAJOR_VERSION" ]; then
         # project(QMapShack VERSION 1.11.0)
        MAJOR_VERSION=$(sed -E -n 's/.*QMapShack VERSION.*([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+).*/\1/p' $QMS_SRC_DIR/CMakeLists.txt)
        MINOR_VERSION=$(sed -E -n 's/.*QMapShack VERSION.*([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+).*/\2/p' $QMS_SRC_DIR/CMakeLists.txt)
        PATCH_VERSION=$(sed -E -n 's/.*QMapShack VERSION.*([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+).*/\3/p' $QMS_SRC_DIR/CMakeLists.txt)
    fi

    echo "Version: $MAJOR_VERSION $MINOR_VERSION $PATCH_VERSION"
    APP_VERSION="$MAJOR_VERSION.$MINOR_VERSION.$PATCH_VERSION"
}

function readRevisionHash {
    cd $QMS_SRC_DIR
    BUILD_HASH_KEY=$(git rev-parse HEAD)
    COMMIT_STATUS=$(git status -s -uno)

    if [[ "$COMMIT_STATUS" != "" ]]; then
    read -p "BEWARE - There are uncommited changes..."
    fi
}

function updateInfoPlist {
    /usr/libexec/PlistBuddy -c "Set :CFBundleShortVersionString $APP_VERSION" "$BUILD_BUNDLE_CONTENTS_DIR/Info.plist"
    /usr/libexec/PlistBuddy -c "Set :CFBundleVersion $APP_VERSION"            "$BUILD_BUNDLE_CONTENTS_DIR/Info.plist"
    /usr/libexec/PlistBuddy -c "Set :BuildHashKey $BUILD_HASH_KEY"            "$BUILD_BUNDLE_CONTENTS_DIR/Info.plist"
    /usr/libexec/PlistBuddy -c "Set :BuildTime $BUILD_TIME"                   "$BUILD_BUNDLE_CONTENTS_DIR/Info.plist"

    /usr/libexec/PlistBuddy -c "Set :CFBundleIconFile $APP_NAME"              "$BUILD_BUNDLE_CONTENTS_DIR/Info.plist"
    /usr/libexec/PlistBuddy -c "Set :CFBundleExecutable $APP_NAME"            "$BUILD_BUNDLE_CONTENTS_DIR/Info.plist"
    /usr/libexec/PlistBuddy -c "Set :CFBundleName $APP_NAME"                  "$BUILD_BUNDLE_CONTENTS_DIR/Info.plist"

    /usr/libexec/PlistBuddy -c "Set :CFBundleIdentifier org.qlandkarte.$APP_NAME" "$BUILD_BUNDLE_CONTENTS_DIR/Info.plist"
}


if [ "$1" = "icon" ]; then
    buildIcon
fi
if [ "$1" = "info" ]; then
    printLinkingApp
fi
if [ "$1" = "info-before" ]; then
    printLinking $BUILD_RELEASE_DIR/$APP_NAME
fi