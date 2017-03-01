#pragma once

//#include "ofMain.h"
#include "Sensor.h"

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
		valuePrev = value;
		value = bMap ? map(val) : val;
		if (ping) lastPing = ofGetElapsedTimef();

		// avg of last (numSamples)
		samples.push_back((float)value);
		if (samples.size() >= maxSamples) {
			samples.pop_front();
			if (avgVal == 0) {
				avgVal = getAvg(); // only once - this will be baseline
			}
		}
		else {
			ofLogVerbose("Cap") << "cap samples: " << samples.size();
		}

		// check if min or max
		if (val < min) {
			min = val;
			ofLogVerbose("Cap") << "new min: " << min;
		}
		if (val > max) {
			max = val;
			ofLogVerbose("Cap") << "new max: " << max;
		}
	}
	
	ulong getPrev() {
		return valuePrev;
	}
	
	float getAvg() {
		return getAvg(samples.size());
	}
	
	float getAvg(int nSamps) {
		if (nSamps <=0 || samples.size() < 1) return 0;
		if (nSamps > maxSamples) nSamps = maxSamples;

		int i=0; float sum = 0;
		for (; i < nSamps && i < samples.size(); i++) {
			sum += samples[0];
		}
		if (i > 0) return sum / i;
		return 0;
	}

	float getBaseAvg() {
		return avgVal;
	}

	ulong getMin()			{ return min; }
	ulong getMax()			{ return max; }

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

	ulong valuePrev = 0;
	float avgVal = 0;
	ulong mapLow, mapHigh, mapInLow, mapInHigh;
	bool bMap, bClamp;

	// tracking min and max for mapping
	ulong min = 65535;
	ulong max = 0;

	// samples for averaging
	deque<float> samples; // num samples
	int maxSamples = 300;
};