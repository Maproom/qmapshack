#!/bin/bash

if [[ "$ROUTINO_LIB_DIR" == "" ]]; then
	echo "ROUTINO_LIB_DIR not set"
fi
if [[ "$ROUTINO_SRC_DIR" == "" ]]; then
	echo "ROUTINO_SRC_DIR not set"
fi


ROUTINO_LIB_LIB_DIR=$ROUTINO_LIB_DIR/lib
ROUTINO_LIB_H_DIR=$ROUTINO_LIB_DIR/include
ROUTINO_LIB_XML_DIR=$ROUTINO_LIB_DIR/xml

REPO_URL_B=http://routino.org/svn/branches/libroutino/
REPO_URL_T=http://routino.org/svn/trunk/


function checkoutRoutino {
	if ! [ -d "$ROUTINO_SRC_DIR" ]; then
		mkdir $ROUTINO_SRC_DIR
		svn checkout $REPO_URL_T
	fi
}

function updateRoutino {
    svn revert $ROUTINO_SRC_DIR
    svn update $ROUTINO_SRC_DIR
}

function buildRoutino {
    cd $ROUTINO_SRC_DIR
    rm $ROUTINO_SRC_DIR/src/*.o
    rm $ROUTINO_SRC_DIR/src/filedumper
    rm $ROUTINO_SRC_DIR/src/filedumper-slim
    rm $ROUTINO_SRC_DIR/src/filedumperx
    rm $ROUTINO_SRC_DIR/src/libroutino.so
    rm $ROUTINO_SRC_DIR/src/planetsplitter
    rm $ROUTINO_SRC_DIR/src/planetsplitter-slim
    rm $ROUTINO_SRC_DIR/src/router
    rm $ROUTINO_SRC_DIR/src/router-slim
    
    pimpMakefileConf
    
    make
}

function adjustLinking {
     sudo install_name_tool -id $ROUTINO_LIB_LIB_DIR/libroutino.so $ROUTINO_LIB_LIB_DIR/libroutino.so
     sudo install_name_tool -id $ROUTINO_LIB_LIB_DIR/routino.so $ROUTINO_LIB_LIB_DIR/routino.so
     sudo install_name_tool -id $ROUTINO_LIB_LIB_DIR/routino.a $ROUTINO_LIB_LIB_DIR/routino.a
     sudo install_name_tool -id $ROUTINO_LIB_LIB_DIR/libroutino.a $ROUTINO_LIB_LIB_DIR/libroutino.a
}

function pimpMakefileConf {
    sed 's/LDFLAGS_SONAME.*/LDFLAGS_SONAME=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,$(SOVERSION),-current_version,$(SOVERSION),-install_name,"libroutino.so" -o "libroutino.so"/' $ROUTINO_SRC_DIR/Makefile.conf> ./makefile.tmp
    sed 's/LDFLAGS_SLIM_SONAME.*/LDFLAGS_SLIM_SONAME=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,$(SOVERSION),-current_version,$(SOVERSION),-install_name,"libroutino-slim.so" -o "libroutino-slim.so"/' ./makefile.tmp > ./makefile2.tmp
    sed 's/LDFLAGS_LDSO.*/LDFLAGS_LDSO=-Wl/' ./makefile2.tmp > $ROUTINO_SRC_DIR/Makefile.conf
    
    rm ./makefile.tmp
    rm ./makefile2.tmp

    # Makefile.conf
    # LDFLAGS_SONAME=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,$(SOVERSION),-current_version,$(SOVERSION),-install_name,"libroutino.so" -o "libroutino.so"
    # LDFLAGS_SLIM_SONAME=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,$(SOVERSION),-current_version,$(SOVERSION),-install_name,"libroutino-slim.so" -o "libroutino-slim.so"
    # LDFLAGS_LDSO=-Wl
}

function releaseRoutino {
    rm -R $ROUTINO_LIB_DIR/*
    mkdir $ROUTINO_LIB_LIB_DIR
    mkdir $ROUTINO_LIB_H_DIR
    mkdir $ROUTINO_LIB_XML_DIR
    
    cp $ROUTINO_SRC_DIR/src/libroutino.so             $ROUTINO_LIB_LIB_DIR
    cp $ROUTINO_SRC_DIR/src/routino.h                 $ROUTINO_LIB_H_DIR
    cp $ROUTINO_SRC_DIR/xml/routino-profiles.xml      $ROUTINO_LIB_XML_DIR
    cp $ROUTINO_SRC_DIR/xml/routino-tagging.xml       $ROUTINO_LIB_XML_DIR
    cp $ROUTINO_SRC_DIR/xml/routino-translations.xml  $ROUTINO_LIB_XML_DIR
    cp $ROUTINO_SRC_DIR/src/planetsplitter            $ROUTINO_LIB_LIB_DIR
    
    cp $ROUTINO_LIB_LIB_DIR/libroutino.so             $ROUTINO_LIB_LIB_DIR/routino
    cp $ROUTINO_LIB_LIB_DIR/libroutino.so             $ROUTINO_LIB_LIB_DIR/routino.so
    cp $ROUTINO_LIB_LIB_DIR/libroutino.so             $ROUTINO_LIB_LIB_DIR/routino.a
    cp $ROUTINO_LIB_LIB_DIR/libroutino.so             $ROUTINO_LIB_LIB_DIR/libroutino.a

    cp $ROUTINO_LIB_XML_DIR/routino-profiles.xml      $ROUTINO_LIB_XML_DIR/profiles.xml
    cp $ROUTINO_LIB_XML_DIR/routino-tagging.xml       $ROUTINO_LIB_XML_DIR/tagging.xml
    cp $ROUTINO_LIB_XML_DIR/routino-translations.xml  $ROUTINO_LIB_XML_DIR/translations.xml
}

if [[ "$1" == "routino-build" ]]; then
	checkoutRoutino
    updateRoutino
    buildRoutino
    releaseRoutino
    adjustLinking
fi