#include "Ambulance.h"

#include <iostream>

bool Ambulance::getUnitAvailability()
{
	return true; //need to change
}

void Ambulance::dispatch(int caseType, double lat, double lon)
{
	std::cout << "Dispatching ambulance to coordinates (" << lat << ", " << lon << ") for case type " << caseType << "\n";
}
