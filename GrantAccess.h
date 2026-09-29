#ifndef GRANTACCESS_H
#define GRANTACCESS_H

#include "AccessControlTeam.h"
#include "Protocol.h"
class GrantAccess : public Protocol
{

private:
	AccessControlTeam *access;
	std::string zone;
	std::string role;

public:
	void execute();

	void undo();

	GrantAccess(AccessControlTeam *a, std::string zone, std::string role);

	std::string getName();

	~GrantAccess();
};

#endif
