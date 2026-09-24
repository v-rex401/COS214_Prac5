#ifndef PROTOCOL_H
#define PROTOCOL_H

class Protocol {


public:
	virtual void execute() = 0;

	virtual void undo() = 0;


	virtual ~Protocol();
};

#endif
