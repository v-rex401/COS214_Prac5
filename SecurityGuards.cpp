#include "SecurityGuards.h"
#include "FirstResponder.h"

SecurityGuards::SecurityGuards(CommunicationTeam *hub) : FirstResponder(hub)
{
}

void SecurityGuards::clearBuilding()
{
	std::cout << "[SECURITY] Building cleared of occupants.\n";
}

void SecurityGuards::issueWarning()
{
	std::cout << "[SECURITY] Warning issued to campus occupants.\n";
}

void SecurityGuards::requestBackup()
{
	std::cout << "[SECURITY] Backup requested from campus security dispatch.\n";
	changed("SECURITY_BACKUP_REQUESTED");
}

void SecurityGuards::receive(const std::string &event)
{
	std::cout << "[SECURITY] Event received: " << event << "\n";

	if (event == "INCIDENT_ESCALATED" || event == "MEDICAL_EMERGENCY_ESCALATED")
	{
		requestBackup();
	}
	if (event == "INTRUDER_DETECTED" || event == "SHOOTING")
	{
		clearBuilding();
		issueWarning();
	}
}
