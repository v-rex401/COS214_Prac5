#ifndef EMERGENCYRESPONDER_H
#define EMERGENCYRESPONDER_H

class EmergencyResponder {


public:
	virtual void respond(const std::string& location, Threat threat) = 0;

	virtual std::string getStatus() = 0;

	virtual void respond(const std::string& location, Threat threat) = 0;

	virtual void ~EmergencyResponder() = 0;

	virtual void ~EmergencyResponder() = 0;
};

#endif
