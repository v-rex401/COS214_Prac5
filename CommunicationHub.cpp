#include "CommunicationHub.h"
#include "FirstResponder.h"

void CommunicationHub::removeResponder(FirstResponder *r)
{
	// Find the responder first
	for (std::size_t i = 0; i < responders.size(); i++)
	{
		if (responders[i] == r)
		{
			responders.erase(responders.begin() + i);

			return;
		}
	}
}

void CommunicationHub::registerResponder(FirstResponder *r)
{
	for(FirstResponder *ptr : responders)
	{
		if(r == ptr)
		{
			return; //responder already registered
		}
	}
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
	responders.clear();
}