#!/bin/sh

# Color echo output (only to emphasize the stages in the build process)
export GREEN=$(tput setaf 2)
export RED=$(tput setaf 1)
export NC=$(tput sgr0)

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please set QMSDEVDIR var to builddir (absolute path needed)${NC}"
    echo "${RED}... OR run 1st_QMS_start.sh${NC}"
    return
fi

LOCAL_ENV=$QMSDEVDIR/local

######################################################################## 
# build GDAL (experimental: now using cmake)
    echo "${RED}Building GDAL ...${NC}"
    cd $QMSDEVDIR
    git clone https://github.com/OSGeo/gdal.git
# --> folder $QMSVERDIR/gdal/ created
    cd $QMSDEVDIR/gdal
    mkdir build
    cd ./build
    cmake ..  -DCMAKE_PREFIX_PATH=$LOCAL_ENV -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV -DGDAL_SET_INSTALL_RELATIVE_RPATH=ON -DGDAL_USE_INTERNAL_LIBS=ON -DGDAL_USE_EXTERNAL_LIBS=OFF
    cmake --build . -j4
    cmake --build . --target install
    cd $QMSDEVDIR