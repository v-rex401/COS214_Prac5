#ifndef EMERGENCY_H
#define EMERGENCY_H

class Emergency : IncidentState {


public:
	virtual void escalate(IncidentControl* context) = 0;

	void deescalate(IncidentControl* context);

	virtual std::string getLabel() = 0;

	void ~Emergency();

	void ~Emergency();
};

#endif
