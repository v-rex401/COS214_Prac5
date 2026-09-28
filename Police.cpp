#include "Police.h"

#include <iostream>

std::string Police::confirmDeployment()
{
	return "Police have been deployed";
}

void Police::dispatch(const std::string& location, int severity, int incidentCode)
{
	std::cout << "Dispatching police to " << location << " with severity level " << severity << " and incident code " << incidentCode << "\n";
}
