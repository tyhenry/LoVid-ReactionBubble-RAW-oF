#pragma once

#include "ofMain.h"
#include "Commander.hpp"

#define ulong unsigned long

class Sensor {

public:
	
	Sensor()
	: value(0), lastPing(0) {}
	~Sensor() {}

	virtual void set(ulong val, bool doPing=true)
	{ 
		value = val; 
		if (doPing) ping();
	}
	void ping() { lastPing = ofGetElapsedTimef(); }

	virtual ulong get()		{ return value; }

	float getLastPing()				{ return lastPing; }
	float getTimeSincePing()		{ return ofGetElapsedTimef() - lastPing; }

protected:

	ulong value;
	float lastPing;

};