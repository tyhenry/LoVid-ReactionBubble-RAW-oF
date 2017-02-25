#include "SensorManager.h"

int SensorManager::update()
{
	// read in serial
	int nMsgs = com.update();

	if (nMsgs) {
		ofLogVerbose("SensorManager") << "\n\t" << nMsgs << " new messages at timestamp: "
			<< ofGetElapsedTimef();
	}
	for (int i = 0; i < nMsgs; i++) { // loop thru msgs

		char h = 0;
		unsigned long val = 0;

		if (com.getNext(&h, &val)) {

			// save to archive

			msgs.push_back(Msg(h,val));
			// TO DO: save to disk once msgs reaches certain length

			// check header for which sensor

			SensorType type		= UNKNOWN;
			int index			= -1;

			char firstBeam		= 'A';
			char firstCap		= firstBeam + numBeams;

			// IR
			if (h >= firstBeam		&& h < firstCap) {
				type	= IR;
				index	= h - firstBeam;
			} 

			//CAP
			else if (h >= firstCap	&& h < firstCap + numCaps) {
				type	= CAP;
				index	= h - firstCap;
			}

			else if (h == 'R') {
				type	= RF;
			}

			switch (type)
			{

			case IR:
			{
				if (index >= 0 && index <= beams.size()) {

					// BEAM BREAK
					if (val == 62500) 
					{
						beams[index].set(val);
						ofLogVerbose("SensorManager") << "IR beam " << h << " BREAK - " << val;
					}

					// PING
					else if (val == 0)
					{
						beams[index].ping();
						ofLogVerbose("SensorManager") << "IR beam " << h << " ping - " << val;
					}

					// value error
					else 
					{
						ofLogError("SensorManager") << "IR beam " << h << ", ERROR val - " << val;
					}
				}

				// beam not found!
				else {
					ofLogError("SensorManager") << "IR beam " << h << " - ERROR index [" << index << "] out of bounds, val - " << val;
				}
				break;
			}

			case CAP:
			{
				// set cap sensor value
				if (index >= 0 && index < caps.size())
				{
					caps[index].set(val); // no mapping values yet
					ofLogVerbose("SensorManager") << "CAP sensor " << h << " val - " << val;
				}

				// cap sensor not found!
				else 
				{
					ofLogError("SensorManager") << "CAP sensor " << h << " - ERROR index [" << index << "] out of bounds, val - " << val;
				}
				break;
			}

			case RF: // rf relays
			{
				string relay = "RF relay ";
				if (val == 1) relay += "IR";
				else if (val == 2) relay += "CAP";
				ofLogVerbose("SensorManager") <<  relay << " (" << val << ") pinged";
				break;
			}

			default: // ERROR
			{
				ofLogError("SensorManager") << "ERROR! " << h << " unrecognized, val - " << val;
				break;
			}

			}
		}
	}
	
	if (nMsgs > 0) ofLogVerbose("SensorManager") << "------------------------------";
	return nMsgs;
}



bool SensorManager::input(char header, ulong val)
{
	return false;
}
