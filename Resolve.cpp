#include "Resolve.h"

void Resolve::execute()
{
	facility->changed("RESOLVED");
	access->changed("RESOLVED");
}

void Resolve::undo()
{
	access->lockdownZone("ALL");
}

std::string Resolve::getName()
{
	return "RESOLVE";
}

Resolve::Resolve(FacilityStaff *f, AccessControlTeam *a)
{
	facility = f;
	access = a;
}

Resolve::~Resolve()
{
}