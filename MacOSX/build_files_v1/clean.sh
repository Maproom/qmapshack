#!/bin/sh

if [[ "$QMSDEVDIR" == "" ]]; then
    echo "${RED}Please run 1st_QMS_start.sh${NC}"
       exit
fi
source $SRC_OSX_DIR/env-path.sh

######################################################################## 
# clean up
echo "${RED}Cleaning build artifacts ...${NC}"

if [[ $MACPORTS -eq 1 ]]
then
    rm $QMSDEVDIR/portlist*.txt
     echo "${RED}Installed Macports are listed in $QMSDEVDIR/ports*.diff${NC}"
else
    rm $QMSDEVDIR/brewlist*.txt
    echo "${RED}Installed homebrew packages are listed in $QMSDEVDIR/brewdiff*.diff${NC}"
fi

rm -rf $QMSDEVDIR/quazip
rm -rf $LOCAL_ENV
rm -rf $ROUTINO_ORG
rm -rf $QMSDEVDIR/qmapshack 
rm -rf $QMSDEVDIR/build_QMapShack

echo "${RED}You can remove 1st_QMS_start.sh script from $QMSDEVDIR, if you don't want to build again${NC}"




