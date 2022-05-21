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

# Check location of Qt (from Homebrew)
export QT_DIR=$HOMEBREW_PREFIX/opt/qt5
export Qt5_DIR=$QT_DIR/lib/cmake

    ######################################################################## 
    # build Quazip
cd $QMSDEVDIR
echo "${RED}Building Quazip ...${NC}"
git clone https://github.com/stachenov/quazip.git quazip
# --> folder $QMSVERDIR/quazip/ created
cd $QMSDEVDIR/quazip
mkdir build
cd ./build
cmake .. -DCMAKE_INSTALL_PREFIX=$LOCAL_ENV
cmake --build . -j4
cmake --build . --target install
cd $QMSDEVDIR

