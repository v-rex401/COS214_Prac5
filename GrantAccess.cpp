#include "GrantAccess.h"

GrantAccess::GrantAccess(AccessControlTeam *a, std::string zone, std::string role)
{
	access = a;
	this->zone = zone;
	this->role = role;
}

void GrantAccess::execute()
{
	access->grantEmergencyAccess();
}

void GrantAccess::undo()
{
	// TODO - implement GrantAccess::undo
	throw "Not yet implemented";
}

GrantAccess::GrantAccess(AccessControlTeam *a, std::string zone, std::string role)
{
	// TODO - implement GrantAccess::GrantAccess
	throw "Not yet implemented";
}
