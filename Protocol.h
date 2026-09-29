#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
class Protocol
{

public:
	virtual void execute() = 0;

	virtual void undo() = 0;

	virtual std::string getName() = 0;

	virtual ~Protocol() {};
};

#endif
