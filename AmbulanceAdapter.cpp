#include "AmbulanceAdapter.h"

AmbulanceAdapter::AmbulanceAdapter(Ambulance* a)
	: adaptee(a) {}

void AmbulanceAdapter::respond(const std::string& location, Threat threat)
{
	double lat = 0.0;  //need to change
	double lon = 0.0;  //need to change
	
	int caseType = 0;

	switch(threat)
	{
		case FIGHT:
		caseType = 1;
		break;

		case SHOOTING:
		caseType = 2;
		break;

		case INJURY:
		caseType = 3;
		break;

		case MEDICAL_EMERGENCY:
		caseType = 4;
		break;

		case FIRE:
		caseType = 5;
		break;

		case DAMAGED:
		caseType = 6;
		break;
	}

	adaptee->dispatch(caseType, lat, lon);
}

std::string AmbulanceAdapter::getStatus() const
{
	if(adaptee->getUnitAvailability() == true)
	{
		return "Available";
	}
	else
	{
		return "Unavailable";
	}
}

AmbulanceAdapter::~AmbulanceAdapter()
{
	delete adaptee;
}
