#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"

class ofApp : public ofBaseApp {

public:

	struct Palette
	{
		int r1;
		int r2;
		int g1;
		int g2;
		int b1;
		int b2;
	};

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	bool bg;

	//Trees
	void branch(float len, Palette p);

	//Headtracking
	void drawScene();
	ofVideoGrabber video;
	ofxCvHaarFinder finder;

	//the view window is defined by 3 corners
	ofVec3f windowTopLeft;
	ofVec3f windowBottomLeft;
	ofVec3f windowBottomRight;

	ofCamera headTrackedCamera;
	ofEasyCam previewCamera;

	float windowWidth;
	float windowHeight;
	float viewerDistance;

	deque<ofPoint> headPositionHistory;

	ofVboMesh window;

	// Interaction parameters

	float len;
	float theta;		
	float lenFac;
	float xTilt;

	float blobX; 

	float lenBuf;

	float lastWidth;
	float currentWidth;

};


