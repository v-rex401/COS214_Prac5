#ifndef URGENT_H
#define URGENT_H
#include "IncidentState.h"
#include "IncidentControl.h"
#include <string>

class Urgent : public IncidentState
{

public:
	void escalate(IncidentControl *context);

	void deescalate(IncidentControl *context);

	std::string getLabel();

	~Urgent();
};

#endif
