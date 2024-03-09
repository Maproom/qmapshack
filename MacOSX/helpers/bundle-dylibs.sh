#!/bin/sh

source $QMSDEVDIR/qmapshack/MacOSX/config.sh   # check for important paramters

INPUT_LIB="/Users/manfred/Developer/CPlusPlus/Projects/QMS/release/QMapShack.app/Contents/Tools/qmt_rgb2pct"
# INPUT_LIB=$1

function adjustLinkDyLib {
    echo ">>> Adjusting dylibs of `basename $1`"
    # adjust all dylibs in Frameworks with references to package manager
    F=$1 # file
    
    # exclude symlinks
    # if [[ -L "$F" ]]; then
    #         return
    # fi

    for P in `otool -L $F | awk '{print $1}'`
    do
        # $P = dylib referenced by $F
        # get filename of path
        LIB=`basename $P`
        
        # only for references to package, i.e. check if $P starts with $PACKAGES_PATH
         if [[ "$P" =~ ^"$PACKAGES_PATH"  ]]; then
            PREL="@executable_path/../Frameworks/$LIB"
            echo "Changing $LIB to reference $PREL"
            sudo install_name_tool -change $P $PREL $F
         fi
    done
}

function adjustLinkExtTool {
    # $1 = filename of exttool
    echo ">>> Adjusting ext Tools of $1"
    for P in `otoolrecursive $1`
    do

        LIB=${P##*/}    
        LIB=${LIB%%:}
        PREL="@executable_path/../Frameworks/$LIB"

        if [[ "$P" == *".framework"* ]]; then
            LIB_VERSION=Versions/5
            LIB=$LIB.framework/$LIB_VERSION/$LIB
            PREL="@executable_path/../Frameworks/$LIB"
        else
             echo "cp -v $P ../Frameworks/"
        fi
   
        echo "install_name_tool -change $P $PREL `basename $1`"
        install_name_tool -change $P $PREL $1
    done
}


# adjustLinkDyLib $INPUT_LIB
adjustLinkExtTool $INPUT_LIB
