#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

	//ofSetLogLevel("SensorManager", OF_LOG_VERBOSE);
	ofSetLogLevel("Cap", OF_LOG_VERBOSE);

	ofDisableArbTex();

	ofBackground(0);

	ofW = ofGetWidth();
	ofH = ofGetHeight();

	wormBounds.set(0,0,1366,768);


	// setup displays

	for (int i = 0; i < 3; i++) {
		displays.push_back(Display(1366, 768));
		displays.push_back(Display(1366, 768));
		displays.push_back(Display(1366, 768));
	}

	// setup cameras

	for (int i = 0; i < 3; i++) {
		cams.push_back(ofCamera());
		cams.back().setPosition(wormBounds.getCenter());
	}
	ofVec3f up(0, 0, -1);
	cams[0].lookAt(wormBounds.getTopLeft(), up);
	cams[1].lookAt(wormBounds.getTopRight(), up);
	cams[2].lookAt(wormBounds.getBottomRight(), up);

	topDisplay = Display(1366,768);
	ofVec3f p = wormBounds.getCenter();
	p.z = -700;
	topCam.setPosition(p);
	topCam.lookAt(wormBounds.getCenter(), ofVec3f(0,-1,0));

	//topCam.rotate(180,ofVec3f(0,0,1));

	overCam.setTarget(wormBounds.getCenter());

	// setup sensors

	sensors.setNumSensors(numBeams, numCaps);

	serial.listDevices();							// get serial devices
	if (serial.setup(serialIdx, serialSpeed))		// try setup serial
		sensors.connect(&serial);					//	+ connect sensors
	else
		ofLogError("ofApp") 
		<< "could not connect to serial [" << serialIdx << "] "
		<< "at speed " << serialSpeed;

	// load vids

	ofDirectory walkingDir(ofToDataPath("videos/walking"));
	walkingDir.allowExt("mp4");
	walkingDir.listDir();

	for (auto& file : walkingDir.getFiles()) {
		walkingVids.push_back(ofVideoPlayer());
		string fn = "videos/walking/" + file.getFileName();
		if (walkingVids.back().load(fn)) {
			walkingVids.back().play();
			walkingVids.back().setVolume(0);
		}
		else walkingVids.pop_back();
	}
	cout << "loaded " << walkingVids.size() << " walking vids" << endl;

	skinVid.load("videos/skin/slomo_texture_social_space.mp4");
	skinVid.play();
	skinVid.setVolume(0);

	synthVid.load("videos/old/synth.mp4");
	synthVid.play();
	synthVid.setVolume(0);


	// kinect

	kinect1.init();
	kinect1.open();

	kinect2.init();
	kinect2.open();

}

//--------------------------------------------------------------
void ofApp::update() {

	for (auto& vid : walkingVids) {
		vid.update();
	}
	synthVid.update();

	int nMsgs = sensors.update();

	// beams
	int idx = 0;
	for (auto& beam : beams) {
		if (beam.get() != 0) {
			// switch video
			beamBreak(idx, beam);

			// reset
			beam.set(0, false);

			ofLogNotice("ofApp") << "beam [" << idx << "] broken!";
		}
		idx++;
	}

	// caps
	//idx = 0;
	//for (auto& cap : caps) {
	//	capSense(idx, cap);
	//	idx++;
	//}
	// override
	capSense(0, caps[0]);

	// kinect

	kinect1.update();
	kinect2.update();

	// worms

	for (auto& worm : worms) {
		worm.resetForces();
		worm.flock(worms);
		ofVec2f seek = worm.seek(ofVec2f(ofRandom(640,720), ofRandom(350,400)), 2000);
		worm.applyForce(seek); // seek mouse
		worm.update();
	}

}

//--------------------------------------------------------------
void ofApp::draw() {

	displays[0].begin();

	ofClear(0);

	ofSetColor(255);
	walkingVids[0].draw(displays[0].getBounds());
	//
	ofSetColor(255,synthAlpha);
	synthVid.draw(displays[0].getBounds());

	cams[0].begin(displays[0].getBounds());
	//ofEnableDepthTest();

	for (auto& worm : worms) {
		ofSetColor(255);
		worm.draw();
		worm.drawMesh();
	}

	//ofDisableDepthTest();
	cams[0].end();

	displays[0].end();


	displays[1].begin();
	ofClear(0);
	//displays[0].draw();

	cams[1].begin(displays[1].getBounds());
	//ofEnableDepthTest();

	for (auto& worm : worms) {
		ofSetColor(255);
		worm.draw();
		worm.drawMesh();
	}

	//ofDisableDepthTest();
	cams[1].end();

	displays[1].end();

	displays[2].begin();
	ofClear(0);
	//displays[1].draw();

	cams[2].begin(displays[2].getBounds());
	//ofEnableDepthTest();

	for (auto& worm : worms) {
		ofSetColor(255);
		worm.draw();
		worm.drawMesh();
	}

	//ofDisableDepthTest();
	cams[2].end();
	displays[2].end();

	int x = 0;
	for (int i = 0; i < 3; i++) {
		displays[i].draw(x, 0);
		x+=1366;
	}

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()) + " fps", 100, 100);

	
	topDisplay.begin();
	ofClear(ofColor::darkGrey);
	topCam.begin(topDisplay.getBounds());

	for (auto& worm : worms) {
		ofSetColor(255);
		worm.draw();
		worm.drawMesh();
	}

	ofSetColor(ofColor::red); 
	ofDrawSphere(cams[0].getGlobalPosition() + cams[0].getLookAtDir()*50, 10);
	cams[0].draw();
	ofSetColor(ofColor::green); 
	ofDrawSphere(cams[1].getGlobalPosition() + cams[1].getLookAtDir() * 50, 10);
	cams[1].draw();
	ofSetColor(ofColor::blue); 
	ofDrawSphere(cams[2].getGlobalPosition() + cams[2].getLookAtDir() * 50, 10);
	cams[2].draw();

	ofSetColor(255);
	ofDrawSphere(10);

	ofPushMatrix();
	ofTranslate(cams[0].getGlobalPosition());
	ofDrawAxis(100);
	ofPopMatrix();

	ofDrawAxis(100);

	topCam.end();

	kinect1.draw(0,0,320,240);
	kinect2.draw(320,0,320,240);
	topDisplay.end();
	topDisplay.draw(0,0);
	

}

void ofApp::exit()
{
	kinect1.close();
	kinect2.close();
}

void ofApp::capSense(int capIdx, Cap& cap) {

	float val = cap.get();
	val = ofMap(val, cap.getMin(), cap.getMax(), 0, 1, true); // map to tracking min/max

	float a = 255.f * val;
	synthAlpha = (int)a;

	// increase
	//if (val > avg * 1.2) {
	//	if (synthAlpha < 255) synthAlpha++;
	//}
	//// decrease
	//else if (val < avg) {
	//	if (synthAlpha > 0) synthAlpha--;
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

	worms.push_back(Worm(wormBounds));
	cout << "# worms: " << worms.size() << endl;
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
