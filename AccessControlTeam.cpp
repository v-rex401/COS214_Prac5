#include "AccessControlTeam.h"
#include <iostream>

AccessControlTeam::AccessControlTeam(CommunicationTeam *hub) : FirstResponder(hub)
{
}

void AccessControlTeam::unlockZone(const std::string &zone)
{
	std::cout << "[ACCESS CONTROL] Zone unlocked: " << zone << "\n";
}

void AccessControlTeam::lockdownZone(const std::string &zone)
{
	std::cout << "[ACCESS CONTROL] Zone locked down: " << zone << "\n";
}

void AccessControlTeam::grantEmergencyAccess()
{
	std::cout << "[ACCESS CONTROL] Emergency access granted to responders.\n";
}

void AccessControlTeam::revokeAccess(const std::string &zone, const std::string &role)
{
	std::cout << "[ACCESS CONTROL] Access revoked for role '" << role
			  << "' in zone: " << zone << "\n";
}

void AccessControlTeam::broadcastRestriction(const std::string &zone)
{
	std::cout << "[ACCESS CONTROL] Restriction broadcast for zone: " << zone << "\n";
	changed("RESTRICTION_BROADCAST");
}

void AccessControlTeam::receive(const std::string &event)
{
	if (event == "MEDICAL_EMERGENCY_ESCALATED")
	{
		grantEmergencyAccess();
	}
	if (event == "SECURITY_BACKUP_REQUESTED" || event == "INTRUDER_DETECTED")
	{
		lockdownZone("Zone-Unspecified");
	}
	if (event == "INCIDENT_RESOLVED")
	{
		unlockZone("Zone-Unspecified");
	}
}
