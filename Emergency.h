#ifndef EMERGENCY_H
#define EMERGENCY_H

#include "IncidentControl.h"
#include <string>
class IncidentControl;

class Emergency : public IncidentState
{

public:
	void escalate(IncidentControl *context);

	void deescalate(IncidentControl *context);

	std::string getLabel();

	~Emergency();
};

#endif
