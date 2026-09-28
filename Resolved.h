#ifndef RESOLVED_H
#define RESOLVED_H

#include "IncidentState.h"
#include "IncidentControl.h"
#include <string>

class Resolved : public IncidentState
{

public:
	void escalate(IncidentControl *context);

	void deescalate(IncidentControl *context);

	std::string getLabel();

	~Resolved();
};

#endif
