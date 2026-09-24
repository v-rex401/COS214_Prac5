#ifndef FIREFIGHTERADAPTER_H
#define FIREFIGHTERADAPTER_H

class FireFighterAdapter : EmergencyResponder {

public:
	FireFighter* adaptee;

	FireFighterAdapter(FireFighter* f);

	virtual void respond(const std::string& location, Threat threat) = 0;

	virtual std::string getStatus() = 0;

	void ~FireFighterAdapter();

	void respond(const std::string& location, Threat threat);

	void ~FireFighterAdapter();
};

#endif
