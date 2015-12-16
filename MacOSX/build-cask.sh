#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/env-path.sh

GITHUB_USER='kribe'
CASK_DIR=$(brew --prefix)/Library/Taps/caskroom/homebrew-cask
CASK_SRC_DIR=/$ROOT_DIR/homebrew-cask
CASK_QMS_FILE=Casks/qmapshack.rb
CASK_QMS='qmapshack'

function command {
    echo "$CASK_DIR/"
    echo "CREATE BRANCH:" 
    echo "git checkout $CASK_QMS"
    echo "git pull"
    echo "git remote -v"
    echo "git remote add $GITHUB_USER https://github.com/$GITHUB_USER/homebrew-cask"
    echo "git checkout remotes/origin/$CASK_QMS -b $CASK_QMS"
    echo ""
    echo "COMMIT BRANCH:"
    echo "git status"
    echo "git commit -m 'QMapShack v $VERSION_FILE' -v"
    echo "git push $GITHUB_USER $CASK_QMS"
    echo "git checkout master"
    echo ""
    echo "PULL REQUEST ERSTELLEN:"
    echo "https://github.com/$GITHUB_USER/homebrew-cask" 
}

function updateCask {
    cp $CASK_SRC_DIR/$CASK_QMS_FILE ./qmapshack-save-src.rb
    cp $CASK_DIR/$CASK_QMS_FILE ./qmapshack-save.rb
    
    cd $CASK_SRC_DIR
    
    FILE_NAME=`ls $BUILD_RELEASE_DIR/*.tar.gz | sort -n | head -1`
    SHASUM=`shasum -a 256 $FILE_NAME | awk '{ print $1 }'`
    
    VERSION_FILE="${FILE_NAME%.tar.gz}"
    VERSION_FILE="${VERSION_FILE#*MacOSX\_}"
    
    cat $CASK_SRC_DIR/$CASK_QMS_FILE
    echo "$FILE_NAME $SHASUM $VERSION_FILE" 
    sed "s/version .*/version '$VERSION_FILE'/" $CASK_SRC_DIR/$CASK_QMS_FILE > ./qmapshack.tmp
    sed "s/sha256.*/sha256 '$SHASUM'/"  ./qmapshack.tmp > $CASK_SRC_DIR/$CASK_QMS_FILE
    rm ./qmapshack.tmp
    cat $CASK_SRC_DIR/$CASK_QMS_FILE
    cp $CASK_SRC_DIR/$CASK_QMS_FILE $CASK_DIR/$CASK_QMS_FILE
    brew cask install $CASK_QMS
    brew cask audit $CASK_QMS --download
    
    command
    
}

if [[ "$1" == "cask" ]]; then
    updateCask
fi