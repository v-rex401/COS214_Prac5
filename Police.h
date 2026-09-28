#ifndef POLICE_H
#define POLICE_H

#include <string>

class Police {


public:
	std::string confirmDeployment();

	void dispatch(const std::string& location, int severity, int incidentCode);
};

#endif
