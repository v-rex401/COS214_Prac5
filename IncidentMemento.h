#ifndef INCIDENTMEMENTO_H
#define INCIDENTMEMENTO_H

class IncidentMemento {

private:
	int threatCount;
	std::map<std::string, Threat> activeThreats;
	std::string stateLabel;

public:
	int getThreatCount();

	std::map getActiveThreats();

	std::string getStateLabel();
};

#endif
