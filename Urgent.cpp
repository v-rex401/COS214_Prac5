#include "Urgent.h"
#include "Moderate.h"
#include "Emergency.h"
#include <iostream>

void Urgent::deescalate(IncidentControl *context)
{
	std::cout << "URGENT changing to MODERATE";
	context->setState(new Moderate());
}

void Urgent::escalate(IncidentControl *context)
{
	std::cout << "URGENT escalating to EMERGENCY";
	context->setState(new Emergency());
}

std::string Urgent::getLabel()
{
	return "URGENT";
}
Urgent::~Urgent() {}