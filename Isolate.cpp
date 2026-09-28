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
	// TODO - implement Isolate::undo
	throw "Not yet implemented";
}

Isolate::~Isolate()
{
	if (access != nullptr)
		delete access;
}