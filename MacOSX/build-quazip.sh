#!/bin/sh

DIR_SCRIPT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"  # absolute path to the dir of this script
source $DIR_SCRIPT/config.sh   # check for important paramters

######################################################################## 
# build Quazip
cd $QMSDEVDIR
echo "${ATTN}Building Quazip ...${NC}"
git clone https://github.com/stachenov/quazip.git quazip
# --> folder $QMSVERDIR/quazip/ created
cd $QMSDEVDIR/quazip
mkdir build
cd ./build
cmake .. -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV
cmake --build . -j4
cmake --build . --target install
cd $QMSDEVDIR
pushd $LOCAL_ENV/lib/cmake
mv QuaZip-Qt5-1.4 QuaZip-Qt5
popd