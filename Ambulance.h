#ifndef AMBULANCE_H
#define AMBULANCE_H

class Ambulance {


public:
	void dispatch(const std::string& location, int caseType);

	boolean getUnitAvailability();

	void dispatch(int caseType, double lat, double lon);
};

#endif
