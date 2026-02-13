#!/bin/sh

echo "Setting up GDAL environment ..."

# Check for GDAL apps
if [ ! -e gdalinfo ]
then
  echo "GDAL app gdalinfo not found"
  exit
fi

# Check for GDAL data
gdaldata=../Resources/gdal
if [ ! -d $gdaldata ]
then
  echo "GDAL data folder $gdaldata not found"
  exit
fi

# Check for GDAL plugins
gdalplugins=../Resources/gdalplugins
if [ ! -d $data ]
then
  echo "GDAL plugin folder $gdalplugins not found"
  exit
fi

# Check for PROJ apps
if [ ! -e proj ]
then
  echo "PROJ app proj not found"
  exit
fi

# Check for PROJ data
projdata=../Resources/proj
if [ ! -d $data ]
then
  echo "PROJ data folder $projdata not found"
  exit
fi

export PATH=$(pwd):$PATH
export GDAL_DATA=$gdaldata
export GDAL_DRIVER_PATH=$gdalplugins
export PROJ_DATA=$projdata

exec $SHELL

