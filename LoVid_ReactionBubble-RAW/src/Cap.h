#pragma once

#include "ofMain.h"
#include "Commander.hpp"

class Cap : public Sensor {

public:

	Cap() :
		mapLow(0),		mapHigh(65535),
		mapInLow(0),	mapInHigh(65535),
		bMap(false),	bClamp(false)
	{}
	~Cap() {}

	void set(ulong val, bool ping = true) override
	{
		value = bMap ? map(val) : val;
		if (ping) lastPing = ofGetElapsedTimef();
	}

	void useMap(bool use)	{ bMap = use; }
	bool isMapping()		{ return bMap; }


	void setMapRange(ulong low, ulong high)
	{
		mapLow = low;		mapHigh = high;
	}
	void setMapRange(ulong low, ulong high, ulong inLow, ulong inHigh, bool clamp = true)
	{
		setMapRange(low, high);
		mapInLow = inLow;	mapInHigh = inHigh;
	}

	vector<ulong> getMapRange() {
		vector<ulong> map;
		map.resize(4);
		map[0] = mapLow;	map[1] = mapHigh;
		map[2] = mapInLow;	map[4] = mapInHigh;
		return map;
	}

	ulong map(ulong val)
	{
		float v = val;
		v = ofMap(v, mapInLow, mapInLow, mapLow, mapHigh, bClamp);
		v = ofClamp(v, 0, 65535); // just in case, no overflow
		return (ulong)v;
	}



private:

	ulong mapLow, mapHigh, mapInLow, mapInHigh;
	bool bMap, bClamp;
};