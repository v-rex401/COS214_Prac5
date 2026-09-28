#include "FireFighterAdapter.h"

FireFighterAdapter::FireFighterAdapter(FireFighter* f)
	: adaptee(f) {}

void FireFighterAdapter::respond(const std::string& location, Threat threat)
{
	int buildingNumber = 0; //need to change

	switch(threat)
	{
		case FIGHT:
		buildingNumber = 1;
		break;

		case SHOOTING:
		buildingNumber = 2;
		break;

		case INJURY:
		buildingNumber = 3;
		break;

		case MEDICAL_EMERGENCY:
		buildingNumber = 4;
		break;

		case FIRE:
		buildingNumber = 5;
		break;

		case DAMAGED:
		buildingNumber = 6;
		break;
	}

	adaptee->alertStation(location, buildingNumber);
}

std::string FireFighterAdapter::getStatus() const
{
	std::string mins = std::to_string(adaptee->getResponseETA());

	return "Firefighter ETA: " + mins + " minutes";
}

FireFighterAdapter::~FireFighterAdapter()
{
	delete adaptee;
}
