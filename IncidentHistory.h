#ifndef INCIDENTHISTORY_H
#define INCIDENTHISTORY_H

class IncidentHistory {

private:
	std::stack<IncidentMemento*> snapshots;

public:
	void push(IncidentMemento* memento);

	IncidentMemento* pop();

	bool isEmpty();

	void ~IncidentHistory();

	void ~IncidentHistory();
};

#endif
