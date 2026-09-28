#include "Dispatcher.h"
#include <iostream>

void Dispatcher::issueCommand(Protocol *cmd)
{
	if (cmd != nullptr)
	{
		cmd->execute();
	}
	else
	{
		std::cout << "No command issued";
	}
}

void Dispatcher::undoLast()
{
	// TODO - implement Dispatcher::undoLast
	throw "Not yet implemented";
}
