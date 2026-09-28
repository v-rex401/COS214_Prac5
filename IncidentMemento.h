#ifndef INCIDENTMEMENTO_H
#define INCIDENTMEMENTO_H

#include <map>
#include <string>

#include "Threat.h"

class IncidentMemento {
private:
	int threatCount;
	std::map<std::string, Threat> activeThreats;
	std::string stateLabel;

public:
	IncidentMemento(int threatCount, const std::map<std::string, Threat>& activeThreats, const std::string& stateLabel);

	int getThreatCount() const;

	std::map<std::string, Threat> getActiveThreats() const;

	std::string getStateLabel() const;
};

#endif
