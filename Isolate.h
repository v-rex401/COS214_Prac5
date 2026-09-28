#ifndef ISOLATE_H
#define ISOLATE_H

#include "Protocol.h"
#include <string>
#include "AccessControlTeam.h"

class Isolate : public Protocol
{

private:
	AccessControlTeam *access;
	std::string zone;

public:
	Isolate(AccessControlTeam *a, std::string zone);

	void execute();

	void undo();

	~Isolate();
};

#endif
