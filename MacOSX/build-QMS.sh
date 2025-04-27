#!/bin/sh

######################################################################## 
# Run this script as "sh build-QMS.sh"
######################################################################## 
# build QMapShack

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Building QMS ...${NC}"
echo "${ATTN}----------------${NC}"

# Dir to Quazip cmake files
QUAZIP_CMAKE_DIR=`find $LOCAL_ENV/lib/cmake -type d -name "QuaZip*"`
echo "QUAZIP_CMAKE_DIR = $QUAZIP_CMAKE_DIR"

if [ -d $QMSDEVDIR/build_QMapShack ]; then
  rm -rf $QMSDEVDIR/build_QMapShack
fi
mkdir $QMSDEVDIR/build_QMapShack
cd $QMSDEVDIR/build_QMapShack

GDAL_CMAKE_PAR="-DGDAL_CONFIG=$GDAL/bin/gdal-config -DGDAL_INCLUDE_DIR=$GDAL/include -DGDAL_LIBRARY=$GDAL/lib/libgdal.dylib -DGDAL_DEV_PATH=$GDAL"
CMAKE_PAR="-DLOCAL_DEV=$LOCAL_ENV  -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DQT_DEV_PATH=$QT_DEV_PATH $GDAL_CMAKE_PAR -DROUTINO_DEV_PATH=$ROUTINO_DEV_PATH -DPROJ_DEV_PATH=$PROJ_DEV_PATH -DQuaZip-Qt6_DIR=$QUAZIP_CMAKE_DIR"

echo "${INFO} cmake ../qmapshack ${CMAKE_PAR}${NC}"

if [[ "$XCODE_PROJECT" == "" ]]; then
    $PACKAGES_PATH/bin/cmake ../qmapshack $CMAKE_PAR -DCMAKE_BUILD_TYPE=Release 
    # building QMapShack
    echo "${INFO}Building QMapShack - can take very long ...${NC}"
    make -j$(sysctl -n hw.ncpu)
    echo "${INFO}Building QMapShack - finished${NC}"
else
    # BROKEN:  Creade Xcode project for debugging    
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
    $PACKAGES_PATH/bin/cmake ../qmapshack -G Xcode $CMAKE_PAR -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV -DCMAKE_MACOSX_BUNDLE=ON
    echo "${INFO}Xcode project written in $QMSDEVDIR/build_QMapShack${NC}"
    exit
fi

cd $QMSDEVDIR