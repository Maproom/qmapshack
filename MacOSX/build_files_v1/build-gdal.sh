#!/bin/sh

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
    return
fi
source $SRC_OSX_DIR/env-path.sh

######################################################################## 
# build GDAL (experimental: now using cmake)
    echo "${RED}Building GDAL ...${NC}"
    cd $QMSDEVDIR
    git clone https://github.com/OSGeo/gdal.git
# --> folder $QMSVERDIR/gdal/ created
    cd $QMSDEVDIR/gdal
    mkdir build
    cd ./build
    cmake ..  -DCMAKE_PREFIX_PATH=$LOCAL_ENV -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$GDAL_DIR -DGDAL_SET_INSTALL_RELATIVE_RPATH=ON -DGDAL_USE_INTERNAL_LIBS=ON -DGDAL_USE_EXTERNAL_LIBS=OFF
    cmake --build . -j4
    cmake --build . --target install
    cd $QMSDEVDIR