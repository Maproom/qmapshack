#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

# DIR="/Users/manfred/Developer/CPlusPlus/Projects/QMS/release/QMapShack.app/Contents/Frameworks"
DIR="/Users/manfred/Developer/CPlusPlus/Projects/QMS/release/QMapShack.app/Contents/Tools"
# DIR=$1

function checkkDyLibs {
  
    for F in `find $DIR -type f ! \( -name "*.py" \)`
    do
        echo "Checking `basename $F`"
        otoolrecursive $F
    done
}

checkkDyLibs $DIR
