#ifndef POLICEADAPTER_H
#define POLICEADAPTER_H

#include <string>

#include "EmergencyResponder.h"
#include "Police.h"

class PoliceAdapter : public EmergencyResponder {

private:
	Police* adaptee;

public:
	PoliceAdapter(Police* p);

	void respond(const std::string& location, Threat threat) override;

	std::string getStatus() const override;

	~PoliceAdapter();
};

#endif
