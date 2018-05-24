/*

Interactive Fractal Tree
By Sander de Jong & Pieter Pierrot


Inspiration taken from and code adapted from: 
	- Recursive Tree Processing example code
	  Daniel Shiffman
	  https://processing.org/examples/tree.html 
	  Consult License.txt for more info
	- openFrameworks example code
	  CameraOffsetExample

*/



#include "ofApp.h"
#include "FixMath.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableSmoothing();
	ofSetVerticalSync(true);
	
	video.setup(320, 240);
	finder.setup("haarcascade_frontalface_default.xml");
	
	previewCamera.setDistance(3.0f);
	previewCamera.setNearClip(0.01f);
	previewCamera.setFarClip(500.0f);
	previewCamera.setPosition(0.4f, 0.2f, 0.8f);
	previewCamera.lookAt(ofVec3f(0.0f, 0.0f, 0.0f));
	
	headTrackedCamera.setNearClip(0.01f);
	headTrackedCamera.setFarClip(1000.0f);
	
	//defining the real world coordinates of the window which is being headtracked is important for visual accuracy
	windowWidth = 0.3f;
	windowHeight = 0.2f;
	
	windowTopLeft = ofVec3f(-windowWidth / 2.0f,
							+windowHeight / 2.0f,
							0.0f);
	windowBottomLeft = ofVec3f(-windowWidth / 2.0f,
							   - windowHeight / 2.0f,
							   0.0f);
	windowBottomRight = ofVec3f(+windowWidth / 2.0f,
								-windowHeight / 2.0f,
								0.0f);
	
	//we use this constant since we're using a really hacky headtracking in this example
	//if you use something that can properly locate the head in 3d (like a kinect), you don't need this fudge factor
	viewerDistance = 0.4f;


	// Initialise interaction parameters
	len = 200;		

	lenFac = 0.66;	
	theta = 60;		
	xTilt = 0;			

	blobX = 0;		

	lastWidth = -INFINITY;
	currentWidth = 0;

	lenBuf = 0;
	
	bg = false; 
}



//--------------------------------------------------------------
void ofApp::update(){
	video.update();
	finder.findHaarObjects(video.getPixels());
	
	ofVec3f headPosition(0,0,viewerDistance);
	
	if (finder.blobs.size() > 0) {
		//get the head position in camera pixel coordinates
		const ofxCvBlob & blob = finder.blobs.front();
		float cameraHeadX = blob.centroid.x;
		float cameraHeadY = blob.centroid.y;
		blobX = blob.centroid.x;				// x-Position of head that is being tracked
		
		
		//since camera isn't mirrored, high x in camera means -ve x in world
		float worldHeadX = ofMap(cameraHeadX, 0, video.getWidth(), windowBottomRight.x, windowBottomLeft.x);
		
		//low y in camera is +ve y in world
		float worldHeadY = ofMap(cameraHeadY, 0, video.getHeight(), windowTopLeft.y, windowBottomRight.y);
		
		//set position in a pretty arbitrary way
		headPosition = ofVec3f(worldHeadX, worldHeadY, viewerDistance);
	} else {
		if (!video.isInitialized()) {
			//if video isn't working, just make something up
			headPosition = ofVec3f(0.5f * windowWidth * sin(ofGetElapsedTimef()), 0.5f * windowHeight * cos(ofGetElapsedTimef()), viewerDistance);
		}
	}
	
	headPositionHistory.push_back(headPosition);
	while (headPositionHistory.size() > 50.0f){
		headPositionHistory.pop_front();
	}

	headTrackedCamera.setPosition(headPosition);
	headTrackedCamera.setupOffAxisViewPortal(windowTopLeft, windowBottomLeft, windowBottomRight);
}



//--------------------------------------------------------------
void ofApp::draw() {

	drawScene();

}



//--------------------------------------------------------------
void ofApp::drawScene(){

	// Create 2x4 color palettes, 4 on black background, 4 on white background
	Palette p1, p2, p3, p4;
	if (bg) {

		ofBackground(255);

		p1 = { 0, 50, 0, 50, 0, 50 };
		p2 = { 0, 100, 0, 100, 0, 100 };
		p3 = { 0, 150, 0, 150, 0, 150 };
		p4 = { 0, 250, 0, 250, 0, 250 };

	}
	else {

		ofBackground(0);

		p1 = { 230, 255, 230, 255, 230, 255 };
		p2 = { 150, 255, 150, 255, 150, 255 };
		p3 = { 100, 255, 100 , 255, 100, 255 };
		p4 = { 25, 255, 25, 255, 25, 255 };
	}


	// Is there a face in the camera input? 
	if (finder.blobs.size() != 0) {

		if (lastWidth == -INFINITY)
			lastWidth = finder.blobs[0].boundingRect.width;		// buffer with last known face z-position
		currentWidth = finder.blobs[0].boundingRect.width;		// current face z-position
		blobX = finder.blobs[0].boundingRect.x;					// current face x-position

		

		// Face x- and z-positions are clipped
		ofClamp(finder.blobs[0].boundingRect.width, 10, 200);
		ofClamp(finder.blobs[0].boundingRect.x, 0, 250);


		// Length of tree stem
		len = ofMap(currentWidth, 10, 200, 200, 600);
			

		// Factor with which branches get smaller relative to parent
		lenFac = ofMap(currentWidth, 10, 200, 0.6, 0.7);
			

		// Angle at which branch is placed relative to parent
		theta = ofMap(currentWidth, 10, 200, 0, 110);
			

		// Angle bias relative to symmetric branching
		xTilt = ofMap(finder.blobs[0].boundingRect.x, 50, 250, -50, 50);

		lastWidth = currentWidth;

	}

	// Generate tree with appropriate color palette 
	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight());
	if (len < 300)
		branch(len, p1);
	else if (len < 350)
		branch(len, p2);
	else if (len < 400)
		branch(len, p3);
	else
		branch(len, p4);
	ofPopMatrix();
}



//--------------------------------------------------------------
void ofApp::branch(float len, Palette p) {

	// Map the length of a branch its strokeweight
	float strokeWeight = ofMap(len, 2, 120, 1, 3);
	ofSetLineWidth(strokeWeight);

	// Choose pseudo-random values from the color palette of this branch
	ofSetColor(ofRandom(p.r1, p.r2), ofRandom(p.g1, p.g2), ofRandom(p.b1, p.b2));

	// Draw line 
	ofDrawLine(0, 0, 0, -len);

	// Translate to end of that line
	ofTranslate(0, -len);

	// The more parents a branch has, the shorter it becomes
	len *= lenFac;
	
	if (len > 2) {

		// Right branch 
		ofPushMatrix();					// Push and pop on every recursive level to maintain proper tree structure 
		ofRotate(theta - xTilt);   
		branch(len, p);					// Recursive step 
		ofPopMatrix();     

		// Left branch 
		ofPushMatrix();
		ofRotate(-theta - xTilt);
		branch(len, p);					// Recursive step 
		ofPopMatrix();
	}
}

//---------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Switch to negative colors with spacebar 
	if (key == ' ')
		(bg == true) ? bg = false : bg = true;
}