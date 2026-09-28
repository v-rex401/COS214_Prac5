#include <iostream>
#include "IncidentControl.h"
#include "IncidentState.h"
#include "IncidentMemento.h"

IncidentControl::IncidentControl()
: currentState(new Moderate()), threatCount(0) {}

IncidentControl::~IncidentControl()
{
	delete currentState;
}

void IncidentControl::setState(IncidentState* state)
{
	delete this->currentState;
	this->currentState = state;
}

void IncidentControl::escalate()
{
	this->currentState->escalate(this);
}

void IncidentControl::deescalate()
{
	this->currentState->deescalate(this);
}

void IncidentControl::addThreat(const std::string& location, Threat threat)
{
	this->activeThreats[location] = threat;
	this->threatCount = static_cast<int>(this->activeThreats.size());

	this->escalate();
}

void IncidentControl::removeThreat(const std::string& location)
{
	this->activeThreats.erase(location);
	this->threatCount = static_cast<int>(this->activeThreats.size());

	this->deescalate();
}

void IncidentControl::clearThreats()
{
	this->activeThreats.clear();
	this->threatCount = 0;

	this->setState(new Resolved());
}

int IncidentControl::getThreatCount() const
{
	return this->threatCount;
}

std::string IncidentControl::getState() const
{
	return this->currentState->getLabel();
}

IncidentMemento* IncidentControl::createMemento()
{
	return new IncidentMemento(this->threatCount, this->activeThreats, this->currentState->getLabel());
}

void IncidentControl::restore(IncidentMemento* memento)
{
	if(memento == nullptr)
	{
		std::cerr << "Warning: Null memento\n";
		return;
	}

	this->threatCount = memento->getThreatCount();
	this->activeThreats = memento->getActiveThreats();

	const std::string stateLabel = memento->getStateLabel();

	if(stateLabel == "MODERATE")
	{
		this->setState(new Moderate());
	}
	else if(stateLabel == "URGENT")
	{
		this->setState(new Urgent());
	}
	else if(stateLabel == "EMERGENCY")
	{
		this->setState(new Emergency());
	}
	else if(stateLabel == "RESOLVED")
	{
		this->setState(new Resolved());
	}
	else
	{
		std::cerr << "Warning: Unknown state label: " << stateLabel << "\n";
	}

	delete memento;
}
