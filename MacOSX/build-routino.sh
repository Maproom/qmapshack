#!/bin/sh

# Color echo output (only to emphasize the stages in the build process)
export GREEN=$(tput setaf 2)
export RED=$(tput setaf 1)
export NC=$(tput sgr0)

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please set QMSDEVDIR var to builddir (absolute path needed)${NC}"
    echo "${RED}... OR run 1st_QMS_start.sh${NC}"
    return
fi

# source $SRC_OSX_DIR/env-path.sh
cd $QMSDEVDIR

######################################################################## 
# build Routino
echo "${GREEN}Building Routino ...${NC}"
echo "${GREEN}At the end you will be prompted for admin password for adjusting routino libs${NC}"

# Prerequisite
# QMapShack installed from git in QMapShack
# Local environment created

SRC_OSX_DIR=$QMSDEVDIR/QMapShack/MacOSX
LOCAL_ENV=$QMSDEVDIR/local

ROUTINO_PKG=routino-3.3.3
ROUTINO_ORG=$QMSDEVDIR/routino
ROUTINO_SRC_DIR=$ROUTINO_ORG/$ROUTINO_PKG
ROUTINO_PATCH_DIR=$ROUTINO_ORG/routino-patch
ROUTINO_LIB_DIR=$LOCAL_ENV/lib
ROUTINO_INCLUDE_DIR=$LOCAL_ENV/include
ROUTINO_XML_DIR=$LOCAL_ENV/xml

    
mkdir $ROUTINO_ORG
   
function checkoutRoutino {
    echo "${GREEN}Fetching Routino from Subversion ...${NC}"
    cd $QMSDEVDIR/routino
    curl http://routino.org/download/$ROUTINO_PKG.tgz  | tar xzf -
    patchRoutino
}
 
function patchRoutino {
    echo "${GREEN}Patching Routino ...${NC}"
    cp -r $SRC_OSX_DIR/routino-patch $ROUTINO_ORG
    # cp $ROUTINO_SRC_DIR/Makefile $ROUTINO_SRC_DIR/Makefile.org
    # patch $ROUTINO_SRC_DIR/Makefile $ROUTINO_PATCH_DIR/Makefile.diff
    cp $ROUTINO_SRC_DIR/Makefile.conf $ROUTINO_SRC_DIR/Makefile.conf.org
    patch $ROUTINO_SRC_DIR/Makefile.conf $ROUTINO_PATCH_DIR//Makefile.conf.diff
    cp $ROUTINO_SRC_DIR/src/Makefile $ROUTINO_SRC_DIR/src/Makefile.org
    patch $ROUTINO_SRC_DIR/src/Makefile $ROUTINO_PATCH_DIR//src-Makefile.diff
}

function buildRoutino {
    echo "${GREEN}Building Routino ...${NC}"
    cd $ROUTINO_SRC_DIR
    
    make clean
    make
}

function adjustLinking {
    echo "${GREEN}Adjust Routino linking libs ...${NC}"

    install_name_tool -id $ROUTINO_LIB_DIR/libroutino.dylib $ROUTINO_LIB_DIR/libroutino.dylib    
    install_name_tool -id $ROUTINO_LIB_DIR/routino.dylib    $ROUTINO_LIB_DIR/routino.dylib    
    install_name_tool -id $ROUTINO_LIB_DIR/routino.a        $ROUTINO_LIB_DIR/routino.a
    install_name_tool -id $ROUTINO_LIB_DIR/libroutino.a     $ROUTINO_LIB_DIR/libroutino.a
}

function copyRoutinoToInstallDir {
    echo "${GREEN}Copy Routino build files to install directory ...${NC}"
    
    cp -f $ROUTINO_SRC_DIR/src/libroutino.dylib          $ROUTINO_LIB_DIR
    cp -f $ROUTINO_SRC_DIR/src/routino.h                 $ROUTINO_INCLUDE_DIR
    cp -f $ROUTINO_SRC_DIR/xml/routino-profiles.xml      $ROUTINO_XML_DIR
    cp -f $ROUTINO_SRC_DIR/xml/routino-tagging.xml       $ROUTINO_XML_DIR
    cp -f $ROUTINO_SRC_DIR/xml/routino-translations.xml  $ROUTINO_XML_DIR
    cp -f $ROUTINO_SRC_DIR/src/planetsplitter            $ROUTINO_LIB_DIR
    
    cp -f $ROUTINO_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_DIR/routino
    cp -f $ROUTINO_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_DIR/routino.dylib   
    cp -f $ROUTINO_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_DIR/routino.a
    cp -f $ROUTINO_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_DIR/libroutino.a

    cp -f $ROUTINO_XML_DIR/routino-profiles.xml      $ROUTINO_XML_DIR/profiles.xml
    cp -f $ROUTINO_XML_DIR/routino-tagging.xml       $ROUTINO_XML_DIR/tagging.xml
    cp -f $ROUTINO_XML_DIR/routino-translations.xml  $ROUTINO_XML_DIR/translations.xml
}

if [[ "$1" == "" ]]; then
	checkoutRoutino
    buildRoutino
    copyRoutinoToInstallDir
    adjustLinking
fi

if [[ "$1" == "checkoutRoutino" ]]; then
	checkoutRoutino
fi


if [[ "$1" == "buildRoutino" ]]; then
	buildRoutino
fi


if [[ "$1" == "copyRoutino" ]]; then
	copyRoutinoToInstallDir
fi


if [[ "$1" == "adjustLinking" ]]; then
	adjustLinking
fi


cd $QMSDEVDIR