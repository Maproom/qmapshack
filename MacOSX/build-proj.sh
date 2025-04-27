#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Building PROJ ...${NC}"
echo "${ATTN}-----------------${NC}"

######################################################################## 
# build Proj

PROJ_PKG=proj-9.6.0

cd $QMSDEVDIR
echo "${ATTN}Building Proj ...${NC}"
curl https://download.osgeo.org/proj/$PROJ_PKG.tar.gz  | tar xzf -

# --> folder $QMSVERDIR/$PROJ_PKG/ created
cd $QMSDEVDIR/$PROJ_PKG
mkdir build
cd build
$PACKAGES_PATH/bin/cmake .. -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV
$PACKAGES_PATH/bin/cmake --build . -j4
$PACKAGES_PATH/bin/cmake --build . --target install

cd $LOCAL_ENV/share/proj
curl https://download.osgeo.org/proj/proj-data-1.21.tar.gz | tar xzf -
cd $QMSDEVDIR
