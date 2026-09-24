#ifndef MODERATE_H
#define MODERATE_H

class Moderate : IncidentState {


public:
	virtual void escalate(IncidentControl* context) = 0;

	virtual std::string getLabel() = 0;

	void deescalate(IncidentControl* context);

	void ~Moderate();

	void ~Moderate();
};

#endif
