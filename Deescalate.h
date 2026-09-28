#ifndef DEESCALATE_H
#define DEESCALATE_H

#include "SecurityGuards.h"
#include "FacilityStaff.h"
#include "AccessControlTeam.h"
#include "Protocol.h"

class Deescalate : public Protocol
{
private:
	SecurityGuards *guards;

public:
	Deescalate(SecurityGuards *s);

	void execute();

	void undo();

	~Deescalate();
};

#endif
