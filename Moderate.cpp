#include "Moderate.h"
#include <iostream>
#include "IncidentControl.h"
#include "Resolved.h"
#include "Urgent.h"

void Moderate::deescalate(IncidentControl *context)
{
	std::cout << "MODERATE issued resolved changing to RESOLVED";
	context->setState(new Resolved());
}

void Moderate::escalate(IncidentControl *context)
{
	std::cout << "MODERATE issue changing to URGENT";
	context->setState(new Urgent());
}

std::string Moderate::getLabel()
{
	return "Moderate";
}