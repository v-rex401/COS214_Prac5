#ifndef FIRSTRESPONDER_H
#define FIRSTRESPONDER_H

class FirstResponder {

protected:
	CommunicationTeam* hub;

public:
	FirstResponder(CommunicationTeam* hub);

	virtual void receive(const std::string& event) = 0;

	void changed(const std::string& event);

	virtual void receive(const std::string& event) = 0;

	void changed(const std::string& event);

	virtual void ~FirstResponder() = 0;

	virtual void ~FirstResponder() = 0;
};

#endif
