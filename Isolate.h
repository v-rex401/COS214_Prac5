#ifndef ISOLATE_H
#define ISOLATE_H

class Isolate : Protocol {

private:
	AccessControlTeam* access;
	std::string zone;

public:
	Isolate(AccessControlTeam* a, std::string zone);

	void execute();

	void undo();

	Isolate(AccessControlTeam* a, std::string zone);

	void ~Isolate();

	void ~Isolate();
};

#endif
