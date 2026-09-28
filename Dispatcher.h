#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "Protocol.h"
#include <queue>
#include <stack>

class Dispatcher
{

private:
	std::queue<Protocol *> commandQueue;
	std::stack<Protocol *> commandHistory;

public:
	void issueCommand(Protocol *cmd);

	void undoLast();

	~Dispatcher();
};

#endif
