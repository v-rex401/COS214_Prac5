#ifndef GRANTACCESS_H
#define GRANTACCESS_H

class GrantAccess : Protocol {

private:
	AccessControlTeam* access;
	std::string zone;
	std::string role;

public:
	void execute();

	void undo();

	GrantAccess(AccessControlTeam* a, std::string zone, std::string role);

	GrantAccess(AccessControlTeam* a, std::string zone, std::string role);

	void ~GrantAccess();

	void ~GrantAccess();
};

#endif
