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
  colorImg.allocate(camWidth, camHeight);
  grayImg.allocate(camWidth, camHeight);

  shots.resize(25);
  for (unsigned int i = 0; i < shots.size(); i++){
    shots[i].allocate(camWidth, camHeight);
  }

  peopleTracker.setup(camWidth, camHeight);

    bKinect         = false;
    cameraState     = CAMERA_NOT_INITED;
    
    // are there any kinects out there?
    kinect.init();
    bKinectConnected = (kinect.numAvailableDevices() >= 1);
    
    // are we loading from a video file?
    
    if ( peopleTracker.useVideoFile() || bUseVideoFile ){
        if ( bUseVideoFile && !peopleTracker.useVideoFile()){
            peopleTracker.setUseVideoFile();            
        } else if ( !bUseVideoFile && peopleTracker.useVideoFile() ){
            bUseVideoFile = peopleTracker.useVideoFile();
        }
        
        initVideoFile();
    } else {
        // no kinects connected, let's just try to set up the device
        if (kinect.numAvailableDevices() < 1 || !peopleTracker.useKinect()){
            kinect.clear();        
            bKinect = false;
            initVideoInput();
        } else {
            bKinect = true;
            initVideoInput();
        }
    }    

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
  vidRecorder.setup(filename, camWidth, camHeight, 40);
  bRecording = true;
  cout << "Start recording to file " << filename << endl;
}

//--------------------------------------------------------------
void tspsApp::update(){
    
    if (peopleTracker.useKinect() && !bKinect){
        bKinect = true;
        initVideoInput();
    } else if (!peopleTracker.useKinect() && bKinect){
        bKinect = false;
        initVideoInput();
    } else if ( ( peopleTracker.useVideoFile() && !bUseVideoFile ) || ( peopleTracker.useVideoFile() && videoFile != peopleTracker.getVideoFile()) ){
        closeVideoInput();
        bUseVideoFile = initVideoFile();
    } else if ( !peopleTracker.useVideoFile() && bUseVideoFile){
        bUseVideoFile = false;
        cameraState = CAMERA_NOT_INITED;
        initVideoInput();        
    }

    bool bNewFrame = false;
    
    if ( cameraState != CAMERA_NOT_INITED){
        if ( cameraState == CAMERA_KINECT ){
            kinect.update();
            bNewFrame = true;//kinect.isFrameNew();
        } else if ( cameraState == CAMERA_VIDEOGRABBER ){
            vidGrabber.grabFrame();
            bNewFrame = vidGrabber.isFrameNew();
        } else if ( cameraState == CAMERA_VIDEOFILE ){
            vidPlayer.idleMovie();
            bNewFrame = true; //vidPlayer.isFrameNew();
        }
    }
    
	if (bNewFrame){
        if ( cameraState == CAMERA_KINECT ){   
			grayImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
			colorImg = grayImg;
            peopleTracker.update(grayImg);
        } else if ( cameraState == CAMERA_VIDEOGRABBER ){
            colorImg.setFromPixels(vidGrabber.getPixels(), camWidth,camHeight);
            peopleTracker.update(colorImg);
        } else if ( cameraState == CAMERA_VIDEOFILE ){     
            colorImg.setFromPixels(vidPlayer.getPixels(), camWidth, camHeight);
            peopleTracker.update(colorImg);
        }
        
		// iterate through the people
		for(int i = 0; i < peopleTracker.totalPeople(); i++){
			ofxTSPSPerson* p = peopleTracker.personAtIndex(i);
            if (cameraState == CAMERA_KINECT){
                // distance is in mm, with the max val being 10 m
                // scale it by max to get it in a 0-1 range
                p->depth = (kinect.getDistanceAt( p->highest )/10000.0);
            } else {
                p->depth = p->highest.z / 255.0f;
            }
		}
    shotsUpdate();
	}
}

//--------------------------------------------------------------
void tspsApp::shotsUpdate(){

     float opticalFlowLength = peopleTracker.getOpticalFlowInRegion(0, 0, camWidth, camHeight).length();
    cout << "tspsApp.cpp: optical flow: " << opticalFlowLength << endl;
    if ( opticalFlowLength > 1000){
      shots.push_back(colorImg);
      shots.pop_front();
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
void tspsApp::shotsDraw(){
  float width = camWidth/4;
  float height = camWidth/4;
  float verticalSpace = 20;
  float horizontalSpace = 10;
  int nbInLine = ofGetWidth()/ (width + horizontalSpace);

  for (int i = 0; i < shots.size(); i++){
    float posx = (i % nbInLine) * (width + horizontalSpace);
    float posy = (i / nbInLine) * (height + verticalSpace);
    shots[i].draw(posx, posy, width, height);
  }

}

//--------------------------------------------------------------
void tspsApp::draw(){
	ofEnableAlphaBlending();
	ofSetHexColor(0xffffff);
	ofPushStyle();
	background.draw(0,0);
	//peopleTracker.draw();
  shotsDraw();

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
    if ( cameraState == CAMERA_KINECT){  
        cameraState = CAMERA_NOT_INITED;
        kinect.close();
    } else if ( cameraState == CAMERA_VIDEOGRABBER ){ 
        cameraState = CAMERA_NOT_INITED;
        vidGrabber.close();
    }
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
	}
}

//--------------------------------------------------------------
bool tspsApp::initVideoInput(){
    if ( bKinect && cameraState != CAMERA_KINECT ){
        kinect.init();
        bKinectConnected = (kinect.numAvailableDevices() >= 1);
        if (!bKinectConnected){
            bKinect = false;
            peopleTracker.setUseKinect(false);
            return false;
        }
        
        if ( cameraState == CAMERA_VIDEOGRABBER ){
            vidGrabber.close();
            cameraState = CAMERA_NOT_INITED;
        } else if ( cameraState == CAMERA_VIDEOFILE ){
            vidPlayer.stop();
            vidPlayer.closeMovie();
            vidPlayer.close();
        }
        
        if ( cameraState != CAMERA_KINECT){            
            kinect.init();
            //kinect.setVerbose(true);
            bool bOpened = kinect.open();
            if (bOpened){
                cameraState = CAMERA_KINECT;
                //set this so we can access video settings through the interface
                peopleTracker.setVideoGrabber(&kinect, TSPS_INPUT_KINECT);
                return true;
            } else {
                peopleTracker.setUseKinect(false);
                return false;
            }
        }        
    } else {      
        if ( cameraState != CAMERA_VIDEOGRABBER ){  
            
            if ( cameraState == CAMERA_KINECT ){
                kinect.close();
                kinect.clear();
                cameraState = CAMERA_NOT_INITED;
            } else if (cameraState == CAMERA_VIDEOFILE ){   
                vidPlayer.stop();
                vidPlayer.closeMovie();   
                vidPlayer.close();             
            }
            
            vidGrabber.setVerbose(false);
            //vidGrabber.videoSettings();
            vidGrabber.listDevices();
            vidGrabber.setDeviceID(0);
            bool bAvailable = vidGrabber.initGrabber(camWidth,camHeight);
            if (bAvailable){ 
                cameraState = CAMERA_VIDEOGRABBER;
                //set this so we can access video settings through the interface
                peopleTracker.setVideoGrabber(&vidGrabber, TSPS_INPUT_VIDEO);
                return true;
            } else {
                return false;
            }
        }
    }    
}

//--------------------------------------------------------------
void tspsApp::closeVideoInput(){
    if ( cameraState == CAMERA_KINECT ){
        kinect.close();
        kinect.clear();
        cameraState = CAMERA_NOT_INITED;
    } else if ( cameraState == CAMERA_VIDEOGRABBER ){
        vidGrabber.close();
        cameraState = CAMERA_NOT_INITED;
    }
}

//------------------------------------------------------------------------
bool tspsApp::initVideoFile(){    
    videoFile = peopleTracker.getVideoFile();
    vidPlayer = ofVideoPlayer();
    bool loaded = vidPlayer.loadMovie( videoFile );
    
    if ( loaded ){
        vidPlayer.play();
        if ( camWidth != vidPlayer.width || camHeight != vidPlayer.height ){            
            camWidth    = vidPlayer.width;
            camHeight   = vidPlayer.height;
            peopleTracker.resize( camWidth, camHeight );
        }
        
        // reallocate
        colorImg.resize(camWidth, camHeight);
        grayImg.resize(camWidth, camHeight);
        
        cameraState = CAMERA_VIDEOFILE;        
    }
    
    return loaded;
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
