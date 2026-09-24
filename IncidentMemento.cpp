#include "IncidentMemento.h"

int IncidentMemento::getThreatCount() {
	return this->threatCount;
}

std::map IncidentMemento::getActiveThreats() {
	// TODO - implement IncidentMemento::getActiveThreats
	throw "Not yet implemented";
}

std::string IncidentMemento::getStateLabel() {
	return this->stateLabel;
}
