#ifndef INCIDENTHISTORY_H
#define INCIDENTHISTORY_H

#include <vector>
#include <string>

#include "IncidentMemento.h"

class IncidentHistory {

private:
	std::vector<IncidentMemento*> snapshots;

public:
	void push(IncidentMemento* memento);

	IncidentMemento* pop();

	bool isEmpty();

	~IncidentHistory();
};

#endif
