#include "Urgent.h"
#include "Moderate.h"
#include "Emergency.h"
#include <iostream>

void Urgent::deescalate(IncidentControl *context)
{
	std::cout << "URGENT changing to MODERATE\n";
	context->setState(new Moderate());
}

void Urgent::escalate(IncidentControl *context)
{
	std::cout << "URGENT escalating to EMERGENCY\n";
	context->setState(new Emergency());
}

std::string Urgent::getLabel()
{
	return "URGENT";
}
Urgent::~Urgent() {}