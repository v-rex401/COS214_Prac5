#include "FirstResponder.h"

FirstResponder::FirstResponder(CommunicationTeam *hub)
{
	this->hub = hub;
	if (hub != nullptr)
		hub->registerResponder(this);
}

void FirstResponder::changed(const std::string &event)
{
	if (hub != nullptr)
		hub->notify(this, event);
}

FirstResponder::~FirstResponder()
{
}