#ifndef FIREFIGHTERADAPTER_H
#define FIREFIGHTERADAPTER_H

#include <string>

#include "EmergencyResponder.h"
#include "FireFighter.h"

class FireFighterAdapter : public EmergencyResponder {

private:
	FireFighter* adaptee;

public:
	FireFighterAdapter(FireFighter* f);

	void respond(const std::string& location, Threat threat) override;

	std::string getStatus() const override;

	~FireFighterAdapter();
};

#endif
