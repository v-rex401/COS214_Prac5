#include "Resolved.h"
#include "Moderate.h"
#include <iostream>

void Resolved::deescalate(IncidentControl *context)
{
	std::cout << "Issue has already been Resolved";
}

void Resolved::escalate(IncidentControl *context)
{
	std::cout << "Issue being escalted to MODERATE";
	context->setState(new Moderate());
}
std::string Resolved::getLabel()
{
	return "RESOLVED";
}

Resolved::~Resolved()
{
}
