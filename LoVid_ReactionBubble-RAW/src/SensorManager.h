#pragma once

#include "ofMain.h"
#include "Commander.hpp"
#include "Sensor.h"
#include "Beam.h"
#include "Cap.h"

class SensorManager {

public:

	struct Msg {
		char header = 0;
		ulong value = 0;
		float timestamp = 0;
		Msg(char h = 0, ulong v = 0, float t = ofGetElapsedTimef())
		: header(h), value(v), timestamp(t) {}
	};

	enum SensorType {
		IR, CAP, RF, // (rf relay)
		UNKNOWN = -1
	};
	
	SensorManager()
		: numBeams(0), numCaps(0) {}
	SensorManager(int nBeams, int nCaps)	 
	{
		setNumSensors(nBeams, nCaps);
	}
	~SensorManager() {}

	void setNumSensors(int nBeams, int nCaps)
	{
		if (beams.size() != nBeams) {
			beams.clear();	beams.resize(nBeams);
			numBeams = nBeams;
		}
		if (caps.size() != nCaps) {
			caps.clear();	caps.resize(nCaps);
			numCaps = nCaps;
		}
	}

	bool connect(ofSerial* serial)
	{
		com.setSerial(serial);
		bool b = com.connect();
		ofLogVerbose("SensorManager") << (b ? "":"not? ") << "connnected to device!";
	}

	bool isConnected() { return com.isConnected(); }

	int update();		// returns number of messages found

	bool input(char header, ulong val);

	vector<Beam> beams;
	vector<Cap> caps;

	Commander com;

protected:

	int numBeams;
	int numCaps;

	deque<Msg> msgs; // archive

};