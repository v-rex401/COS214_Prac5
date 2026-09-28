#ifndef ACCESSCONTROLTEAM_H
#define ACCESSCONTROLTEAM_H

#include <string>
#include "FirstResponder.h"
#include "CommunicationTeam.h"

class AccessControlTeam : public FirstResponder
{

public:
	AccessControlTeam(CommunicationTeam *hub);

	void unlockZone(const std::string &zone);

	void lockdownZone(const std::string &zone);

	void grantEmergencyAccess();

	void revokeAccess(const std::string &zone, const std::string &role);

	void broadcastRestriction(const std::string &zone);

	void receive(const std::string &event);

	~AccessControlTeam();
};

#endif
