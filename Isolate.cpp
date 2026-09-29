#include "Isolate.h"

Isolate::Isolate(AccessControlTeam *a, std::string zone)
{
	access = a;
	this->zone = zone;
}

void Isolate::execute()
{
	access->lockdownZone(zone);
}

std::string getName()
{
	return "ISOLATE";
}

void Isolate::undo()
{
	access->unlockZone(zone);
}

Isolate::~Isolate()
{
}