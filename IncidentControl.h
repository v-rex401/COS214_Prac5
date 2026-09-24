#ifndef INCIDENTCONTROL_H
#define INCIDENTCONTROL_H

class IncidentControl {

private:
	IncidentState* currentState;
	int threatCount;
	std::map<std::string, Threat> activeThreats;

public:
	IncidentControl();

	void ~IncidentControl();

	void setState(IncidentState* state);

	void escalate();

	void deescalate();

	void addThreat(const std::string& location, Threat threat);

	void removeThreat(const std::string& location);

	void clearThreats();

	int getThreatCount();

	const std::string getState();

	IncidentMemento* createMemento();

	void restore(IncidentMemento* memento);

	void ~IncidentControl();

	void addThreat(const std::string& location, Threat threat);

	void removeThreat(const std::string& location);
};

#endif
