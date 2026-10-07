#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Building GDAL ...${NC}"
echo "${ATTN}-----------------${NC}"

########################################################################
# build GDAL (using cmake)
echo "${ATTN}Building GDAL ...${NC}"
cd $QMSDEVDIR

# Check for local GDAL repo
if [ -d gdal ] && [ -e gdal/gdal-$GDAL_RELEASE.txt ]
then
  # Update existing repo
  cd $QMSDEVDIR/gdal
  git fetch
  git merge
else
  # Create new repo
  rm -rf gdal 2>/dev/null
  git clone -b "release/$GDAL_RELEASE" https://github.com/OSGeo/gdal.git
  # --> folder $QMSVERDIR/gdal created
  cd $QMSDEVDIR/gdal
  touch gdal-$GDAL_RELEASE.txt
fi

mkdir build
cd build

# Ensure local PROJ is used
export PKG_CONFIG_PATH="$LOCAL_ENV/lib/pkgconfig"
GDAL_CMAKE_PREFIX_PATH="$LOCAL_ENV;$CMAKE_PREFIX_PATH"

if [ -z "$MACPORTS_BUILD" ]
then
  # Use homebrew packages
  TIFF=$(brew --prefix libtiff 2>/dev/null)
  CURL=$(brew --prefix curl 2>/dev/null)
  SQLITE3=$(brew --prefix sqlite 2>/dev/null)
  GDAL_CMAKE_PREFIX_PATH="$TIFF;$CURL;$SQLITE3;$GDAL_CMAKE_PREFIX_PATH"
fi

# Boost headers need to be in the include path
export CPATH="$LOCAL_ENV/include:$PACKAGES_PATH/include:${CPATH}"
export LIBRARY_PATH="$LOCAL_ENV/lib:$PACKAGES_PATH/lib"
export LD_LIBRARY_PATH="$LOCAL_ENV/lib:$PACKAGES_PATH/lib"

GDAL=$LOCAL_ENV

$PACKAGES_PATH/bin/cmake .. \
	-DCMAKE_PREFIX_PATH="$GDAL_CMAKE_PREFIX_PATH" \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$LOCAL_ENV \
	-DGDAL_SET_INSTALL_RELATIVE_RPATH=ON \
	-DGDAL_USE_INTERNAL_LIBS=ON \
	-DGDAL_USE_EXTERNAL_LIBS=OFF \
	-DCMAKE_DISABLE_FIND_PACKAGE_Arrow=ON \
	-DGDAL_USE_CURL=ON \
	-DGDAL_ENABLE_DRIVER_WMS:BOOL=ON \
	-DGDAL_ENABLE_DRIVER_WCS:BOOL=ON \
	-DGDAL_USE_GEOS=ON \
	-DGDAL_USE_ODBC=ON \
	-DGDAL_USE_PCRE2=ON \
	-DGDAL_USE_ICONV=ON \
	-DGDAL_USE_LIBXML2=ON \
	-DGDAL_USE_EXPAT=ON \
	-DGDAL_USE_HEIF=ON \
	-DGDAL_USE_WEBP=OFF \
	-DGDAL_USE_POPPLER=ON \
	-DGDAL_USE_DEFLATE=ON \
	-DGDAL_USE_LIBLZMA=ON \
	-DGDAL_USE_LZ4=ON \
	-DGDAL_USE_ZSTD=ON \
	-DGDAL_USE_SQLITE3=ON \
	-DGDAL_USE_OPENSSL=ON \
	-DBUILD_PYTHON_BINDINGS=OFF

$PACKAGES_PATH/bin/cmake --build . -j"$(sysctl -n hw.ncpu)"
$PACKAGES_PATH/bin/cmake --build . --target install

cd $QMSDEVDIR
