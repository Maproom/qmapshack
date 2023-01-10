#!/bin/bash

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
    #      PlugIns
    #         <libs>
    
    rm -rf $BUILD_BUNDLE_DIR
    mkdir $BUILD_BUNDLE_DIR
    mkdir $BUILD_BUNDLE_CONTENTS_DIR
    mkdir $BUILD_BUNDLE_APP_DIR
    mkdir $BUILD_BUNDLE_RES_DIR
    mkdir $BUILD_BUNDLE_RES_QM_DIR
    mkdir $BUILD_BUNDLE_FRW_DIR
    mkdir $BUILD_BUNDLE_PLUGIN_DIR

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
    cp -v $SRC_RESOURCES_DIR/Info.plist $BUILD_BUNDLE_CONTENTS_DIR

    # new icon, if one has been created (otherwise the one from predefined data)
    if [ -f "$BUILD_BIN_DIR/$APP_NAME.icns" ]; then
        echo "cp -v $BUILD_BIN_DIR/$APP_NAME.icns $BUILD_BUNDLE_RES_DIR/"
        cp -v $BUILD_BIN_DIR/$APP_NAME.icns $BUILD_BUNDLE_RES_DIR/
    fi
    
    # binary
    cp -v $BUILD_BIN_DIR/$APP_NAME_LOWER  $BUILD_BUNDLE_APP_DIR/$APP_NAME

    # app translations
    cp -v $BUILD_DIR/src/$APP_NAME_LOWER/*.qm $BUILD_BUNDLE_RES_QM_DIR
}


function copyQtTrqnslations {
    for i in "${APP_LANG[@]}"
    do
        cp -v $QT_DIR/translations/*_${i}.qm $BUILD_BUNDLE_RES_QM_DIR
    done
}


function qtDeploy {
    # -no-strip 
    $QT_DIR/bin/macdeployqt $BUILD_BUNDLE_DIR -always-overwrite -verbose=3
}


function printLinkingApp {
    printLinking $BUILD_BUNDLE_APP_FILE

    for F in `find $BUILD_BUNDLE_FRW_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)`    
    do
        printLinking $F
    done
    
    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/5 -type f -maxdepth 1` 
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
    
    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/5 -type f -maxdepth 1` 
    do
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
        adjustLinkQt $F "/usr/local/"
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR/Qt*.framework/Versions/5 -type f -maxdepth 1` 
    do 
        adjustLinkQt $F "Qt"
        adjustLinkQt $F "/usr/local/"
    done

    for F in `find $BUILD_BUNDLE_FRW_DIR -type f -type f \( -iname "*.dylib" -o -iname "*.so" \)` 
    do 
        adjustLinkQt $F "Qt"
        adjustLinkQt $F "libroutino"
        adjustLinkQt $F "/usr/local/"
    done

    adjustLinkQt $BUILD_BUNDLE_APP_FILE "Qt"
    adjustLinkQt $BUILD_BUNDLE_APP_FILE "libroutino"
}


function adjustLinkQt {
    F=$1 # file
    L=$2 # search condition
    FREL=${F##*/}

    for P in `otool -L $F | awk '{print $1}'`
    do
        #  replace double slashes
        if [[ "$P" == *//* ]]; then 
            PSLASH=$(echo $P | sed 's,//,/,g')
            sudo install_name_tool -change $P $PSLASH $F
        fi

        LIB=${P##*/}
        LIB=${LIB%%:}
        PREL="@executable_path/../Frameworks/$LIB"

        if [[ "$P" == *".framework"* ]]; then
            LIB_VERSION=Versions/5
            LIB=$LIB.framework/$LIB_VERSION/$LIB
            PREL="@executable_path/../Frameworks/$LIB"
        elif [[ "$P" == *"PlugIns"* ]]; then
            # subdirectory for PlugIns
            LIB=${P##*PlugIns/} # remove prepart
            PREL="@executable_path/../PlugIns/$LIB"
        fi

echo "-----"
echo "F    = $F"
echo "FREL = $FREL"
echo "L    = $L"
echo "P    = $P"
echo "LIB  = $LIB"
echo "PREL = $PREL"

        if [[ "$P" == "$FREL" ]]; then
            echo "no update - is a relativ id"
		elif [[ "$LIB" == *"$FREL" ]]; then
            echo "name_tool: $FREL >> $PREL ($P)"
            sudo install_name_tool -id $PREL $F
        elif [[ "$P" == *$L* ]]; then
            echo "name_tool: $FREL > $PREL ($P)"
            sudo install_name_tool -change $P $PREL $F
        fi
    done
}


function checkLibraries {
	F=$1 # file
	DIR=${BUILD_BUNDLE_APP_FILE%/*}
	
	for P in `otool -L $F | awk '{print $1}'`
    do
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

    if [[ -z "$MAJOR_VERSION" ]]; then
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


if [[ "$1" == "icon" ]]; then
    buildIcon
fi
if [[ "$1" == "info" ]]; then
    printLinkingApp
fi
if [[ "$1" == "info-before" ]]; then
    printLinking $BUILD_RELEASE_DIR/$APP_NAME
    # TODO
    printLinking $LIB_ROUTINO_LIB_DIR/libroutino.so
fi

