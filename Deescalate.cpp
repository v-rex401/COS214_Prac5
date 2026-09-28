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
	// TODO - implement Deescalate::undo
	throw "Not yet implemented";
}
Deescalate::~Deescalate()
{
	if (guards != nullptr)
		delete guards;
}
