#ifndef INCIDENTSTATE_H
#define INCIDENTSTATE_H

class IncidentState {


public:
	virtual void escalate(IncidentControl* context) = 0;

	virtual void deescalate(IncidentControl* context) = 0;

	virtual std::string getLabel() = 0;

	virtual void ~IncidentState() = 0;

	virtual void ~IncidentState() = 0;
};

#endif
