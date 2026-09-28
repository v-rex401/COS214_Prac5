#ifndef EMERGENCY_H
#define EMERGENCY_H

class Emergency : public IncidentState
{

public:
	virtual void escalate(IncidentControl *context);

	void deescalate(IncidentControl *context);

	virtual std::string getLabel();

	~Emergency();
};

#endif
