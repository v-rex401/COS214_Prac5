#ifndef RESOLVE_H
#define RESOLVE_H

#include "FacilityStaff.h"
#include "AccessControlTeam.h"
#include "Protocol.h"

class Resolve : public Protocol
{

private:
	FacilityStaff *facility;
	AccessControlTeam *access;

public:
	void execute();

	void undo();

	Resolve(FacilityStaff *f, AccessControlTeam *a);

	std::string getName();

	~Resolve();
};

#endif
