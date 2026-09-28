#include "Resolve.h"

void Resolve::execute()
{
	facility->changed("RESOLVED");
	access->changed("RESOLVED");
}

void Resolve::undo()
{
	// TODO - implement Resolve::undo
	throw "Not yet implemented";
}

Resolve::Resolve(FacilityStaff *f, AccessControlTeam *a)
{
	facility = f;
	access = a;
}

Resolve::~Resolve()
{
	if (facility != nullptr)
		delete facility;
	if (access != nullptr)
		delete access;
}