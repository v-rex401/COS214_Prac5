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
	GrantAccess(AccessControlTeam *a, std::string zone, std::string role);
	void execute();

	void undo();

	std::string getName();

	~GrantAccess();
};

#endif
