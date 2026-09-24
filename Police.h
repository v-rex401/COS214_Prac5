#ifndef POLICE_H
#define POLICE_H

class Police {


public:
	void dispatch(const std::string& location, int severity);

	std::string confirmDeployment();

	void dispatch(const std::string& location, int severity, int incidentCode);
};

#endif
