#ifndef AMBULANCEADAPTER_H
#define AMBULANCEADAPTER_H

#include <string>

#include "EmergencyResponder.h"
#include "Ambulance.h"

class AmbulanceAdapter : public EmergencyResponder {

private:
	Ambulance* adaptee;

public:
	AmbulanceAdapter(Ambulance* a);

	void respond(const std::string& location, Threat threat) override;

	std::string getStatus() const override;

	~AmbulanceAdapter();
};

#endif
