#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Building GDAL ...${NC}"
echo "${ATTN}-----------------${NC}"

######################################################################## 
# build GDAL (experimental: now using cmake)
    echo "${ATTN}Building GDAL ...${NC}"
    cd $QMSDEVDIR
    git clone -b "release/$GDAL_RELEASE" https://github.com/OSGeo/gdal.git
# --> folder $QMSVERDIR/gdal/ created
    cd $QMSDEVDIR/gdal
    mkdir build
    cd ./build
    
    # Ensure local PROJ is used
    export PKG_CONFIG_PATH="$LOCAL_ENV/lib/pkgconfig"
    export CMAKE_PREFIX_PATH="$LOCAL_ENV:$CMAKE_PREFIX_PATH"
    export PROJ_INCLUDE="$LOCAL_ENV/include"

    # Boost headers need to be in the include path
    export CPATH="$LOCAL_ENV/include:$PACKAGES_PATH/include:${CPATH}"
    export LIBRARY_PATH="$LOCAL_ENV/lib:$PACKAGES_PATH/lib"
    export LD_LIBRARY_PATH="$LOCAL_ENV/lib:$PACKAGES_PATH/lib"

    GDAL=$LOCAL_ENV

    $PACKAGES_PATH/bin/cmake .. -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
                                -DCMAKE_BUILD_TYPE=Release \
                                -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV \
                                -DGDAL_SET_INSTALL_RELATIVE_RPATH=ON \
                                -DGDAL_USE_INTERNAL_LIBS=ON \
                                -DGDAL_USE_EXTERNAL_LIBS=OFF \
                                -DCMAKE_DISABLE_FIND_PACKAGE_Arrow=ON \
                                -DGDAL_USE_CURL=ON \
                                -DGDAL_ENABLE_DRIVER_WMS:BOOL=ON \
                                -DGDAL_ENABLE_DRIVER_WCS:BOOL=ON \
                                -DGDAL_USE_TIFF=ON \
                                -DGDAL_USE_GEOTIFF=ON \
                                -DGDAL_USE_GEOS=ON \
                                -DGDAL_USE_PNG=ON \
                                -DGDAL_USE_GIF=ON \
                                -DGDAL_USE_ODBC=ON \
                                -DGDAL_USE_PCRE2=ON \
                                -DGDAL_USE_ICONV=ON \
                                -DGDAL_USE_LIBXML2=ON \
                                -DGDAL_USE_EXPAT=ON \
                                -DGDAL_USE_HEIF=ON \
                                -DGDAL_USE_WEBP=OFF \
                                -DGDAL_ENABLE_PYTHON=OFF \
                                -DGDAL_USE_SWIG=OFF \
                                -DBUILD_PYTHON_BINDINGS=OFF
                            
                                                                
                             
    $PACKAGES_PATH/bin/cmake --build . -j"$(sysctl -n hw.ncpu)"
    $PACKAGES_PATH/bin/cmake --build . --target install
    cd $QMSDEVDIR