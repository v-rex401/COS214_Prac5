#ifndef DISPATCHER_H
#define DISPATCHER_H

class Dispatcher {

private:
	std::queue<Protocol*> commandQueue;
	std::stack<Protocol*> commandHistory;

public:
	void issueCommand(Protocol* cmd);

	void undoLast();

	void ~Dispatcher();

	void ~Dispatcher();
};

#endif
