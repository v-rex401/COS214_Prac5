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
	access->lockdownZone("EXITS");
	guards->issueWarning();
}

std::string Evacuate::getName()
{
	return "EVECUATE";
}

Evacuate::~Evacuate()
{
}