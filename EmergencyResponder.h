#ifndef EMERGENCYRESPONDER_H
#define EMERGENCYRESPONDER_H

#include <string>
#include "Threat.h"

class EmergencyResponder {


public:
	virtual void respond(const std::string& location, Threat threat) = 0;

	virtual std::string getStatus() const = 0;

	virtual ~EmergencyResponder() = default;
};

#endif
