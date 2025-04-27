#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

######################################################################## 
# build Routino
echo "${ATTN}Building Routino ...${NC}"
echo "${ATTN}--------------------${NC}"
echo "${INFO}At the end you will be prompted for admin password for adjusting routino libs${NC}"

# Prerequisite
# QMapShack installed from git in QMapShack
# Local environment created

ROUTINO_PKG=routino-3.4.2
ROUTINO_SRC_DIR=$QMSDEVDIR/$ROUTINO_PKG
# The following paths are needed by cmake/Modules/FindROUTINO.cmake. So do not change!
ROUTINO_LIB_DIR=$ROUTINO_DEV_PATH/lib
ROUTINO_INCLUDE_DIR=$ROUTINO_DEV_PATH/include
ROUTINO_SHARE_DIR=$ROUTINO_DEV_PATH/xml
ROUTINO_BIN_DIR=$ROUTINO_DEV_PATH/bin

   
function checkoutRoutino {
    echo "${GREEN}Fetching Routino ...${NC}"
    cd $QMSDEVDIR
    curl http://routino.org/download/$ROUTINO_PKG.tgz  | tar xzf -
}

function buildRoutino {
    echo "${GREEN}Building Routino ...${NC}"
    cd $ROUTINO_SRC_DIR

    # Patch Makefile.conf for macOS
    # inspired by macports version
    # https://github.com/macports/macports-ports/blob/master/gis/routino/files/patch-Makefile_conf.diff
    
    echo "${GREEN}Patching Makefile.conf for macOS ...${NC}"
    sed -i.bak \
        -e 's|^CC=gcc|#CC=gcc|' \
        -e 's|^CXX=g++|#CXX=g++|' \
        -e 's|^LD=gcc|#LD=gcc|' \
        -e 's|^CFLAGS=-std=c99|#CFLAGS=-std=c99|' \
        -e 's|^LDFLAGS_LIB=-shared|LDFLAGS_LIB=-dynamiclib -install_name $(libdir)/$@|' \
        -e 's|-Wl,-soname=[^ ]*||g' \
        -e 's|-Wl,-R.|-Wl,-rpath,.|g' \
        Makefile.conf

    make clean
    make CLANG=1 LDFLAGS_SONAME=""
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
    
    
    mkdir -p $ROUTINO_SHARE_DIR
    cp -f $ROUTINO_SRC_DIR/src/libroutino.so.0.0.0       $ROUTINO_LIB_DIR/libroutino.dylib
    cp -f $ROUTINO_SRC_DIR/src/routino.h                 $ROUTINO_INCLUDE_DIR
    cp -f $ROUTINO_SRC_DIR/xml/routino-profiles.xml       $ROUTINO_SHARE_DIR/profiles.xml
    cp -f $ROUTINO_SRC_DIR/xml/routino-tagging.xml       $ROUTINO_SHARE_DIR/tagging.xml
    cp -f $ROUTINO_SRC_DIR/xml/routino-translations.xml  $ROUTINO_SHARE_DIR/translations.xml
    cp -f $ROUTINO_SRC_DIR/src/planetsplitter            $ROUTINO_BIN_DIR
    cp -f $ROUTINO_SRC_DIR/src/router                    $ROUTINO_BIN_DIR
    
    cp -f $ROUTINO_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_DIR/routino
    cp -f $ROUTINO_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_DIR/routino.dylib   
    cp -f $ROUTINO_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_DIR/routino.a
    cp -f $ROUTINO_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_DIR/libroutino.a
}

checkoutRoutino
buildRoutino
copyRoutinoToInstallDir
adjustLinking

cd $QMSDEVDIR