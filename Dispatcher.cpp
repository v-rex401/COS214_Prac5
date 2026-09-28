#include "Dispatcher.h"
#include <iostream>

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

Dispatcher::~Dispatcher()
{
	while (!commandHistory.empty())
	{
		delete commandHistory.top();
		commandHistory.pop();
	}
}