#ifndef FIRSTAIDTEAM_H
#define FIRSTAIDTEAM_H

class FirstAidTeam : FirstResponder {


public:
	FirstAidTeam(CommunicationTeam* hub);

	virtual void receive(const std::string& event) = 0;

	void treatInjury();

	void assesInjury();

	void emergencyEscalation();

	void receive(const std::string& event);

	void ~FirstAidTeam();

	void ~FirstAidTeam();
};

#endif
