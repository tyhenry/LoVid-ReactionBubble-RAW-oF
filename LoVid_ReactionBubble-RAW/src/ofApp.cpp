#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

	//ofSetLogLevel("SensorManager", OF_LOG_VERBOSE);
	//ofSetLogLevel("Cap", OF_LOG_VERBOSE);
	//ofSetLogLevel("Beam", OF_LOG_VERBOSE);
	//ofSetLogLevel("ofApp", OF_LOG_VERBOSE);

	ofDisableArbTex();

	ofBackground(0);

	ofW = ofGetWidth();
	ofH = ofGetHeight();

	//wormBounds.set(0,0,1366,768);
	//wormBounds.set(0,0,683,384);
	wormBounds.set(0,0,400,225); //?try this 
	
	// setup displays

	displays.resize(3);
	for (int i=0; i<3; i++) displays[i] = Display(1366,768);

	// setup cameras

	for (int i = 0; i < 3; i++) {
		cams.push_back(ofCamera());
		cams.back().setPosition(wormBounds.getCenter());
	}
	ofVec3f up(0, 0, -1);
	cams[0].lookAt(wormBounds.getTopLeft(), up);
	cams[1].lookAt(wormBounds.getTopRight(), up);
	cams[2].lookAt(wormBounds.getBottomRight(), up);

	for (auto& c : cams) {
		//c.enableOrtho(); // try this for worms, maybe look cool?
		c.setNearClip(10);
	}

	topDisplay = Display(wormBounds); //Display(1366,768);
	ofVec3f p = wormBounds.getCenter();
	p.z = -700;
	topCam.setPosition(p);
	topCam.lookAt(wormBounds.getCenter(), ofVec3f(0,-1,0)); // +y should be bottom screen


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
	walkingDir.allowExt("mpeg");
	walkingDir.listDir();

	walkingVids.resize(5);
	int idx = 0; int good = 0;
	for (auto& file : walkingDir.getFiles()) {
		string fn = "videos/walking/" + file.getFileName();
		if (walkingVids[idx].load(fn)) {
			walkingVids[idx].play();
			walkingVids[idx].setVolume(0);
			good++;
		}
		idx++;
	}
	ofLogVerbose("ofApp") << "loaded " << good << " walking vids successfully";

	for (int i=0; i<5; i++) {
		walkingVidPlaces.push_back(i);
	}

	bgVid = walkingVids.size() - 1;


	effectPcts.resize(6, 0); // 6 bowls - 0-1 pct effect per bowl

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

	// open cv
	ofVec2f k(kinect1.width, kinect1.height);
	grayImg1.allocate(k.x, k.y);		grayImg2.allocate(k.x, k.y);
	pGrayImg1.allocate(k.x, k.y);		pGrayImg2.allocate(k.x, k.y);
	threshNImg1.allocate(k.x, k.y);		threshNImg2.allocate(k.x, k.y);
	threshFImg1.allocate(k.x, k.y);		threshFImg2.allocate(k.x, k.y);

}

//--------------------------------------------------------------
void ofApp::update() {

	for (auto& vid : walkingVids) {
		vid.update();
	}
	skinVid.update();
	synthVid.update();

	int nMsgs = sensors.update();

	// beams
	int idx = 0;
	for (auto& beam : beams) {
		if (beam.get() == 62500) {

			beamBreak(idx, beam);
			ofLogNotice("ofApp") << "beam [" << idx << "] broken!";
		}
		idx++;
	}

	// caps
	idx = 0;
	for (auto& cap : caps) {
		capSense(idx, cap);
		idx++;
	}

	// kinect

	kinect1.update();
	kinect2.update();

	findPeople(); // get people positions within worm bounds

	// worms

	for (auto& worm : worms) {
		worm.resetForces();
		worm.flock(worms);
		//ofVec2f seek = worm.seek(ofVec2f(ofRandom(640,720), ofRandom(350,400)), 2000);
		//worm.applyForce(seek); // seek mouse
		if (people1.size() > 0) {
			for (auto& p : people1) {
				//ofVec2f seek = worm.seek(p, 1366);
				//worm.applyForce(seek * ofRandom(-.5,1.));
				ofVec2f seek = worm.seek(p, wormBounds.width);
				worm.applyForce(seek);
			}
		}
		//else {
		//	ofVec2f seek = worm.seek(wormBounds.getCenter());
		//	worm.applyForce(seek);
		//}
		if (people2.size() > 0) {
			for (auto& p : people2) {
				//ofVec2f seek = worm.seek(p, 1366);
				//worm.applyForce(seek * ofRandom(-.5,1.));
				ofVec2f seek = worm.seek(p, wormBounds.width);
				worm.applyForce(seek);
			}
		}
		//else {
		//	ofVec2f seek = worm.seek(wormBounds.getCenter());
		//	worm.applyForce(seek);
		//}
		worm.update();
	}

}

//--------------------------------------------------------------
void ofApp::draw() {

	// BACKGROUND VID

	// cap effects
	ofColor tint = ofColor::white;
	//for (int i=0; i<caps.size(); i++){
	//	float hue = (255./caps.size()) * i;
	//	float a = 255.*effectPcts[i];
	//	ofColor c = ofColor::fromHsb(hue,255,200);
	//	tint.lerp(c,effectPcts[i]);
	//}

	ofPushStyle();
	ofSetColor(tint);
	float bgX = 0;
	for (int i = 0; i < 3; i++) {
		walkingVids[bgVid].draw(bgX,0, 1366,768);
		bgX += 1366;
	}

	// WALKING VIDS

	float vidX = 0;
	for (int i = 0; i < numBeams; i++) {
		auto& vid = walkingVids[walkingVidPlaces[i]];
		float timeElapsedSinceBreak = ofGetElapsedTimef() - beams[i].get();
		int alpha = ofMap(timeElapsedSinceBreak, 0, 30, 255, 0, true);
		ofSetColor(tint.r,tint.g,tint.b, alpha);
		vid.draw(vidX,0,1366,768);
		vidX += 1366*0.5;
	}
	ofPopStyle();

	// CAMS

	// cam 0

	displays[0].begin();
	ofClear(0);
	cams[0].begin(displays[0].getBounds());
	ofEnableDepthTest();

	for (auto& worm : worms) {
		ofSetColor(255);
		skinVid.bind();
		worm.drawMesh();
		skinVid.unbind();
	}

	ofDisableDepthTest();
	cams[0].end();
	displays[0].end();

	// cam 1

	displays[1].begin();
	ofClear(0);
	cams[1].begin(displays[1].getBounds());
	ofEnableDepthTest();

	for (auto& worm : worms) {
		ofSetColor(255);
		skinVid.bind();
		worm.drawMesh();
		skinVid.unbind();
	}

	ofDisableDepthTest();
	cams[1].end();
	displays[1].end();

	// cam 2

	displays[2].begin();
	ofClear(0);
	cams[2].begin(displays[2].getBounds());
	ofEnableDepthTest();

	for (auto& worm : worms) {
		ofSetColor(255);
		skinVid.bind();
		worm.drawMesh();
		skinVid.unbind();
	}

	ofDisableDepthTest();
	cams[2].end();
	displays[2].end();


	// draw cams to three screens
	float dispX = 0;
	for (int i = 0; i < 3; i++) {
		displays[i].draw(dispX, 0);
		dispX += 1366;
	}


	// DEBUG VIEW

	if (drawTop) {

		topDisplay.begin();
		ofClear(ofColor::darkGrey);
		topCam.begin(topDisplay.getBounds());
		ofEnableDepthTest();

		for (auto& worm : worms) {
			ofSetColor(255);
			worm.draw();
			worm.drawMesh();
		}

		ofSetColor(ofColor::red);
		ofDrawSphere(cams[0].getGlobalPosition() + cams[0].getLookAtDir() * 50, 3);
		cams[0].draw();
		ofSetColor(ofColor::green);
		ofDrawSphere(cams[1].getGlobalPosition() + cams[1].getLookAtDir() * 50, 3);
		cams[1].draw();
		ofSetColor(ofColor::blue);
		ofDrawSphere(cams[2].getGlobalPosition() + cams[2].getLookAtDir() * 50, 3);
		cams[2].draw();

		ofSetColor(255);
		ofDrawSphere(3);

		ofPushMatrix();
		ofTranslate(cams[0].getGlobalPosition());
		ofDrawAxis(100);
		ofPopMatrix();

		ofDrawAxis(100);

		ofDisableDepthTest();
		topCam.end();


		ofSetColor(ofColor::coral);
		for (auto& p : people1) { ofDrawSphere(p, 10); }
		for (auto& p : people2) { ofDrawSphere(p, 10); }

		ofSetColor(255);

		ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()) + " fps", 100, 100);

		topDisplay.end();
		topDisplay.draw(0, 0);

		//ofSetColor(255, 50);
		ofPushMatrix();
		ofTranslate(wormBounds.width,0);
		grayImg2.draw(0, 0, 320, 240);
		contourFinder2.draw(0, 0, 320, 240);
		grayImg1.draw(320, 0, 320, 240);
		contourFinder1.draw(320, 0, 320, 240);
		ofPopMatrix();
	}
	

}

void ofApp::exit()
{
	kinect1.close();
	kinect2.close();
}



// BEAM / CAP LOGIC:


void ofApp::beamBreak(int beamIdx, Beam& beam) { // trigger or switch video

	int vidIdx = bgVid;
	while (vidIdx == bgVid) {
		vidIdx = ofRandom(4.999);
	}
	walkingVidPlaces[beamIdx] = vidIdx;

	ulong time = (ulong)ofGetElapsedTimef();
	beam.set(time,false); // manually set to current time

	ofLogVerbose("ofApp") << "beam [" << beamIdx << "] is now vid [" << vidIdx << "]";
}


void ofApp::capSense(int capIdx, Cap& cap) {

	float val = ofMap(cap.get(), cap.getMin(), cap.getMax(), 0, 1, true); // map to tracking min/max
	effectPcts[capIdx] = ofMap(val, capThreshold, 1.0, 0, 1, true); // threshold at 0.2

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

void ofApp::findPeople() {


	if (kinect1.isFrameNew()) {

		if (gotImg1 && !gotBg1) {
			pGrayImg1 = grayImg1;
			gotBg1 = true;
		}


		// load grayscale depth image from the kinect source
		grayImg1.setFromPixels(kinect1.getDepthPixels());

		//grayImg1.absDiff(pGrayImg1);

		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		threshNImg1 = grayImg1;
		threshFImg1 = grayImg1;		

		threshNImg1.threshold(nearThreshold, true);
		threshFImg1.threshold(farThreshold);

		cvAnd(threshNImg1.getCvImage(), threshFImg1.getCvImage(), grayImg1.getCvImage(), NULL);

		// update the cv images
		grayImg1.flagImageChanged();
		gotImg1 = true;

		if (gotBg1) {
			grayImg1.absDiff(pGrayImg1); // bg subtraction
			grayImg1.erode(); grayImg1.erode(); grayImg1.erode(); grayImg1.erode();
		}

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder1.findContours(grayImg1, 500, 20000, 20, false);

		people1.clear();

		float mapH = wormBounds.height * 0.5;
		float mapW = mapH / 1.333;

		float xOffset = mapW * 3;
		float yOffset = mapH * 0.5;

		for (int i=0; i<contourFinder1.nBlobs; i++){
			auto& blob = contourFinder1.blobs[i];
			ofVec2f bPos = blob.centroid;
			ofVec2f pos;
			pos.x = xOffset + ofMap(bPos.y, 0, 640, 0, mapW, true);
			pos.y = yOffset + ofMap(bPos.x, 0, 480, mapH, 0, true);
			people1.push_back(pos);
		}

	}

	if (kinect2.isFrameNew()) {

		if (gotImg2 && !gotBg2) {
			pGrayImg2 = grayImg2;
			gotBg2 = true;
		}

		// load grayscale depth image from the kinect source
		grayImg2.setFromPixels(kinect2.getDepthPixels());

		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		threshNImg2 = grayImg2;
		threshFImg2 = grayImg2;

		threshNImg2.threshold(nearThreshold, true);
		threshFImg2.threshold(farThreshold);

		cvAnd(threshNImg2.getCvImage(), threshFImg2.getCvImage(), grayImg2.getCvImage(), NULL);

		// update the cv images
		grayImg2.flagImageChanged();
		gotImg2 = true;

		if (gotBg2) {
			grayImg2.absDiff(pGrayImg2); // bg subtraction
			grayImg2.erode(); grayImg2.erode(); grayImg2.erode(); grayImg2.erode();
		}

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder2.findContours(grayImg2, 500, 20000, 20, false);

		people2.clear();

		float mapH = wormBounds.height * 0.5;
		float mapW = mapH / 1.333;

		float xOffset = mapW;
		float yOffset = mapH*0.5;

		for (int i = 0; i<contourFinder2.nBlobs; i++) {
			auto& blob = contourFinder2.blobs[i];
			ofVec2f bPos = blob.centroid;
			ofVec2f pos;
			pos.x = xOffset + ofMap(bPos.y, 0, 640, 0, mapW, true);
			pos.y = yOffset + ofMap(bPos.x, 0, 480, mapH, 0, true);
			people2.push_back(pos);
		}
	}
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {


	if (key == 't') drawTop = !drawTop;
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

	// updates: randomize maxspeed, maxforce and wriggle size
	// also, worm wriggle speed is modulated by sin
	float maxSpeed = ofRandom(0.2,0.8);
	float maxForce = ofRandom(0.03,0.15);
	worms.push_back(Worm(wormBounds, wormBounds.getCenter(), 3., maxSpeed, maxForce));
	worms.back().wriggle = ofRandom(1.,8.);
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
