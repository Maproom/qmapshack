#!/bin/sh
source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters
echo "${ATTN}Building otoolrecursive ...${NC}"
echo "${ATTN}---------------------------${NC}"

######################################################################## 
# build otoolrecursive (command line tool which recursivly lists all dependencies of a dylib)
cd $QMSDEVDIR
git clone https://github.com/d029940/otoolrecursive.git otoolrecursive
# --> folder $QMSVERDIR/quazip/ created
cd $QMSDEVDIR/otoolrecursive
mkdir build
cd ./build
$PACKAGES_PATH/bin/cmake .. -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV
$PACKAGES_PATH/bin/cmake --build . -j4
$PACKAGES_PATH/bin/cmake --build . --target install
cd $QMSDEVDIR
