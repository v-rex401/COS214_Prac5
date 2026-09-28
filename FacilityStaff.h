#ifndef FACILITYSTAFF_H
#define FACILITYSTAFF_H
#include "FirstResponder.h"
#include "CommunicationTeam.h"

class FacilityStaff : public FirstResponder
{

public:
	std::string name;
	FacilityStaff(CommunicationTeam *hub);

	void dispatchMaintenance();

	void securePremises();

	void receive(const std::string &event);

	~FacilityStaff();
};

#endif
