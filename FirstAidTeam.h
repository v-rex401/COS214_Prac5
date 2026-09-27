#ifndef FIRSTAIDTEAM_H
#define FIRSTAIDTEAM_H

#include <string>
#include "CommunicationTeam.h"

class FirstAidTeam : public FirstResponder
{

public:
	FirstAidTeam(CommunicationTeam *hub);

	void treatInjury();

	void checkInjury();

	void emergencyEscalation();

	void receive(const std::string &event);

	~FirstAidTeam();
};

#endif
