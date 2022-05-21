#!/bin/bash

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
    return
fi
source $SRC_OSX_DIR/env-path.sh
cd $QMSDEVDIR

######################################################################## 
# build Routino
echo "${RED}Building Routino ...${NC}"
echo "${GREEN}At the end you will be prompted for admin password for adjusting routino libs${NC}"

ROUTINO_PKG=routino-3.3.3
ROUTINO_SRC_DIR=$ROUTINO_ORG/$ROUTINO_PKG
ROUTINO_PATCH_DIR=$ROUTINO_ORG/routino-patch
ROUTINO_LIB_LIB_DIR=$ROUTINO_LIB_DIR/lib
ROUTINO_LIB_H_DIR=$ROUTINO_LIB_DIR/include
ROUTINO_LIB_XML_DIR=$ROUTINO_LIB_DIR/xml

if [[ $MACPORTS -eq 0 ]]; then
    
    mkdir $ROUTINO_ORG
    cp -r $SRC_OSX_DIR/routino-patch $ROUTINO_ORG
       
    function checkoutRoutino {
        echo "${GREEN}Fetching Routino from Subversion ...${NC}"
        cd $QMSDEVDIR/routino
        curl http://routino.org/download/$ROUTINO_PKG.tgz  | tar xzf -
        patchRoutino
    }
     
    function patchRoutino {
        echo "${GREEN}Patching Routino ...${NC}"
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
        sudo install_name_tool -id $ROUTINO_LIB_LIB_DIR/libroutino.dylib $ROUTINO_LIB_LIB_DIR/libroutino.dylib    
        sudo install_name_tool -id $ROUTINO_LIB_LIB_DIR/routino.dylib    $ROUTINO_LIB_LIB_DIR/routino.dylib    
        sudo install_name_tool -id $ROUTINO_LIB_LIB_DIR/routino.a        $ROUTINO_LIB_LIB_DIR/routino.a
        sudo install_name_tool -id $ROUTINO_LIB_LIB_DIR/libroutino.a     $ROUTINO_LIB_LIB_DIR/libroutino.a
    }
    
    function copyRoutinoToInstallDir {
        echo "${GREEN}Copy Routino build files to install directory ...${NC}"
        if ! [ -d "$ROUTINO_LIB_DIR" ]; then
                    mkdir $ROUTINO_LIB_DIR
        fi
        # rm -R $ROUTINO_LIB_DIR/*
        # mkdir $ROUTINO_LIB_LIB_DIR
        # mkdir $ROUTINO_LIB_H_DIR
        mkdir $ROUTINO_LIB_XML_DIR
        
        cp $ROUTINO_SRC_DIR/src/libroutino.dylib          $ROUTINO_LIB_LIB_DIR
        cp $ROUTINO_SRC_DIR/src/routino.h                 $ROUTINO_LIB_H_DIR
        cp $ROUTINO_SRC_DIR/xml/routino-profiles.xml      $ROUTINO_LIB_XML_DIR
        cp $ROUTINO_SRC_DIR/xml/routino-tagging.xml       $ROUTINO_LIB_XML_DIR
        cp $ROUTINO_SRC_DIR/xml/routino-translations.xml  $ROUTINO_LIB_XML_DIR
        cp $ROUTINO_SRC_DIR/src/planetsplitter            $ROUTINO_LIB_LIB_DIR
        
        cp $ROUTINO_LIB_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_LIB_DIR/routino
        cp $ROUTINO_LIB_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_LIB_DIR/routino.dylib   
        cp $ROUTINO_LIB_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_LIB_DIR/routino.a
        cp $ROUTINO_LIB_LIB_DIR/libroutino.dylib          $ROUTINO_LIB_LIB_DIR/libroutino.a
    
        cp $ROUTINO_LIB_XML_DIR/routino-profiles.xml      $ROUTINO_LIB_XML_DIR/profiles.xml
        cp $ROUTINO_LIB_XML_DIR/routino-tagging.xml       $ROUTINO_LIB_XML_DIR/tagging.xml
        cp $ROUTINO_LIB_XML_DIR/routino-translations.xml  $ROUTINO_LIB_XML_DIR/translations.xml
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
    
fi

cd $QMSDEVDIR