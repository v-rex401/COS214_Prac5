#include "GrantAccess.h"

GrantAccess::GrantAccess(AccessControlTeam *a, std::string zone, std::string role)
{
	access = a;
	this->zone = zone;
	this->role = role;
}

void GrantAccess::execute()
{
	access->unlockZone(zone);
	access->grantEmergencyAccess();
}

void GrantAccess::undo()
{
	access->revokeAccess(zone, role);
	access->lockdownZone(zone);
}

std::string getName()
{
	return "GRANT ACCESS";
}

GrantAccess::~GrantAccess()
{
}