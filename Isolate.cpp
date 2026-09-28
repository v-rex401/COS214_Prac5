#include "Isolate.h"

Isolate::Isolate(AccessControlTeam *a, std::string zone)
{
	access = a;
}

void Isolate::execute()
{
	access->lockdownZone("ZONE Lockdown");
}

void Isolate::undo()
{
	access->unlockZone(zone);
}

Isolate::~Isolate()
{
}