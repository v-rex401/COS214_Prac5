#include "Isolate.h"

Isolate::Isolate(AccessControlTeam *a, std::string zone)
{
	access = a;
	zone = zone;
}

void Isolate::execute()
{
	access->lockdownZone(zone);
}

void Isolate::undo()
{
	access->unlockZone(zone);
}

Isolate::~Isolate()
{
}