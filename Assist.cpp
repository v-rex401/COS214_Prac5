#include "Assist.h"
#include <iostream>

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
	std::cout << "[ASSIST] Assistance protocol rolled back.\n";
}

Assist::~Assist()
{
}
