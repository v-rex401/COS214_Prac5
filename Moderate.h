#ifndef MODERATE_H
#define MODERATE_H

#include "IncidentState.h"
#include "IncidentControl.h"

class Moderate : public IncidentState
{

public:
	virtual void escalate(IncidentControl *context);

	virtual std::string getLabel();

	void deescalate(IncidentControl *context);
};

#endif
