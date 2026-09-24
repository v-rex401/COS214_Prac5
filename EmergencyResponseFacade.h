#ifndef EMERGENCYRESPONSEFACADE_H
#define EMERGENCYRESPONSEFACADE_H

class EmergencyResponseFacade {

private:
	IncidentControl* controller;
	CommunicationHub* hub;
	SecurityGuards* guards;
	FirstAidTeam* medics;
	FacilityStaff* facility;
	AccessControlTeam* access;
	EmergencyResponder* police;
	EmergencyResponder* ambulance;
	EmergencyResponder* fire;
	IncidentHistory* history;

public:
	EmergencyResponseFacade();

	void ~EmergencyResponseFacade();
private:
	Dispatcher* dispatcher;
public:

	void reportIncident(const std::string& location, Threat threat);

	void escalateToEmergency();

	void resolveIncident();

	void addThreat(const std::string& location, Threat threat);

	void rollbackLastAction();

	void printHistory();

	void ~EmergencyResponseFacade();

	void reportIncident(const std::string& location, Threat threat);

	void addThreat(const std::string& location, Threat threat);
};

#endif
