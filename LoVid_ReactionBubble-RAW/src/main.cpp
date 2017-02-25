#include "ofMain.h"
#include "ofApp.h"

int ww = 4098;
int wh = 768;

//========================================================================
int main( ){
	ofSetupOpenGL(ww,wh,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
