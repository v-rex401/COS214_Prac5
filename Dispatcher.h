#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "Protocol.h"
#include <queue>
#include <stack>

class Dispatcher
{

protected:
	std::queue<Protocol *> commandQueue;
	std::stack<Protocol *> commandHistory;

public:
	void issueCommand(Protocol *cmd);

	void undoLast();

	std::stack<Protocol *> getCommandHistory();
	std::queue<Protocol *> getCommandQueue();

	~Dispatcher();
};

#endif
