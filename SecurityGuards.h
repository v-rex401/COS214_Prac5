#ifndef SECURITYGUARDS_H
#define SECURITYGUARDS_H

class SecurityGuards : FirstResponder {


public:
	SecurityGuards(CommunicationTeam* hub);

	virtual void receive(const std::string& event) = 0;

	void clearBuilding();

	void issueWarning();

	void requestBackup();

	void receive(const std::string& event);

	void ~SecurityGuards();

	void ~SecurityGuards();
};

#endif
