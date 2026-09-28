#ifndef EMERGENCYESCALATION_H
#define EMERGENCYESCALATION_H

#include "Protocol.h"
#include "SecurityGuards.h"
#include "FacilityStaff.h"
#include "AccessControlTeam.h"
#include "FirstAidTeam.h"
#include "EmergencyResponder.h"

class EmergencyEscalation : public Protocol
{

private:
	SecurityGuards *guards;
	FacilityStaff *facility;
	AccessControlTeam *access;
	FirstAidTeam *medics;
	EmergencyResponder *police;
	EmergencyResponder *ambulance;
	EmergencyResponder *fire;

public:
	void undo();

	void execute();

	EmergencyEscalation(SecurityGuards *s, FacilityStaff *f, AccessControlTeam *a, FirstAidTeam *m, EmergencyResponder *p, EmergencyResponder *fire, EmergencyResponder *am);

	~EmergencyEscalation();
};

#endif
