#ifndef URGENT_H
#define URGENT_H

class Urgent : IncidentState {


public:
	virtual void escalate(IncidentControl* context) = 0;

	void deescalate(IncidentControl* context);

	virtual std::string getLabel() = 0;

	void ~Urgent();

	void ~Urgent();
};

#endif
