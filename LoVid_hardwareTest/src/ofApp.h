#pragma once

#include "ofMain.h"
#include "Commander.hpp"

class ofApp : public ofBaseApp{
    
    const int numBeams = 5;
    const int numCaps = 6;
    
    const long capLow = 200;
    const long capHigh = 20000;
    
    const float sensorTimeout = 10.0; // 10s

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
    
    ofSerial serial;
    Commander com;
    
    vector <unsigned long> beamStates;
    vector <pair<ofVec2f, ofVec2f>> beamPts;
    
    vector <unsigned long> capStates; // map caps from capLow-capHigh -> 0-1000
    vector <ofVec2f> capPts;
    
    vector<float> lastPings;
		
};
