#ifndef EMERGENCYRESPONSEFACADE_H
#define EMERGENCYRESPONSEFACADE_H

#include "IncidentControl.h"
#include "CommunicationHub.h"
#include "FirstAidTeam.h"
#include "SecurityGuards.h"
#include "FacilityStaff.h"
#include "AccessControlTeam.h"
#include "EmergencyResponder.h"
#include "IncidentHistory.h"
#include "Dispatcher.h"

class EmergencyResponseFacade
{

private:
	IncidentControl *controller;
	CommunicationHub *hub;
	SecurityGuards *guards;
	FirstAidTeam *medics;
	FacilityStaff *facility;
	AccessControlTeam *access;
	EmergencyResponder *police;
	EmergencyResponder *ambulance;
	EmergencyResponder *fire;
	IncidentHistory *history;
	Dispatcher *dispatcher;

public:
	EmergencyResponseFacade();

	void reportIncident(const std::string &location, Threat threat);

	void escalateToEmergency();

	void resolveIncident();

	void addThreat(const std::string &location, Threat threat);

	void rollbackLastAction();

	void printHistory();

	~EmergencyResponseFacade();
};

#endif
