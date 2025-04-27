#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Building QUAZIP ...${NC}"
echo "${ATTN}-------------------${NC}"

######################################################################## 
# build Quazip
cd $QMSDEVDIR
echo "${ATTN}Building Quazip ...${NC}"
git clone https://github.com/stachenov/quazip.git quazip
# --> folder $QMSVERDIR/quazip/ created
cd $QMSDEVDIR/quazip
mkdir build
cd ./build

$PACKAGES_PATH/bin/cmake .. -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV -DQT_VERSION_MAJOR=6 -DQUAZIP_QT_MAJOR_VERSION=6 -DCMAKE_PREFIX_PATH=$QT_DEV_PATH
$PACKAGES_PATH/bin/cmake --build . -j"$(sysctl -n hw.ncpu)"
$PACKAGES_PATH/bin/cmake --build . --target install

cd $QMSDEVDIR
