#ifndef POLICEADAPTER_H
#define POLICEADAPTER_H

class PoliceAdapter : EmergencyResponder {

public:
	Police* adaptee;

	PoliceAdapter(Police* p);

	virtual void respond(const std::string& location, Threat threat) = 0;

	virtual std::string getStatus() = 0;

	void ~PoliceAdapter();

	void respond(const std::string& location, Threat threat);

	void ~PoliceAdapter();
};

#endif
