#ifndef FIREFIGHTER_H
#define FIREFIGHTER_H

#include <string>

class FireFighter {


public:
	int getResponseETA();

	void alertStation(const std::string& location, int buildingNumber);
};

#endif
