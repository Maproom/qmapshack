#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Building PROJ ...${NC}"
echo "${ATTN}-----------------${NC}"

######################################################################## 
# build Proj

PROJ_PKG=proj-$PROJ_RELEASE

cd $QMSDEVDIR
echo "${ATTN}Building Proj ...${NC}"
if [ ! -d $PROJ_PKG ]
then 
  curl https://download.osgeo.org/proj/$PROJ_PKG.tar.gz  | tar xzf -
fi

# --> folder $QMSVERDIR/$PROJ_PKG/ created
cd $QMSDEVDIR/$PROJ_PKG
mkdir build
cd build
$PACKAGES_PATH/bin/cmake .. -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV
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
