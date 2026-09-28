#include "FirstResponder.h"

FirstResponder::FirstResponder(CommunicationTeam *hub)
{
	this->hub = hub;
}

void FirstResponder::changed(const std::string &event)
{
	hub->notify(this, event);
}

FirstResponder::~FirstResponder()
{
	delete hub;
}