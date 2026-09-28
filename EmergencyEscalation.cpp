#include "EmergencyEscalation.h"

#include <iostream>

void EmergencyEscalation::undo()
{
	std::cout << "Undoing emergency escalation protocol\n";

	access->revokeAccess("ALL", "EMERGENCY");

	std::cout << "Emergency responders are returning to standby mode\n";
}

void EmergencyEscalation::execute()
{
	std::cout << "Executing emergency escalation protocol\n";

	//issue commands to responders
	guards->clearBuilding();
	facility->securePremises();
	access->grantEmergencyAccess();
	medics->emergencyEscalation();

	//notify responders of escalation
	police->respond("CAMPUS", SHOOTING);
	fire->respond("CAMPUS", FIRE);
	ambulance->respond("CAMPUS", MEDICAL_EMERGENCY);
}

EmergencyEscalation::EmergencyEscalation(SecurityGuards *s, FacilityStaff *f, AccessControlTeam *a, FirstAidTeam *m, EmergencyResponder *p, EmergencyResponder *fire, EmergencyResponder *am)
	: guards(s), facility(f), access(a), medics(m), police(p), fire(fire), ambulance(am) {}

EmergencyEscalation::~EmergencyEscalation()
{
}