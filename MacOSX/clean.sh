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

######################################################################## 
# clean up
echo "${RED}Cleaning build artifacts ...${NC}"

rm $QMSDEVDIR/brewlist*.txt
echo "${RED}Installed homebrew packages are listed in $QMSDEVDIR/brewdiff*.diff${NC}"

rm -rf $QMSDEVDIR/local
rm -rf $QMSDEVDIR/quazip
rm -rf $QMSDEVDIR/gdal
rm -rf $QMSDEVDIR/routino
rm -rf $QMSDEVDIR/qmapshack 
rm -rf $QMSDEVDIR/build_QMapShack

echo "${RED}You can remove 1st_QMS_start.sh script from $QMSDEVDIR, if you don't want to build again${NC}"




