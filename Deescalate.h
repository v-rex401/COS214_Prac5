#ifndef DEESCALATE_H
#define DEESCALATE_H

class Deescalate : Protocol {

public:
	SecurityGuards* guards;

	void Deescalate(SecurityGuards* s);

	void execute();

	void undo();

	void ~Deescalate();

	void ~Deescalate();
};

#endif
