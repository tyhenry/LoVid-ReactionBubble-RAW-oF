#include "ofApp.h"

float vw = 1280;
float vh = 720;

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(0);

	vid.load("slomo_texture_social_space.mp4");
	vid.play();
	vid.setVolume(0);

	cam.setVerbose(true);
	cam.setDeviceID(1);
	cam.setup(vw, vh);

	colorImg.allocate(vw, vh);
	grayImg.allocate(vw, vh);
	grayBg.allocate(vw, vh);
	grayDiff.allocate(vw, vh);

	bLearnBackground = true;
	threshold = 100;
}

//--------------------------------------------------------------
void ofApp::update(){

	vid.update();

	cam.update();
	
	if (cam.isFrameNew()) {

		colorImg.setFromPixels(cam.getPixels());
		grayImg = colorImg;

		if (bLearnBackground == true) {
			grayBg = grayImg;
			bLearnBackground = false;
		}

		grayDiff.absDiff(grayBg, grayImg);
		grayDiff.threshold(threshold);

		contourFinder.findContours(grayDiff, 100, vw*vh/4, 1, false);
		int nBlobs = contourFinder.nBlobs;

		if (nBlobs > 0) drawCam = true;
		else drawCam = false;
	}

}

//--------------------------------------------------------------
void ofApp::draw(){


	if (drawCam) {
		// draw cam subsection
		float sh = vh;
		float sw = vh*1.3333;
		float sx = vw - sw;
		float sy = 0;
		cam.getTexture().drawSubsection(0,0,1024,768, sx,sy,sw,sh);
	}
	else vid.draw(0,0,1024,768);


	if (debugDraw) {
		grayDiff.draw(0, 0, 640, 360);
		contourFinder.draw(0, 0, 640, 360);
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){


	switch (key) {
	case ' ':
		bLearnBackground = true;
		break;
	case '+':
	case '=':
		threshold++;
		if (threshold > 255) threshold = 255;
		break;
	case '-':
		threshold--;
		if (threshold < 0) threshold = 0;
		break;
	case 'd':
		debugDraw = !debugDraw;
		break;
	}

	cout << "threshold: " << threshold << endl;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){


}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
