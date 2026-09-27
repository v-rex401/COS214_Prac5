#include "CommunicationHub.h"

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

void notify(FirstResponder *r, const std::string &event)
{
	r->receive(event);
}
