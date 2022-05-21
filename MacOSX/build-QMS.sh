#!/bin/sh

######################################################################## 
# Run this script as "sh build-QMS.sh"
######################################################################## 
# build QMapShack

# Color echo output (only to emphasize the stages in the build process)
export GREEN=$(tput setaf 2)
export RED=$(tput setaf 1)
export NC=$(tput sgr0)

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please set QMSDEVDIR var to builddir (absolute path needed)${NC}"
    echo "${RED}... OR run 1st_QMS_start.sh${NC}"
    return
fi

# Some vars:

# MacOS build target
OSX_DEPLOYMENT_TARGET=11.0
# Local build env
LOCAL_ENV=$QMSDEVDIR/local
# Dir to Quazip cmake files
QUAZIP_CMAKE_DIR=`find $LOCAL_ENV/lib/cmake -type d -name "QuaZip*"`
# QT5
QT_DIR=$HOMEBREW_PREFIX/opt/qt5
if [[ $BUILD_GDAL -eq 1 ]]; then
    GDAL_DIR=$LOCAL_ENV
else
    GDAL_DIR=$HOMEBREW_PREFIX
fi

cd $QMSDEVDIR

mkdir $QMSDEVDIR/build_QMapShack
cd $QMSDEVDIR/build_QMapShack


# export CPATH=$LOCAL_ENV/include:$CPATH

# Creating Makefile with cmake
# Following CMAKE_PAR used to be used with the CMakeLists.txt from the original repo
# CMAKE_PAR="-DCMAKE_PREFIX_PATH=$QT_DIR -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DROUTINO_DEV_PATH=$LOCAL_ENV -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON  -DGDAL_CONFIG=$GDAL_DIR/bin/gdal-config -DGDAL_INCLUDE_DIR=$GDAL_DIR/include -DGDAL_LIBRARY=$GDAL_DIR/lib/libgdal.dylib  -DPROJ_DIR=$HOMEBREW_PREFIX/lib/cmake/proj -DQuaZip-Qt5_DIR=$QUAZIP_CMAKE_DIR"
# Following CMAKE_PAR used for setting most variables from cmake command line
# CMAKE_PAR="-DHOMEBREW_PREFIX:PATH=$PKGMANAGER -DQT_DEV_PATH:PATH=$QT_DIR -DLOCAL_DEV:PATH=$LOCAL_ENV -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DROUTINO_DEV_PATH:PATH=$LOCAL_ENV -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON  -DGDAL_CONFIG=$GDAL_DIR/bin/gdal-config -DGDAL_INCLUDE_DIR=$GDAL_DIR/include -DGDAL_LIBRARY=$GDAL_DIR/lib/libgdal.dylib  -DPROJ_DIR:PATH=$HOMEBREW_PREFIX/lib/cmake/proj -DQuaZip-Qt5_DIR:PATH=$QUAZIP_CMAKE_DIR"

# Following CMAKE_PAR sets the least variables from cmake command line and relies instead of CMakeLists.txt
# Hence less flexibility: Additional software packages need to be installed under $LOCAL_ENV 
CMAKE_PAR="-DHOMEBREW_PREFIX:PATH=$HOMEBREW_PREFIX -DQT_DEV_PATH:PATH=$QT_DIR -DLOCAL_DEV:PATH=$LOCAL_ENV -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"


echo "${GREEN} cmake ../qmapshack ${CMAKE_PAR}${NC}"

if [[ $XCODE_PROJECT -eq 0 ]]
then
    cmake ../qmapshack $CMAKE_PAR -DCMAKE_BUILD_TYPE=Release 
    # building QMapShack
    echo "${GREEN}Building QMapShack - can take very long ...${NC}"
    make
    echo "${GREEN}Building QMapShack - finished${NC}"
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