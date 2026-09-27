#include "FirstAidTeam.h"
#include <iostream>

FirstAidTeam::FirstAidTeam(CommunicationTeam *hub) : FirstResponder(hub)
{
}

void FirstAidTeam::treatInjury()
{
	std::cout << "[FIRST AID] Treating injury on-site.\n";
}

void FirstAidTeam::checkInjury()
{
	std::cout << "[FIRST AID] Assessing injury severity.\n";
}

void FirstAidTeam::emergencyEscalation()
{
	std::cout << "[FIRST AID] Emergency escalated - advanced medical support requested.\n";
	changed("MEDICAL_EMERGENCY_ESCALATED");
}

void FirstAidTeam::receive(const std::string &event)
{
	std::cout << "[FIRST AID] Event received: " << event << "\n";

	if (event == "INJURY_REPORTED")
	{
		checkInjury();
	}
	if (event == "INCIDENT_ESCALATED")
	{
		treatInjury();
	}
}
