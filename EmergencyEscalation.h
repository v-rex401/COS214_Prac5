#ifndef EMERGENCYESCALATION_H
#define EMERGENCYESCALATION_H

class EmergencyEscalation : Protocol {

private:
	SecurityGuards* guards;
	FacilityStaff* facility;
	AccessControlTeam* access;
	FirstAidTeam* medics;
	EmergencyResponder* police;
	EmergencyResponder* ambulance;
	EmergencyResponder* fire;

public:
	void undo();

	void execute();

	EmergencyEscalation(SecuritGuards* s, FacilityStaff* f, AccessControlTeam* a, FirstAidTeam* m, EmergencyResponder* p, EmergencyResponder* fire, EmergencyResponder* am);

	void ~EmergencyEscalation();

	void ~EmergencyEscalation();
};

#endif
