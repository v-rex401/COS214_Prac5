#include "IncidentHistory.h"

#include <iostream>

void IncidentHistory::push(IncidentMemento* memento)
{
	this->snapshots.push_back(memento);
}

IncidentMemento* IncidentHistory::pop()
{
	if(this->snapshots.empty() == true)
	{
		std::cerr << "Warning: snapshots is empty\n";
		return nullptr;
	}

	IncidentMemento* memento = this->snapshots.back();
	this->snapshots.pop_back();

	return memento;
}

bool IncidentHistory::isEmpty()
{
	if(this->snapshots.empty() == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

IncidentHistory::~IncidentHistory()
{
	for(IncidentMemento* memento : this->snapshots)
	{
		delete memento;
	}

	this->snapshots.clear();
}
