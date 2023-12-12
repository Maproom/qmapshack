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

if [ ! -z `brew --prefix qt` ]; then
  echo "unlinking qt and linking qt@5"
  brew unlink qt
  brew link qt@5
fi

$PACKAGES_PATH/bin/cmake .. -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV -DQT_VERSION_MAJOR=5 -DQUAZIP_QT_MAJOR_VERSION=5
$PACKAGES_PATH/bin/cmake --build . -j4
$PACKAGES_PATH/bin/cmake --build . --target install

if [ ! -z `brew --prefix qt` ]; then
  echo "unlinking qt@5 and linking qt"
  brew unlink qt@5
  brew link qt
fi

cd $QMSDEVDIR
