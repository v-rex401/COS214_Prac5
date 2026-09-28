#include "FireFighter.h"

#include <iostream>

int FireFighter::getResponseETA()
{
	return 5; //need to change
}

void FireFighter::alertStation(const std::string& location, int buildingNumber)
{
	std::cout << "Alerting fire station at " << location << " for building number " << buildingNumber << "\n";
}
