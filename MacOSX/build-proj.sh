#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Building PROJ ...${NC}"
echo "${ATTN}-----------------${NC}"

########################################################################
# build PROJ
echo "${ATTN}Building Proj ...${NC}"
cd $QMSDEVDIR

# Check for local PROJ repo
if [ -d proj ] && [ -e proj/proj-$PROJ_RELEASE.txt ]
then
  # Update existing repo
  cd $QMSDEVDIR/proj
  git fetch
  git merge
else
  # Create new repo
  rm -rf proj 2>/dev/null
  git clone -b "$PROJ_RELEASE" https://github.com/OSGeo/proj.git
  # --> folder $QMSVERDIR/proj created
  cd $QMSDEVDIR/proj
  touch proj-$PROJ_RELEASE.txt
fi

mkdir build
cd build

PROJ=$LOCAL_ENV

if [ -z "$MACPORTS_BUILD" ]
then
  # Prefer homebrew packages
  TIFF=$(brew --prefix libtiff 2>/dev/null)
  CURL=$(brew --prefix curl 2>/dev/null)
  SQLITE3=$(brew --prefix sqlite 2>/dev/null)
  PROJ_CMAKE_PREFIX_PATH="$TIFF;$CURL;$SQLITE3;$CMAKE_PREFIX_PATH"
fi

$PACKAGES_PATH/bin/cmake .. \
	-DCMAKE_PREFIX_PATH="$PROJ_CMAKE_PREFIX_PATH" \
	-DCMAKE_INSTALL_PREFIX=$LOCAL_ENV \
	-DBUILD_TESTING=OFF

$PACKAGES_PATH/bin/cmake --build . -j4
$PACKAGES_PATH/bin/cmake --build . --target install

PROJ_DATA=proj-data-$PROJ_DATA_RELEASE

cd $LOCAL_ENV/share/proj
if [ ! -e $PROJ_DATA.txt ]
then
  echo "----Installing PROJ DATA -----------"
  curl https://download.osgeo.org/proj/$PROJ_DATA.tar.gz | tar xzf -
  touch $PROJ_DATA.txt
fi

cd $QMSDEVDIR
