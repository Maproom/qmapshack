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
    # Boost headers need to be in the include path
    export CPATH="$LOCAL_ENV/include:$PACKAGES_PATH/include:${CPATH}"
    echo "CPATH = $CPATH"
    export LIBRARY_PATH="$LOCAL_ENV/lib:$PACKAGES_PATH/lib"
    export LD_LIBRARY_PATH="$LOCAL_ENV/lib:$PACKAGES_PATH/lib"
    export PATH="$PACKAGES_PATH/opt/expat/bin:$PATH"


    # LDFLAGS="-L$PACKAGES_PATH/opt/expat/lib"
    # CPPFLAGS="-I$PACKAGES_PATH/opt/expat/include"

    GDAL=$LOCAL_ENV

    $PACKAGES_PATH/bin/cmake .. -DCMAKE_PREFIX_PATH=$GDAL \
                                -DCMAKE_BUILD_TYPE=Release \
                                -DCMAKE_INSTALL_PREFIX=$GDAL \
                                -DGDAL_SET_INSTALL_RELATIVE_RPATH=ON \
                                -DGDAL_USE_INTERNAL_LIBS=ON \
                                -DGDAL_USE_EXTERNAL_LIBS=OFF \
                                -DCMAKE_DISABLE_FIND_PACKAGE_Arrow=ON \
                                -DGDAL_USE_CURL=ON \
                                -DGDAL_ENABLE_DRIVER_WMS:BOOL=ON \
                                -DGDAL_ENABLE_DRIVER_WCS:BOOL=ON \
                                -DGDAL_USE_TIFF=ON \
                                -DGDAL_USE_GEOTIFF=ON \
                                -DGDAL_USE_GEOS=ON=ON \
                                -DGDAL_USE_PNG=ON \
                                -DGDAL_USE_GIF=ON \
                                -DGDAL_USE_ODBC=ON \
                                -DGDAL_USE_PCRE2=ON \
                                -DGDAL_USE_ICONV=ON \
                                -DGDAL_USE_LIBXML2=ON \
                                -DGDAL_USE_EXPAT=ON \
                                -DGDAL_USE_HEIF=ON \
                                -DGDAL_USE_WEBP=OFF \
                                # -DGDAL_USE_HDF5=ON \ (Problem)
                                # -DGDAL_USE_JPEG=ON \ (Problem)
                            
                                                                
                             
    $PACKAGES_PATH/bin/cmake --build . -j4
    $PACKAGES_PATH/bin/cmake --build . --target install
    cd $QMSDEVDIR