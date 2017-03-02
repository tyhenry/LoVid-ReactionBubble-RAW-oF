#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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


		ofVideoPlayer		vid;
		ofVideoGrabber 		cam;

		ofxCvColorImage			colorImg;

		ofxCvGrayscaleImage 	grayImg;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

		ofxCvContourFinder 	contourFinder;

		int 				threshold;
		bool				bLearnBackground;

		bool drawCam = false;
		bool debugDraw = false;

		float camStartTime = 0; // times how long the webcam has been active, if over 10 minutes, re-learn background
		
};
