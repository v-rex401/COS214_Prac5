#ifndef RESOLVED_H
#define RESOLVED_H

class Resolved : IncidentState {


public:
	virtual void escalate(IncidentControl* context) = 0;

	void deescalate(IncidentControl* context);

	virtual std::string getLabel() = 0;

	void ~Resolved();

	void ~Resolved();
};

#endif
