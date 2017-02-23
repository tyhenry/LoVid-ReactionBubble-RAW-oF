#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

	//ofSetLogLevel("SensorManager", OF_LOG_VERBOSE);

	ofBackground(0);

	ofW = ofGetWidth();
	ofH = ofGetHeight();

	// setup displays

	displays.push_back(Display(ofW,ofH));

	// setup sensors

	sensors.setNumSensors(numBeams, numCaps);

	serial.listDevices();							// get serial devices
	if (serial.setup(serialIdx, serialSpeed))		// try setup serial
		sensors.connect(&serial);					//	+ connect sensors
	else
		ofLogError("ofApp") 
		<< "could not connect to serial [" << serialIdx << "] "
		<< "at speed " << serialSpeed;

}

//--------------------------------------------------------------
void ofApp::update() {

	int nMsgs = sensors.update();

	int idx = 0;
	for (auto& beam : beams) {
		if (beam.get() != 0) {
			ofLogNotice("ofApp") << "beam [" << idx << "] broken!";
			// trigger video

			// reset
			beam.set(0, false);
		}
		idx++;
	}

	for (auto& cap : caps) {
		//ofLogNotice("ofApp") << ""
	}

}

//--------------------------------------------------------------
void ofApp::draw() {

	ofPushMatrix();

	ofVec2f center(ofGetWidth()*0.5, ofGetHeight()*0.5);

	// IR
	//for (int i = 0; i<numBeams; i++) {

	//	bool alive =
	//		(lastPings[i] != 0.0) &&
	//		(ofGetElapsedTimef() - lastPings[i] < sensorTimeout); // heard from sensor <2s ago

	//															  // draw beams
	//	float hue = ofMap(beamStates[i], 0, 62500, 177, 255, true);
	//	float brt = ofMap(beamStates[i], 0, 62500, 200, 255, true);
	//	ofSetColor(alive ? ofColor::fromHsb(hue, 255, brt) : ofColor(150, 80, 110));
	//	float lw = ofMap(beamStates[i], 0, 62500, 3, 20);
	//	ofSetLineWidth(lw);

	//	ofDrawLine(beamPts[i].first, beamPts[i].second);

	//	// label
	//	char h = 'A' + i;
	//	string sts = ofToString(h) + (alive ? (beamStates[i] > 62500 * .1 ? " BREAK" : "") : " NO signal");
	//	ofSetColor(alive ? ofColor(255) : ofColor(150, 80, 110));
	//	ofVec2f pt = (beamPts[i].first + beamPts[i].second) * 0.5;
	//	pt = center.getInterpolated(pt, 1.1);
	//	float leftShift = 1.0 / (pt.x / center.x) * 30;
	//	ofDrawBitmapString(sts, pt - ofVec2f(leftShift, 0));

	//	// diminish
	//	beamStates[i] *= 0.99;

	//}

	//// CAP
	//for (int i = 0; i<numCaps; i++) {

	//	// draw caps
	//	float hue = ofMap(capStates[i], capLow, capHigh, 177, 255, true);
	//	float brt = ofMap(capStates[i], capLow, capHigh, 200, 255, true);
	//	ofSetColor(ofColor::fromHsb(hue, 255, brt));
	//	float radius = ofMap(capStates[i], capLow, capHigh, 10, 30);

	//	ofDrawCircle(capPts[i], radius);

	//	// label
	//	char h = 'A' + numBeams + i;
	//	bool alive = lastPings[i + numBeams] != 0.0 && ofGetElapsedTimef() - lastPings[i + numBeams] < sensorTimeout;
	//	string sts = ofToString(h) + (alive ? " " + ofToString(capStates[i]) : " no signal");
	//	ofSetColor(alive ? ofColor(255) : ofColor(150, 80, 110));
	//	ofDrawBitmapString(sts, capPts[i] + ofVec2f(-30, 30));

	//}

	ofPopMatrix();

	// Error log

	//if (errorLog != "") {
	//	ofDrawBitmapStringHighlight(errorLog, 10, 30, ofColor(0,100), ofColor(255,0,0,200));
	//}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
