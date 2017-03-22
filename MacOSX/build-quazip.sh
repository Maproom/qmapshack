#!/bin/bash

if [[ "$QUAZIP_LIB_DIR" == "" ]]; then
	echo "QUAZIP_LIB_DIR not set"
fi
if [[ "$QUAZIP_SRC_DIR" == "" ]]; then
	echo "QUAZIP_SRC_DIR not set"
fi
if [[ "$QT_DIR" == "" ]]; then
	echo "QT_DIR not set"
fi

if [[ "$QUAZIP_BUILD_DIR" == "" ]]; then
	QUAZIP_BUILD_DIR=$QUAZIP_SRC_DIR/../build_quazip
fi

QUAZIP_LIB_LIB_DIR=$QUAZIP_LIB_DIR/lib
QUAZIP_LIB_H_DIR=$QUAZIP_LIB_DIR/include

# build
function buildQuazipLib {
	rm -R $QUAZIP_BUILD_DIR/*
	mkdir $QUAZIP_BUILD_DIR
	cd $QUAZIP_BUILD_DIR
	
	PATH=$PATH:$QT_DIR/bin
	qmake $QUAZIP_SRC_DIR -config release LIBS+=-lz
	make
}

# copy
function copyQuazipFiles {
	rm -R $QUAZIP_LIB_DIR
	mkdir $QUAZIP_LIB_DIR
	mkdir $QUAZIP_LIB_LIB_DIR
	mkdir $QUAZIP_LIB_H_DIR
	
	cp $QUAZIP_SRC_DIR/quazip/*.h $QUAZIP_LIB_H_DIR
	cp $QUAZIP_BUILD_DIR/quazip/libquazip.1.0.0.dylib $QUAZIP_LIB_LIB_DIR
	cp $QUAZIP_BUILD_DIR/quazip/libquazip.1.0.0.dylib $QUAZIP_LIB_LIB_DIR/libquazip.1.dylib
	cp $QUAZIP_BUILD_DIR/quazip/libquazip.1.0.0.dylib $QUAZIP_LIB_LIB_DIR/libquazip5.dylib
		
	chmod a+rx $QUAZIP_LIB_LIB_DIR/*
	chmod a+r $QUAZIP_LIB_H_DIR/*
	
	cp $QUAZIP_LIB_LIB_DIR/libquazip.1.0.0.dylib /usr/local/lib/libquazip.1.dylib
}

function adjustLinking {
     sudo install_name_tool -id $QUAZIP_LIB_LIB_DIR/libquazip.1.dylib $QUAZIP_LIB_LIB_DIR/libquazip.1.dylib
}

if [[ "$1" == "quazip-build" ]]; then
	buildQuazipLib
    copyQuazipFiles
    adjustLinking
fi