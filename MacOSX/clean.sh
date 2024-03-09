#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

######################################################################## 
# clean up
echo "${ATTN}Cleaning build artifacts ...${NC}"

rm $QMSDEVDIR/brewlist*.txt
echo "${ATTN}Installed homebrew packages are listed in $QMSDEVDIR/brewdiff*.diff${NC}"
rm $QMSDEVDIR/portlist*.txt
echo "${ATTN}Installed macports are listed in $QMSDEVDIR/portsdiff*.diff${NC}"

rm -rf $QMSDEVDIR/local
rm -rf $QMSDEVDIR/otoolrecursive
rm -rf $QMSDEVDIR/quazip
rm -rf $QMSDEVDIR/gdal
rm -rf $QMSDEVDIR/routino*
rm -rf $QMSDEVDIR/proj*
rm -rf $QMSDEVDIR/build_QMapShack

echo "${ATTN}You can remove the qmapshap git repo from $QMSDEVDIR by rm -rf $QMSDEVDIR/qmapshack ${NC}"




