#!/bin/sh

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
    return
fi

source $SRC_OSX_DIR/env-path.sh

# if [[ $MACPORTS -eq 0 ]]
# then
    ######################################################################## 
    # build Quazip
    cd $QMSDEVDIR
    echo "${RED}Building Quazip ...${NC}"
    git clone https://github.com/stachenov/quazip.git quazip
    # --> folder $QMSVERDIR/quazip/ created
    cd $QMSDEVDIR/quazip
    mkdir build
    cd ./build
    cmake .. -DCMAKE_INSTALL_PREFIX=$QUAZIP_LIB_DIR
    cmake --build . -j4
    cmake --build . --target install
    cd $QMSDEVDIR
# else
#     # mkdir -p $LOCAL_ENV/$QUAZIP_CMAKE_DIR
#     # QUAZIP_INCLUDE=$LOCAL_ENV/include/$QUAZIP_CMAKE_DIR/quazip
#     # QUAZIP_INCLUDE=$LOCAL_ENV/include/
#     # mkdir -p $QUAZIP_INCLUDE
#     #     QUAZIP_INCLUDE=$LOCAL_ENV/include/
#     # mkdir -p $QUAZIP_INCLUDE
#     port contents quazip | grep "include" | xargs -J % cp -v % $LOCAL_ENV/include
#     port contents quazip | grep "lib" | xargs -J % cp -v % $LOCAL_ENV/lib
#     mkdir $LOCAL_ENV/lib/cmake
#     mv $LOCAL_ENV/lib/*.cmake $LOCAL_ENV/lib/cmake
  
# fi
