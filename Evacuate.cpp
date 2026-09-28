#include "Evacuate.h"

Evacuate::Evacuate(SecurityGuards *g, FacilityStaff *f, AccessControlTeam *a)
{
	guards = g;
	facility = f;
	access = a;
}

void Evacuate::execute()
{
	guards->clearBuilding();
	facility->securePremises();
	access->unlockZone("EXITS");
}

void Evacuate::undo()
{
	// TODO - implement Evacuate::undo
	throw "Not yet implemented";
}

Evacuate::~Evacuate()
{
	if (guards != nullptr)
		delete guards;
	if (facility != nullptr)
		delete facility;
	if (access != nullptr)
		delete access;
}