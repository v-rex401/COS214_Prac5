#include "Deescalate.h"

Deescalate::Deescalate(SecurityGuards *s)
{
	guards = s;
}

void Deescalate::execute()
{
	guards->changed("DEESCALATE");
}

void Deescalate::undo()
{
	guards->requestBackup();
}

std::string Deescalate::getName()
{
	return "DEESCALATE";
}

Deescalate::~Deescalate()
{
}
