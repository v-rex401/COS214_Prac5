#ifndef COMMUNICATIONHUB_H
#define COMMUNICATIONHUB_H
#include "CommunicationTeam.h"
#include "FirstResponder.h"
#include <vector>

class CommunicationHub : public CommunicationTeam
{
private:
	std::vector<FirstResponder *> responders;

public:
	virtual void notify(FirstResponder *r, const std::string &event);

	void removeResponder(FirstResponder *r);

	void registerResponder(FirstResponder *r);

	virtual void notify(FirstResponder *r, const std::string &event);

	~CommunicationHub();
};

#endif
