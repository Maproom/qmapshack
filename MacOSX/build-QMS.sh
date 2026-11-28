#!/bin/sh

########################################################################
# Run this script as "sh build-QMS.sh"
########################################################################
# build QMapShack

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important parameters
echo "${ATTN}Building QMS ...${NC}"
echo "${ATTN}----------------${NC}"

if [ -d $BUILD_QMAPSHACK_DIR ]; then
  rm -rf $BUILD_QMAPSHACK_DIR
fi
mkdir -p $BUILD_QMAPSHACK_DIR
cd $BUILD_QMAPSHACK_DIR

GDAL_CMAKE_PAR="-DGDAL_CONFIG=$GDAL_DEV_PATH/bin/gdal-config -DGDAL_INCLUDE_DIR=$GDAL_DEV_PATH/include -DGDAL_LIBRARY=$GDAL_DEV_PATH/lib/libgdal.dylib -DGDAL_DEV_PATH=$GDAL_DEV_PATH"
CMAKE_PAR="-DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_MACOSX_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DQT_DEV_PATH=$QT_DEV_PATH $GDAL_CMAKE_PAR -DROUTINO_DEV_PATH=$ROUTINO_DEV_PATH -DPROJ_DEV_PATH=$PROJ_DEV_PATH"

echo "${INFO} cmake $QMS_SRC_DIR ${CMAKE_PAR}${NC}"

$PACKAGES_PATH/bin/cmake $QMS_SRC_DIR $CMAKE_PAR
# building QMapShack
echo "${INFO}Building QMapShack - can take very long ...${NC}"
make -j$(sysctl -n hw.ncpu)
rc=$?
echo "${INFO}Building QMapShack - finished, rc=$rc${NC}"

cd $QMSDEVDIR
exit $rc
