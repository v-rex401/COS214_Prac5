#include "PoliceAdapter.h"

PoliceAdapter::PoliceAdapter(Police* p) 
	: adaptee(p) {}

void PoliceAdapter::respond(const std::string& location, Threat threat)
{
	int severity = 0;
	int incidentCode = 0;

	switch(threat)
	{
		case FIGHT:
		severity = 2;
		incidentCode = 1;
		break;

		case SHOOTING:
		severity = 3;
		incidentCode = 2;
		break;

		case INJURY:
		severity = 1;
		incidentCode = 3;
		break;

		case MEDICAL_EMERGENCY:
		severity = 3;
		incidentCode = 4;
		break;

		case FIRE:
		severity = 3;
		incidentCode = 5;
		break;

		case DAMAGED:
		severity = 1;
		incidentCode = 6;
		break;
	}

	adaptee->dispatch(location, severity, incidentCode);
}

std::string PoliceAdapter::getStatus() const
{
	return adaptee->confirmDeployment();
}

PoliceAdapter::~PoliceAdapter()
{
	delete adaptee;
}
