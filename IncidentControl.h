#ifndef INCIDENTCONTROL_H
#define INCIDENTCONTROL_H

#include <string>
#include <map>

#include "Threat.h"

class IncidentState;
class IncidentMemento;

class IncidentControl {

private:
	IncidentState* currentState;
	int threatCount;
	std::map<std::string, Threat> activeThreats;

public:
	IncidentControl();

	~IncidentControl();

	void setState(IncidentState* state);

	void escalate();

	void deescalate();

	void addThreat(const std::string& location, Threat threat);

	void removeThreat(const std::string& location);

	void clearThreats();

	int getThreatCount() const;

	std::string getState() const;

	IncidentMemento* createMemento();

	void restore(IncidentMemento* memento);
};

#endif
