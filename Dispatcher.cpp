#include "Dispatcher.h"
#include <iostream>
#include <stack>
#include <queue>

void Dispatcher::issueCommand(Protocol *cmd)
{
	if (cmd != nullptr)
	{
		cmd->execute();
		commandHistory.push(cmd);
	}
	else
	{
		std::cout << "No command issued";
	}
}

void Dispatcher::undoLast()
{
	if (commandHistory.empty())
	{
		return;
	}
	Protocol *prevCmd = commandHistory.top();
	commandHistory.pop();
	prevCmd->undo();
	delete prevCmd;
}

std::stack<Protocol *> Dispatcher::getCommandHistory()
{
	return commandHistory;
}

std::queue<Protocol *> Dispatcher::getCommandQueue()
{
	return commandQueue;
}

Dispatcher::~Dispatcher()
{
	while (!commandHistory.empty())
	{
		delete commandHistory.top();
		commandHistory.pop();
	}
}