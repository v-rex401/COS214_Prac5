#include "FacilityStaff.h"
#include <iostream>

FacilityStaff::FacilityStaff(CommunicationTeam *hub) : FirstResponder(hub)
{
}

void FacilityStaff::dispatchMaintenance()
{
	std::cout << "[FACILITY] Maintenance crew dispatched to affected area.\n";
}

void FacilityStaff::securePremises()
{
	std::cout << "[FACILITY] Premises secured - non-essential systems locked down.\n";
}

void FacilityStaff::receive(const std::string &event)
{
	std::cout << "[FACILITY] Event received: " + event;

	if (event == "MAINTENANCE_ESCALATED" || event == "MEDICAL_EMERGENCY_ESCALATED")
	{
		securePremises();
	}
	if (event == "MAINTENANCE_REQUIRED" || event == "FIRE_DETECTED")
	{
		dispatchMaintenance();
	}
}
