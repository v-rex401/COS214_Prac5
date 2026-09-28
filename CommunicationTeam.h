#ifndef COMMUNICATIONTEAM_H
#define COMMUNICATIONTEAM_H
#include "FirstResponder.h"
#include <string>

class CommunicationTeam
{

public:
	virtual void notify(FirstResponder *r, const std::string &event) = 0;

	virtual void notify(FirstResponder *r, const std::string &event) = 0;

	virtual ~CommunicationTeam();
};

#endif
