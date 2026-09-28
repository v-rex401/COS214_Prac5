#ifndef FIRSTRESPONDER_H
#define FIRSTRESPONDER_H
#include "CommunicationHub.h"
#include <string>

class CommunicationTeam;

class FirstResponder
{

private:
	CommunicationTeam *hub; /**This is the mediator */

public:
	FirstResponder(CommunicationTeam *hub);

	virtual void receive(const std::string &event) = 0;

	void changed(const std::string &event);

	virtual ~FirstResponder();
};

#endif
