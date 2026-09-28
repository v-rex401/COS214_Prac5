#ifndef SECURITYGUARDS_H
#define SECURITYGUARDS_H

#include <iostream>
#include <string>
#include "FirstResponder.h"

class SecurityGuards : public FirstResponder
{

public:
	SecurityGuards(CommunicationTeam *hub);

	void clearBuilding();

	void issueWarning();

	void requestBackup();

	void receive(const std::string &event);

	~SecurityGuards();
};

#endif
