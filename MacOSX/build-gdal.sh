#!/bin/sh

DIR_SCRIPT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"  # absolute path to the dir of this script
source $DIR_SCRIPT/config.sh   # check for important paramters

######################################################################## 
# build GDAL (experimental: now using cmake)
    echo "${ATTN}Building GDAL ...${NC}"
    cd $QMSDEVDIR
    git clone https://github.com/OSGeo/gdal.git
# --> folder $QMSVERDIR/gdal/ created
    cd $QMSDEVDIR/gdal
    mkdir build
    cd ./build
    # cmake ..  -DCMAKE_PREFIX_PATH=$LOCAL_ENV -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV -DGDAL_SET_INSTALL_RELATIVE_RPATH=ON -DGDAL_USE_INTERNAL_LIBS=ON -DGDAL_USE_EXTERNAL_LIBS=OFF
    cmake ..  -DCMAKE_PREFIX_PATH=$LOCAL_ENV -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV -DGDAL_SET_INSTALL_RELATIVE_RPATH=ON -DGDAL_USE_INTERNAL_LIBS=ON
    cmake --build . -j4
    cmake --build . --target install
    cd $QMSDEVDIR