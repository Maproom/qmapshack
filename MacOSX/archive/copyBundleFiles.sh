#!/bin/sh
# Copy additional files from Macports, homebrew and own built files to a folder structure
# for easy copying them to the Mac Bundle
# Called after QMapShack has been built but not bundled

cd $QMSDEVDIR

source $QMSDEVDIR/QMS_build_files/env_build_vars.sh

# Create folders

# GDAL
export GDAL_DIR=$QMSDEVDIR/gdal/
mkdir -p $GDAL_DIR/lib
mkdir -p $GDAL_DIR/bin
cp $PKGMANAGER/lib/libgdal* $GDAL_DIR/lib/

# GEOS
export GEOS_LIB_DIR=$QMSDEVDIR/geos/lib/
mkdir -p $GEOS_LIB_DIR
cp $PKGMANAGER/lib/libgeos* $GEOS_LIB_DIR

# PROJ
export PROJ_DIR=$QMSDEVDIR/proj
mkdir -p $PROJ_DIR/lib
mkdir $PROJ_DIR/bin
mkdir -p $PROJ_DIR/share/proj

if [[ MACPORTS -eq 1 ]]
then
    # GDAL
    port contents gdal | grep bin | xargs -J % cp -v % $GDAL_DIR/bin

    # PROJ
    cp -r $PKGMANAGER/lib/proj6/share/* $QMSDEVDIR/proj/share/proj/
    cp -r $PKGMANAGER/lib/proj6/lib/* $PROJ_DIR/lib/
    cp $PKGMANAGER/lib/proj6/bin/* $PROJ_DIR/bin
else
    # GDAL
    brew list gdal | grep bin | xargs -J % cp -v % $GDAL_DIR/bin

    # PROJ
    brew list proj | grep bin | xargs -J % cp -v % $PROJ_DIR/bin
    cp -r $PKGMANAGER/share/proj/* $PROJ_DIR/share/proj/
    cp -r $PKGMANAGER/lib/libproj/* $PROJ_DIR/lib/
fi



