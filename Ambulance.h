#ifndef AMBULANCE_H
#define AMBULANCE_H

#include <string>

class Ambulance {


public:
	bool getUnitAvailability();

	void dispatch(int caseType, double lat, double lon);
};

#endif
