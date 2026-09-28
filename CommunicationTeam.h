#ifndef COMMUNICATIONTEAM_H
#define COMMUNICATIONTEAM_H

#include <string>

class FirstResponder;

class CommunicationTeam
{

public:
	virtual void notify(FirstResponder *r, const std::string &event) = 0;

	virtual ~CommunicationTeam() {};
};

#endif
