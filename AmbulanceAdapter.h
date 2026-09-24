#ifndef AMBULANCEADAPTER_H
#define AMBULANCEADAPTER_H

class AmbulanceAdapter : EmergencyResponder {

public:
	Ambulance* adaptee;

	AmbulanceAdapter(Ambulance* a);

	virtual void respond(const std::string& location, Threat threat) = 0;

	virtual std::string getStatus() = 0;

	void ~AmbulanceAdapter();

	void respond(const std::string& location, Threat threat);

	void ~AmbulanceAdapter();
};

#endif
