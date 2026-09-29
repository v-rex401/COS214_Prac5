#ifndef ASSIST_H
#define ASSIST_H

#include "FirstAidTeam.h"
#include "AccessControlTeam.h"
#include "Protocol.h"

class Assist : public Protocol
{

private:
	FirstAidTeam *medics;
	AccessControlTeam *access;

public:
	Assist(FirstAidTeam *m, AccessControlTeam *a);

	void execute();

	void undo();

	std::string getName();

	~Assist();
};

#endif
