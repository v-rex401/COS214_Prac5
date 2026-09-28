#include "Assist.h"

Assist::Assist(FirstAidTeam *m, AccessControlTeam *a)
{
	medics = m;
	access = a;
}

void Assist::execute()
{
	medics->checkInjury();
}

void Assist::undo()
{
	// TODO - implement Assist::undo
	throw "Not yet implemented";
}
