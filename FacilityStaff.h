#ifndef FACILITYSTAFF_H
#define FACILITYSTAFF_H

class FacilityStaff : FirstResponder {


public:
	FacilityStaff(CommunicationTeam* hub);

	virtual void receive(const std::string& event) = 0;

	void dispatchMaintanance();

	void securePremises();

	void receive(const std::string& event);

	void ~FacilityStaff();

	void ~FacilityStaff();
};

#endif
