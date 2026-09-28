#include "EmergencyResponseFacade.h"

#include "IncidentControl.h"
#include "CommunicationHub.h"
#include "FirstAidTeam.h"
#include "SecurityGuards.h"
#include "FacilityStaff.h"
#include "AccessControlTeam.h"
#include "EmergencyResponder.h"
#include "IncidentHistory.h"
#include "Dispatcher.h"
#include "Threat.h"
#include "FireFighterAdapter.h"
#include "PoliceAdapter.h"
#include "AmbulanceAdapter.h"
#include "Isolate.h"
#include "EmergencyEscalation.h"
#include "Resolve.h"

#include <iostream>

EmergencyResponseFacade::EmergencyResponseFacade()
{
	// create hub
	hub = new CommunicationHub();

	// create responders for hub
	facility = new FacilityStaff(hub);
	medics = new FirstAidTeam(hub);
	guards = new SecurityGuards(hub);
	access = new AccessControlTeam(hub);

	// add responders to hub - FIX: MIGHT NOT BE NEEDED BECUASE ITS IN CONSTRUCTOR
	/* hub->registerResponder(facility);
	hub->registerResponder(medics);
	hub->registerResponder(guards);
	hub->registerResponder(access);
 */

	// create state
	controller = new IncidentControl();

	// create memento
	history = new IncidentHistory();

	// create invoker
	dispatcher = new Dispatcher();

	// create adapters
	FireFighter *firePtr = new FireFighter;
	fire = new FireFighterAdapter(firePtr);

	Police *policePtr = new Police();
	police = new PoliceAdapter(policePtr);

	Ambulance *ambulancePtr = new Ambulance();
	ambulance = new AmbulanceAdapter(ambulancePtr);
}

void EmergencyResponseFacade::reportIncident(const std::string &location, Threat threat)
{
	std::cout << "Reporting incident at " << location << " with threat type: " << threat << "\n";

	// add threat to incident control
	controller->addThreat(location, threat);

	// create memento snapshot + push to incident history
	history->push(controller->createMemento());

	// notify responders of incident
	/* hub->notify(nullptr, "INCIDENT_REPORTED"); */

	// issue commands to responders
	dispatcher->issueCommand(new Isolate(access, location));
}

void EmergencyResponseFacade::escalateToEmergency()
{
	std::cout << "Escalating incident to emergency level\n";

	// escalate incident control state
	controller->escalate();

	// create memento snapshot + push to incident history
	history->push(controller->createMemento());

	// issue emergency escalation command to responders
	dispatcher->issueCommand(new EmergencyEscalation(guards, facility, access, medics, police, fire, ambulance));

	// notify responders of escalation
	/* hub->notify(nullptr, "EMERGENCY_ESCALATED"); */
}

void EmergencyResponseFacade::resolveIncident()
{
	std::cout << "Resolving incident\n";

	// clear threats incident control state
	controller->clearThreats();

	// issue resolve command to responders
	dispatcher->issueCommand(new Resolve(facility, access));

	// create memento snapshot + push to incident history
	history->push(controller->createMemento());

	// notify responders of resolution
	/* hub->notify(nullptr, "INCIDENT_RESOLVED"); */
}

void EmergencyResponseFacade::addThreat(const std::string &location, Threat threat)
{
	std::cout << "Adding threat at " << location << " with threat type: " << threat << "\n";

	controller->addThreat(location, threat);
}

void EmergencyResponseFacade::rollbackLastAction()
{
	std::cout << "Rolling back last action\n";

	// undo the last command issued by the dispatcher
	dispatcher->undoLast();

	// restore the previous state from the incident history
	IncidentMemento *lastMemento = history->pop();

	if (lastMemento != nullptr)
	{
		controller->restore(lastMemento);
	}
	else
	{
		std::cout << "No previous state to rollback to.\n";
	}
}

void EmergencyResponseFacade::printHistory()
{
	std::cout << "Incident History:\n";
	std::stack<Protocol *> printStack = dispatcher->getCommandHistory();
	while (!printStack.empty())
	{
		std::cout << printStack.top() << std::endl;
		printStack.pop();
	}
}

EmergencyResponseFacade::~EmergencyResponseFacade()
{
	// delete in hierachal order
	delete fire;
	delete police;
	delete ambulance;

	delete dispatcher;

	delete history;
	delete controller;

	delete hub;

	delete facility;
	delete medics;
	delete guards;
	delete access;
}
