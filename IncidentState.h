#ifndef INCIDENTSTATE_H
#define INCIDENTSTATE_H

#include <string>

class IncidentControl;

class IncidentState
{

public:
	virtual void escalate(IncidentControl *context) = 0;

	virtual void deescalate(IncidentControl *context) = 0;

	virtual std::string getLabel() = 0;
};

#endif
