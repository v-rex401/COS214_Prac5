#include "Emergency.h"
#include "IncidentControl.h"
#include "Moderate.h"
#include "Urgent.h"
#include <iostream>

void Emergency::deescalate(IncidentControl *context)
{
	std::cout << "EMERGENCY deescalating to URGENT";
	context->setState(new Urgent());
}

void Emergency::escalate(IncidentControl *context)
{
	std::cout << "EMERGENCY cannot be escalted any further";
}
