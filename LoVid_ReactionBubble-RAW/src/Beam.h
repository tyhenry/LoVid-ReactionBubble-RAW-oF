#pragma once

#include "ofMain.h"
#include "Sensor.h"
#include "Commander.hpp"

class Beam : public Sensor {
	
public:

	Beam() {}
	~Beam() {}

	void setAsTime() 
	{
		value = (ulong)ofGetElapsedTimef();
	}

	ulong decay(float pct) 
	{ 
		value = (ulong)((float)value * pct);
	}
};