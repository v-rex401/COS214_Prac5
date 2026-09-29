#ifndef EVACUATE_H
#define EVACUATE_H

#include "SecurityGuards.h"
#include "FacilityStaff.h"
#include "AccessControlTeam.h"
#include "Protocol.h"

class Evacuate : public Protocol
{

private:
	SecurityGuards *guards;
	FacilityStaff *facility;
	AccessControlTeam *access;

public:
	Evacuate(SecurityGuards *g, FacilityStaff *f, AccessControlTeam *a);

	void execute();

	void undo();

	std::string getName();

	~Evacuate();
};

#endif
