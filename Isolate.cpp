#include "Isolate.h"

Isolate::Isolate(AccessControlTeam *a, std::string zone)
{
	access = a;
}

void Isolate::execute()
{
	access->lockdownZone("ZONE Lockdown");
	// TODO - CHANGE STATE
}

void Isolate::undo()
{
	// TODO - implement Isolate::undo
	throw "Not yet implemented";
}
