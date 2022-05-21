#!/bin/sh

######################################################################## 
# Run this script as "sh build-QMS.sh"
######################################################################## 
# build QMapShack

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
    return
fi

source $SRC_OSX_DIR/env-path.sh

cd $QMSDEVDIR

mkdir $BUILD_DIR
cd $BUILD_DIR

# export CPATH=$LOCAL_ENV/include:$CPATH

# Creating Makefile with cmake
# Following CMAKE_PAR used to be used with the CMakeLists.txt from the original repo
# CMAKE_PAR="-DCMAKE_PREFIX_PATH=$QT_DIR -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DROUTINO_DEV_PATH=$ROUTINO_LIB_DIR -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON  -DGDAL_CONFIG=$GDAL_DIR/bin/gdal-config -DGDAL_INCLUDE_DIR=$GDAL_DIR/include -DGDAL_LIBRARY=$GDAL_DIR/lib/libgdal.dylib  -DPROJ_DIR=$PROJ_DIR/lib/cmake/proj -DQuaZip-Qt5_DIR=$QUAZIP_LIB_DIR/lib/cmake/QuaZip-$QUAZIP_Version"
# Following CMAKE_PAR used for setting most variables from cmake command line
# CMAKE_PAR="-DHOMEBREW_PREFIX:PATH=$PKGMANAGER -DQT_DEV_PATH:PATH=$QT_DIR -DLOCAL_DEV:PATH=$LOCAL_ENV -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DROUTINO_DEV_PATH:PATH=$ROUTINO_LIB_DIR -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON  -DGDAL_CONFIG=$GDAL_DIR/bin/gdal-config -DGDAL_INCLUDE_DIR=$GDAL_DIR/include -DGDAL_LIBRARY=$GDAL_DIR/lib/libgdal.dylib  -DPROJ_DIR:PATH=$PROJ_DIR/lib/cmake/proj -DQuaZip-Qt5_DIR:PATH=$QUAZIP_LIB_DIR/lib/cmake/QuaZip-$QUAZIP_Version"

# Following CMAKE_PAR sets the least variables from cmake command line and relies instead of CMakeLists.txt
# Hence less flexibility: Additional software packages need to be installed under $LOCAL_ENV 
CMAKE_PAR="-DHOMEBREW_PREFIX:PATH=$PKGMANAGER -DQT_DEV_PATH:PATH=$QT_DIR -DLOCAL_DEV:PATH=$LOCAL_ENV -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"


echo "${GREEN} cmake ../qmapshack ${CMAKE_PAR}${NC}"

if [[ $XCODE_PROJECT -eq 0 ]]
then
    cmake ../qmapshack $CMAKE_PAR -DCMAKE_BUILD_TYPE=Release 
    # building QMapShack
    echo "${GREEN}Building QMapShack - can take very long ...${NC}"
    cd $QMSDEVDIR/build_QMapShack
    make
else
# Creade Xcode project for debugging    
    # export Qt5Widgets_DIR=/$QT_DIR/lib/cmake/Qt5Widgets
    # export Qt5Xml_DIR=$QT_DIR/lib/cmake/Qt5Xml
    # export Qt5Sql_DIR=$QT_DIR/lib/cmake/Qt5Sql
    # export Qt5LinguistTools_DIR=$QT_DIR/lib/cmake/Qt5LinguistTools
    # export Qt5PrintSupport_DIR=/$QT_DIR/lib/cmake/Qt5PrintSupport
    # export Qt5UiTools_DIR=/$QT_DIR/lib/cmake/Qt5UiTools
    # export Qt5Network_DIR=$QT_DIRlib/cmake/Qt5Network
    # export Qt5WebEngineWidgets_DIR=$QT_DIR/lib/cmake/Qt5WebEngineWidgets
    # export Qt5Help_DIR=$QT_DIR/lib/cmake/Qt5Help
    # export PROJ_DATA=$PROJ_DIR/share/proj
    cmake ../qmapshack -G Xcode $CMAKE_PAR -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV -DCMAKE_MACOSX_BUNDLE=ON
fi

cd $QMSDEVDIR