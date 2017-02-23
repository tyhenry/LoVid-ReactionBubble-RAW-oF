#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofVec2f center(ofGetWidth()*0.5, ofGetHeight()*0.5);
    
    float beamDist = ofGetHeight()*0.5 - 100;
    float capDist = beamDist*0.5;
    
    ofBackground(0);
    
    float angle = 360.0/numBeams;
    for (int i=0; i<numBeams; i++){
        
        // default state
        beamStates.push_back(0);
        
        // start at 12 o'clock
        ofVec2f beamCenter = center - ofVec2f(0,beamDist);
        
        // rotate
        beamCenter.rotate(angle*i,center);
        
        // get end points of beam
        ofVec2f beamPt1(beamCenter);
        ofVec2f beamPt2(beamCenter);
        beamPt1.rotate(-15,center);
        beamPt2.rotate(15,center);
        
        // save
        beamPts.push_back(pair<ofVec2f,ofVec2f>(beamPt1,beamPt2));
    }
    
    angle = 360.0/numCaps;
    for (int i=0; i<numCaps; i++){
        
        // default state
        capStates.push_back(0);
        
        // start at 12 o'clock
        ofVec2f capPt = center - ofVec2f(0,capDist);
        
        // rotate
        capPt.rotate(angle*i,center);
        
        // save
        capPts.push_back(capPt);
    }
    
    lastPings.resize(numBeams+numCaps,0.0); // tracks time of last sensor's ping
    
	serial.listDevices();
    serial.setup(1, 115200);
    com.setSerial(&serial);

	drawFbo.allocate(ofGetWidth(),ofGetHeight(),GL_RGBA);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // read in serial
    int nMsgs = com.update();

	if (ofStringTimesInString(errorLog, "\n") > 20) {
		auto lines = ofSplitString(errorLog,"\n");
		if (lines.size() > 0)
			errorLog = lines[lines.size()-1];
		else errorLog = "";
	}
    
    // update beam + cap states
    for (int i=0; i<nMsgs; i++){ // loop thru msgs
        
        char h = 0; unsigned long val = 0;
        
        if (com.getNext(&h, &val)) {
            
            // check header
            
            // IR
            if (h >= 'A' && h < 'A'+numBeams){
                
                if (val == 0 || val == 62500) {
                    
                    int idx = h-'A';
                    beamStates[idx] = val;
                    
                    lastPings[idx] = ofGetElapsedTimef();
                    
                    ofLogVerbose("commander") << "beam [" << idx << "] state: " << beamStates[idx] << " << header: " << h << ", val: " << val;
                }
                else {
                    stringstream msg;
					msg << "incorrect val for IR beam - header: " << h << ", val: " << val;
					ofLogError("commander") << msg.str();
					errorLog += msg.str() + "\n";
                }
            }
            
            // CAP
            else if (h >= 'A'+numBeams && h < 'A'+numBeams+numCaps){
                
                int idx = h-'F';
                capStates[idx] = val;
                
                lastPings[idx+numBeams] = ofGetElapsedTimef();
                
                ofLogVerbose("commander") << "cap [" << idx << "] state: " << capStates[idx] << " << header: " << h << ", val: " << val;
            }
            
            else {
				stringstream msg;
				msg << "header out of range (A-" << (int)'A'+numBeams+numCaps-1 << "): " << h << ", val: " << val;
                ofLogError("commander") << msg.str();
				errorLog += msg.str() + "\n";
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofPushMatrix();

    ofVec2f center(ofGetWidth()*0.5, ofGetHeight()*0.5);
    
    // IR
    for (int i=0; i<numBeams; i++){

		bool alive = 
			(lastPings[i] != 0.0) && 
			(ofGetElapsedTimef() - lastPings[i] < sensorTimeout); // heard from sensor <2s ago
    
        // draw beams
        float hue = ofMap(beamStates[i], 0, 62500, 177, 255, true);
        float brt = ofMap(beamStates[i], 0, 62500, 200, 255, true);
        ofSetColor( alive ? ofColor::fromHsb(hue,255,brt) : ofColor(150,80,110));
        float lw = ofMap(beamStates[i],0,62500,3,20);
        ofSetLineWidth(lw);
        
        ofDrawLine(beamPts[i].first,beamPts[i].second);
        
        // label
        char h = 'A' + i;
        string sts = ofToString(h) + (alive ? (beamStates[i] > 62500*.1 ? " BREAK" : "") : " NO signal");
        ofSetColor( alive ? ofColor(255) : ofColor(150,80,110) );
        ofVec2f pt = (beamPts[i].first + beamPts[i].second) * 0.5;
        pt = center.getInterpolated(pt, 1.1);
        float leftShift = 1.0/(pt.x/center.x) * 30;
        ofDrawBitmapString(sts, pt - ofVec2f(leftShift,0));
        
        // diminish
        beamStates[i] *= 0.99;
        
    }
    
    // CAP
    for (int i =0; i<numCaps; i++){
    
        // draw caps
        float hue = ofMap(capStates[i], capLow, capHigh, 177, 255, true);
        float brt = ofMap(capStates[i], capLow, capHigh, 200, 255, true);
        ofSetColor(ofColor::fromHsb(hue, 255, brt));
        float radius = ofMap(capStates[i], capLow, capHigh, 10, 30);
        
        ofDrawCircle(capPts[i], radius);
        
        // label
        char h = 'A' + numBeams + i;
        bool alive = lastPings[i+numBeams] != 0.0 && ofGetElapsedTimef() - lastPings[i+numBeams] < sensorTimeout;
        string sts = ofToString(h) + (alive ? " " + ofToString(capStates[i]) : " no signal");
        ofSetColor( alive ? ofColor(255) : ofColor(150,80,110) );
        ofDrawBitmapString(sts, capPts[i] + ofVec2f(-30,30));
        
    }

	ofPopMatrix();

	// Error log

	//if (errorLog != "") {
	//	ofDrawBitmapStringHighlight(errorLog, 10, 30, ofColor(0,100), ofColor(255,0,0,200));
	//}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
