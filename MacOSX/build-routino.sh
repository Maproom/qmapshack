#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/env-path.sh

SRC_ROUTINO_DIR_B=$ROOT_DIR/routino-lib-src
SRC_ROUTINO_DIR_T=$ROOT_DIR/routino-src
SRC_ROUTINO_DIR=$SRC_ROUTINO_DIR_T

REPO_URL_B=http://routino.org/svn/branches/libroutino/
REPO_URL_T=http://routino.org/svn/trunk/

function updateRoutino {
    svn revert $SRC_ROUTINO_DIR
    svn update $SRC_ROUTINO_DIR
}

function buildRoutino {
    cd $SRC_ROUTINO_DIR
    rm $SRC_ROUTINO_DIR/src/*.o
    rm $SRC_ROUTINO_DIR/src/filedumper
    rm $SRC_ROUTINO_DIR/src/filedumper-slim
    rm $SRC_ROUTINO_DIR/src/filedumperx
    rm $SRC_ROUTINO_DIR/src/libroutino.so
    rm $SRC_ROUTINO_DIR/src/planetsplitter
    rm $SRC_ROUTINO_DIR/src/planetsplitter-slim
    rm $SRC_ROUTINO_DIR/src/router
    rm $SRC_ROUTINO_DIR/src/router-slim
    
    pimpMakefileConf
    
    make


}

function pimpMakefileConf {
    sed 's/LDFLAGS_SONAME.*/LDFLAGS_SONAME=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,$(SOVERSION),-current_version,$(SOVERSION),-install_name,"libroutino.so" -o "libroutino.so"/' $SRC_ROUTINO_DIR/Makefile.conf> ./makefile.tmp
    sed 's/LDFLAGS_SLIM_SONAME.*/LDFLAGS_SLIM_SONAME=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,$(SOVERSION),-current_version,$(SOVERSION),-install_name,"libroutino-slim.so" -o "libroutino-slim.so"/' ./makefile.tmp > ./makefile2.tmp
    sed 's/LDFLAGS_LDSO.*/LDFLAGS_LDSO=-Wl/' ./makefile2.tmp > $SRC_ROUTINO_DIR/Makefile.conf
    
    rm ./makefile.tmp
    rm ./makefile2.tmp

    # Makefile.conf
    # LDFLAGS_SONAME=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,$(SOVERSION),-current_version,$(SOVERSION),-install_name,"libroutino.so" -o "libroutino.so"
    # LDFLAGS_SLIM_SONAME=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,$(SOVERSION),-current_version,$(SOVERSION),-install_name,"libroutino-slim.so" -o "libroutino-slim.so"
    # LDFLAGS_LDSO=-Wl
}

function releaseRoutino {
    rm -R $LIB_ROUTINO_DIR/*
    mkdir $LIB_ROUTINO_LIB_DIR
    mkdir $LIB_ROUTINO_H_DIR
    mkdir $LIB_ROUTINO_XML_DIR
    
    cp $SRC_ROUTINO_DIR/src/libroutino.so             $LIB_ROUTINO_LIB_DIR
    cp $SRC_ROUTINO_DIR/src/routino.h                 $LIB_ROUTINO_H_DIR
    cp $SRC_ROUTINO_DIR/xml/routino-profiles.xml      $LIB_ROUTINO_XML_DIR
    cp $SRC_ROUTINO_DIR/xml/routino-tagging.xml       $LIB_ROUTINO_XML_DIR
    cp $SRC_ROUTINO_DIR/xml/routino-translations.xml  $LIB_ROUTINO_XML_DIR
    
    cp $LIB_ROUTINO_LIB_DIR/libroutino.so             $LIB_ROUTINO_LIB_DIR/routino
    cp $LIB_ROUTINO_LIB_DIR/libroutino.so             $LIB_ROUTINO_LIB_DIR/routino.so
    cp $LIB_ROUTINO_LIB_DIR/libroutino.so             $LIB_ROUTINO_LIB_DIR/routino.a
    cp $LIB_ROUTINO_LIB_DIR/libroutino.so             $LIB_ROUTINO_LIB_DIR/libroutino.a

    cp $LIB_ROUTINO_XML_DIR/routino-profiles.xml      $LIB_ROUTINO_XML_DIR/profiles.xml
    cp $LIB_ROUTINO_XML_DIR/routino-tagging.xml       $LIB_ROUTINO_XML_DIR/tagging.xml
    cp $LIB_ROUTINO_XML_DIR/routino-translations.xml  $LIB_ROUTINO_XML_DIR/translations.xml
}

if [[ "$1" == "routino-build" ]]; then
    updateRoutino
    buildRoutino
    releaseRoutino
fi