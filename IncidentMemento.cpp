#include "IncidentMemento.h"

IncidentMemento::IncidentMemento(int threatCount, const std::map<std::string, Threat>& activeThreats, const std::string& stateLabel)
	: threatCount(threatCount), activeThreats(activeThreats), stateLabel(stateLabel) {}

int IncidentMemento::getThreatCount() const
{
	return this->threatCount;
}

std::map<std::string, Threat> IncidentMemento::getActiveThreats() const
{
	return this->activeThreats;
}

std::string IncidentMemento::getStateLabel() const
{
	return this->stateLabel;
}