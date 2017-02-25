#pragma once

#include "ofMain.h"
#include "Display.h"
#include "SensorManager.h"
#include "Worm.h"

#include "ofxKinect.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{


	// setup

	const int serialIdx = 1;
	const int serialSpeed = 115200;

	const int numBeams = 5;
	const int numCaps = 6;

	const long capLow = 200;
	const long capHigh = 20000;

	const float sensorTimeout = 10.0; // 10s

	int nearThreshold = 150;
	int farThreshold = 100;

	ofRectangle wormBounds;


	public:
		void setup();
		void update();
		void draw();
		void exit();

		void beamBreak(int beamIdx, Beam& beam); // trigger or switch video
		void capSense(int capIdx, Cap& cap); // adjust video

		void findPeople();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		float ofW, ofH;

		vector<Display> displays;

		SensorManager sensors;
		vector<Beam> &beams = sensors.beams;
		vector<Cap> &caps	= sensors.caps;

		ofSerial serial;

		string errorLog;


		// videos

		vector<ofVideoPlayer> walkingVids;
		vector<int> walkingVidPlaces;

		ofVideoPlayer synthVid;
		int synthAlpha = 0;

		ofVideoPlayer skinVid;

		// worms
		vector<Worm> worms;

		vector<ofCamera> cams;
		//ofCamera* cam;

		Display topDisplay;
		ofCamera topCam;
		ofEasyCam overCam;

		// kinect

		ofxKinect kinect1, kinect2;

		vector<ofVec3f> people1, people2;

		// - kinect open cv
		//ofxCvColorImage colorImg1, colorImg2; // color depth images

		ofxCvGrayscaleImage grayImg1, grayImg2; // grayscale depth images
		ofxCvGrayscaleImage pGrayImg1, pGrayImg2; // diff for background subtraction
		ofxCvGrayscaleImage threshNImg1, threshNImg2; // the near thresholded images
		ofxCvGrayscaleImage threshFImg1, threshFImg2; // the far thresholded images

		ofxCvContourFinder contourFinder1, contourFinder2;

		bool drawTop = false;

};
