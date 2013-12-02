#!/bin/bash

# Dependencies for project

cd ../../../addons/

addons=( 'emmanuelgeoffray/ofxLabGui.git' 'kylemcdonald/ofxCv.git' 'labatrockwell/ofxLibwebsockets.git' 'memo/ofxMSAInteractiveObject.git' 'emmanuelgeoffray/ofxNI2.git' 'emmanuelgeoffray/ofxOpenCVExtensions.git' 'emmanuelgeoffray/ofxOpenNI.git' 'Spacebrew/ofxSpacebrew.git' 'astellato/ofxSyphon.git' 'emmanuelgeoffray/ofxTSPS.git' 'labatrockwell/ofxTSPSReceiver.git' 'emmanuelgeoffray/ofxTuioWrapper.git')

if [ -z $1 ]; then
    PREFIX="git clone https://github.com/"
else
    PREFIX="git clone git@github.com:"
fi

installOrUpdate(){
    DIRECTORY=`basename $1/`
    DIRECTORY="${DIRECTORY%.*}"
    if [[ -d "${DIRECTORY}" && ! -L "${DIRECTORY}" ]] ; then
      echo "Repo $DIRECTORY already exist, we should update it...\n\r updating..."
      cd $DIRECTORY
      git pull
      git status
      git submodule sync
      git submodule update
      git submodule status
      cd -
      echo "$DIRECTORY updated\n\r"
    else  
      ${PREFIX}$1 $2 $3
    fi  
}

installAll(){
  for i in "${addons[@]}"
  do
    installOrUpdate $i
  done
}

installAll
