#include "CommunicationHub.h"
#include "FirstResponder.h"

void CommunicationHub::removeResponder(FirstResponder *r)
{
	// Find the responder first
	for (int i = 0; i < responders.size(); i++)
	{
		if (responders[i] != nullptr)
		{
			if (r == responders[i])
			{
				responders.erase(responders.begin() + i);
			}
		}
	}
}

void CommunicationHub::registerResponder(FirstResponder *r)
{
	responders.push_back(r);
}

void CommunicationHub::notify(FirstResponder *r, const std::string &event)
{
	// Notify other responders of change
	for (FirstResponder *ptr : responders)
	{
		if (r != ptr)
		{
			ptr->receive(event);
		}
	}
}

CommunicationHub::~CommunicationHub()
{
	for (FirstResponder *ptr : responders)
	{
		if (ptr != nullptr)
		{
			delete ptr;
		}
	}
}