#include "tspsApp.h"

class TSPSPersonAttributes {
public:
	TSPSPersonAttributes(){
		height = 0;
		hasBeard = false;
	}

	float height;
	bool hasBeard; //this is a joke
};

//--------------------------------------------------------------
void tspsApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(223, 212, 190);
	
	camWidth = 640;
	camHeight = 480;

	#ifdef _USE_LIVE_VIDEO
    bKinect         = false;
    cameraState     = CAMERA_NOT_INITED;
    
    // are there any kinects out there?
    kinect.init();
    bKinectConnected = (kinect.numAvailableDevices() >= 1);
    
    // no kinects connected, let's just try to set up the device
    if (kinect.numAvailableDevices() < 1 /*|| !peopleTracker.useKinect()*/){
        kinect.clear();        
        bKinect = false;
        initVideoInput();
    } else {
        bKinect = true;
        initVideoInput();
    }
    
  #else
    vidPlayer.loadMovie("testMovies/movie20.mov");
    vidPlayer.play();
    camWidth = vidPlayer.width;
    camHeight = vidPlayer.height;
  #endif

	
  // allocate images + setup people tracker
	colorImg.allocate(camWidth, camHeight);
  grayImg.allocate(camWidth, camHeight);
	
	peopleTracker.setup(camWidth, camHeight);
	peopleTracker.loadFont("fonts/times.ttf", 10);
	peopleTracker.setListener( this );
  /*
	//THIS IS HOW YOU CAN ADD CUSTOM PARAMETERS TO THE GUI
	peopleTracker.addSlider("custom INTEGER", &itestValue, 0, ofGetWidth());
	peopleTracker.addSlider("custom FLOAT", &ftestValue, 0, ofGetHeight());
	peopleTracker.addToggle("custom BOOL", &btestValue);
	*/
	peopleTracker.setActiveDimensions( ofGetWidth(), ofGetHeight()-68 );

	//load GUI / interface images

	personEnteredImage.loadImage("graphic/triggers/PersonEntered_Active.png");
	personUpdatedImage.loadImage("graphic/triggers/PersonUpdated_Active.png");
	personLeftImage.loadImage("graphic/triggers/PersonLeft_Active.png");
	statusBar.loadImage("graphic/scbottomBar.png");
	background.loadImage("graphic/scbackground.png");
	timesBoldItalic.loadFont("fonts/timesbi.ttf", 16);
    
	drawStatus[0] = 0;
	drawStatus[1] = 0;
	drawStatus[2] = 0;
  
  bRecord = false;
  if (bRecord){
    setupRecorder();
  }

}

//--------------------------------------------------------------
void tspsApp::stopRecorder(){
    if (bRecording){
      vidRecorder.encodeVideo();
      bRecording = false;
    } 
}

//--------------------------------------------------------------
void tspsApp::setupRecorder(){
  //create dir
  ofDirectory* dir = new ofDirectory("testMovies");
  if (!dir->exists()) dir->create();
  if (dir != NULL) delete dir;

  //seek an available filename, ie. movie0059.mov
  ofFile* file = NULL;
  int fileNb = 0;
  string filename = "testMovies/movie" + ofToString(fileNb) + ".mov";
  file = new ofFile(filename);
  while (file->exists()){
    fileNb++;
    filename = "testMovies/movie" + ofToString(fileNb) + ".mov";
    if (file != NULL) delete file;
    file = new ofFile(filename);
  }
  if (file != NULL) delete file;
  
  //setup recorder with filename
  vidRecorder.setup(filename, camWidth, camHeight, 22);
  bRecording = true;
  cout << "Start recording to file " << filename << endl;
}

//--------------------------------------------------------------
void tspsApp::update(){
    
    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
    if (peopleTracker.useKinect() && !bKinect){
        bKinect = true;
        initVideoInput();
    } else if (!peopleTracker.useKinect() && bKinect){
        bKinect = false;
        initVideoInput();
    }
    if ( cameraState != CAMERA_NOT_INITED){
        if ( cameraState == CAMERA_KINECT ){
            kinect.update();
            bNewFrame = true;//kinect.isFrameNew();
        } else {
            vidGrabber.grabFrame();
            bNewFrame = vidGrabber.isFrameNew();
        }
    }    
    #else
    vidPlayer.idleMovie();
    bNewFrame = true;//vidPlayer.isFrameNew();
	#endif
    
	if (bNewFrame){        
        #ifdef _USE_LIVE_VIDEO
        if ( cameraState == CAMERA_KINECT ){   
          //grayImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
          //colorImg = grayImg;
          //peopleTracker.update(grayImg);
          //MOD
          colorImg.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
          peopleTracker.update(colorImg);
        } else {
            colorImg.setFromPixels(vidGrabber.getPixels(), camWidth,camHeight);
            peopleTracker.update(colorImg);
            if (bRecording) vidRecorder.addFrame(vidGrabber.getPixelsRef());
        }
	    #else
        colorImg.setFromPixels(vidPlayer.getPixels(), camWidth,camHeight);
        peopleTracker.update(colorImg);
        #endif
        
    #ifdef _USE_LIVE_VIDEO
		//iterate through the people
		for(int i = 0; i < peopleTracker.totalPeople(); i++){
			ofxTSPSPerson* p = peopleTracker.personAtIndex(i);
            if (cameraState == CAMERA_KINECT) p->depth = kinect.getDistanceAt( p->centroid );
		}
    #endif
	}
}

//delegate methods for people entering and exiting
void tspsApp::personEntered( ofxTSPSPerson* newPerson, ofxTSPSScene* scene )
{
	newPerson->customAttributes = new TSPSPersonAttributes();

	//do something with them
	ofLog(OF_LOG_VERBOSE, "person %d of size %f entered!\n", newPerson->pid, newPerson->area);
	drawStatus[0] = 10;
}

void tspsApp::personMoved( ofxTSPSPerson* activePerson, ofxTSPSScene* scene )
{

	//do something with the moving person
	ofLog(OF_LOG_VERBOSE, "person %d of moved to (%f,%f)!\n", activePerson->pid, activePerson->boundingRect.x, activePerson->boundingRect.y);
	drawStatus[1] = 10;
}

void tspsApp::personWillLeave( ofxTSPSPerson* leavingPerson, ofxTSPSScene* scene )
{
	//do something to clean up
	ofLog(OF_LOG_VERBOSE, "person %d left after being %d frames in the system\n", leavingPerson->pid, leavingPerson->age);
	drawStatus[2] = 10;
}

void tspsApp::personUpdated( ofxTSPSPerson* updatedPerson, ofxTSPSScene* scene )
{
	TSPSPersonAttributes* attrbs = (TSPSPersonAttributes*)updatedPerson->customAttributes;
	attrbs->hasBeard = true;

	ofLog(OF_LOG_VERBOSE, "updated %d person\n", updatedPerson->pid);
	drawStatus[1] = 10;
}

//--------------------------------------------------------------
void tspsApp::draw(){
	ofEnableAlphaBlending();
	ofSetHexColor(0xffffff);
	ofPushStyle();
	background.draw(0,0);
	peopleTracker.draw();

	ofPopStyle();

	//draw status bar stuff

	statusBar.draw(0,700);//ofGetHeight()-statusBar.height);
	if (drawStatus[0] > 0){
		drawStatus[0]--;
		personEnteredImage.draw(397,728);
	}
	if (drawStatus[1] > 0){
		drawStatus[1]--;
		personUpdatedImage.draw(533,728);
	}
	if (drawStatus[2] > 0){
		drawStatus[2]--;
		personLeftImage.draw(666,728);
	}

	ofSetColor(0, 169, 157);
	char numPeople[1024];
	sprintf(numPeople, "%i", peopleTracker.totalPeople());
    
	timesBoldItalic.drawString(numPeople,350,740);
}


//--------------------------------------------------------------
void tspsApp::exit(){
    #ifdef _USE_LIVE_VIDEO
    if ( !cameraState == CAMERA_KINECT){  
    }
    #endif
    stopRecorder();
}

//--------------------------------------------------------------
void tspsApp::keyPressed  (int key){
	switch (key){
		case ' ':{
			peopleTracker.relearnBackground();
		} break;
		case 'f':{
			ofToggleFullscreen();
		} break;
		case 'r':{
			(bRecording)? stopRecorder() : setupRecorder();
		} break;
		case OF_KEY_LEFT:{
			peopleTracker.goLeft();
		} break;
		case OF_KEY_RIGHT:{
			peopleTracker.goRight();
		} break;
		case OF_KEY_UP:
    case OF_KEY_DOWN:{
			peopleTracker.goStraight();
		} break;
	#ifndef _USE_LIVE_VIDEO
    case 'a':{
			vidPlayer.firstFrame();
		} break;
    case 'q':{
			vidPlayer.setFrame(vidPlayer.getCurrentFrame()-10);
		} break;
    case 's':{
			vidPlayer.setFrame(vidPlayer.getCurrentFrame()+10);
		} break;
    case 'p':{
      videoPause =! videoPause;
			vidPlayer.setPaused(videoPause);
		} break;
  #endif
	}
}

//--------------------------------------------------------------
void tspsApp::initVideoInput(){
    
#ifdef _USE_LIVE_VIDEO
    if ( bKinect && !cameraState == CAMERA_KINECT ){
        kinect.init();
        bKinectConnected = (kinect.numAvailableDevices() >= 1);
        if (!bKinectConnected){
            bKinect = false;
            return;
        }
        
        if ( cameraState == CAMERA_VIDEOGRABBER ){
            vidGrabber.close();
            cameraState = CAMERA_NOT_INITED;
        }
        
        if ( !cameraState == CAMERA_KINECT){            
            kinect.init();
            //kinect.setVerbose(true);
            bool bOpened = kinect.open();
            if (bOpened){
                cameraState = CAMERA_KINECT;
                //set this so we can access video settings through the interface
                peopleTracker.setVideoGrabber(&kinect, TSPS_INPUT_KINECT);
            }
        }        
    } else {      
        if ( cameraState == CAMERA_NOT_INITED || cameraState == CAMERA_KINECT){
            
            if ( cameraState == CAMERA_KINECT ){
                kinect.close();
                kinect.clear();
                cameraState = CAMERA_NOT_INITED;
            }
            
            vidGrabber.setVerbose(false);
            vidGrabber.videoSettings();
            bool bAvailable = vidGrabber.initGrabber(camWidth,camHeight);
            if (bAvailable){ 
                cameraState = CAMERA_VIDEOGRABBER;
                //set this so we can access video settings through the interface
                peopleTracker.setVideoGrabber(&vidGrabber, TSPS_INPUT_VIDEO);
            }
        }
    }
#endif
    
};

//--------------------------------------------------------------
void tspsApp::closeVideoInput(){
#ifdef _USE_LIVE_VIDEO
    if ( cameraState == CAMERA_KINECT ){
        kinect.close();
        kinect.clear();
        cameraState = CAMERA_NOT_INITED;
    } else if ( cameraState == CAMERA_VIDEOGRABBER ){
        vidGrabber.close();
        cameraState = CAMERA_NOT_INITED;
    }
#endif
}

//--------------------------------------------------------------
void tspsApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void tspsApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void tspsApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void tspsApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void tspsApp::windowResized(int w, int h){}
