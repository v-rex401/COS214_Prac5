#ifndef COMMUNICATIONHUB_H
#define COMMUNICATIONHUB_H

class CommunicationHub : CommunicationTeam {

public:
	std::vector<FirstResponder*> responders;

	virtual void notify(FirstResponder* r, const std::string& event) = 0;

	void removeResponder(FirstResponder* r);

	void registerResponder(FirstResponder* r);

	virtual void notify(FirstResponder* r, const std::string& event) = 0;

	void ~CommunicationHub();

	void ~CommunicationHub();
};

#endif
