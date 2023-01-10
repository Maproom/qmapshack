#!/bin/sh

export DIR_SCRIPT=$(cd `dirname $0` && pwd -P) # absolute path to the dir of this script
source $DIR_SCRIPT/config.sh   # check for important paramters

######################################################################## 
# clean up
echo "${ATTN}Cleaning build artifacts ...${NC}"

rm $QMSDEVDIR/brewlist*.txt
echo "${ATTN}Installed homebrew packages are listed in $QMSDEVDIR/brewdiff*.diff${NC}"

rm -rf $QMSDEVDIR/local
rm -rf $QMSDEVDIR/quazip
rm -rf $QMSDEVDIR/gdal
rm -rf $QMSDEVDIR/routino*
rm -rf $QMSDEVDIR/qmapshack 
rm -rf $QMSDEVDIR/build_QMapShack

echo "${ATTN}You can remove 1st_QMS_start.sh script from $QMSDEVDIR, if you don't want to build again${NC}"




