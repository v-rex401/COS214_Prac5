#include "Moderate.h"
#include <iostream>
#include "IncidentControl.h"
#include "Resolved.h"
#include "Urgent.h"

void Moderate::deescalate(IncidentControl *context)
{
	if (context->getThreatCount() == 0)
	{
		std::cout << "MODERATE issued resolved changing to RESOLVED\n";
		context->setState(new Resolved());
	}
	else
	{
		std::cout << "MODERATE is already the minimum active state\n";
	}
}

void Moderate::escalate(IncidentControl *context)
{
	std::cout << "MODERATE issue changing to URGENT\n";
	context->setState(new Urgent());
}

std::string Moderate::getLabel()
{
	return "MODERATE";
}