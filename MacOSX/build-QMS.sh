#!/bin/sh

######################################################################## 
# Run this script as "sh build-QMS.sh"
######################################################################## 
# build QMapShack

DIR_SCRIPT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"  # absolute path to the dir of this script
source $DIR_SCRIPT/config.sh   # check for important paramters

# MacOS build target
OSX_DEPLOYMENT_TARGET=11.0
echo "${INFO}MacOS deplyment target: ${OSX_DEPLOYMENT_TARGET}${NC}"

# Dir to Quazip cmake files
QUAZIP_CMAKE_DIR=`find $LOCAL_ENV/lib/cmake -type d -name "QuaZip*"`

mkdir $QMSDEVDIR/build_QMapShack
cd $QMSDEVDIR/build_QMapShack

echo "LOCAL = $LOCAL_ENV"
# CMAKE_PAR="-DQT_DEV_PATH:PATH=$QT_DIR -DLOCAL_DEV:PATH=$LOCAL_ENV -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DGDAL_CONFIG=$GDAL_DIR/bin/gdal-config -DGDAL_INCLUDE_DIR=$GDAL_DIR/include -DGDAL_LIBRARY=$GDAL_DIR/lib/libgdal.dylib"
CMAKE_PAR="-DLOCAL_DEV:PATH=$LOCAL_ENV -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DGDAL_CONFIG=$GDAL_DIR/bin/gdal-config -DGDAL_INCLUDE_DIR=$GDAL_DIR/include -DGDAL_LIBRARY=$GDAL_DIR/lib/libgdal.dylib"


echo "${INFO} cmake ../qmapshack ${CMAKE_PAR}${NC}"

if [[ "$XCODE_PROJECT" == "" ]]; then
    cmake ../qmapshack $CMAKE_PAR -DCMAKE_BUILD_TYPE=Release 
    # building QMapShack
    echo "${INFO}Building QMapShack - can take very long ...${NC}"
    make
    echo "${INFO}Building QMapShack - finished${NC}"
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
    echo "${INFO}Xcode project written in $QMSDEVDIR/build_QMapShack${NC}"
    exit
fi

cd $QMSDEVDIR