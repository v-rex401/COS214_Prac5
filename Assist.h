#ifndef ASSIST_H
#define ASSIST_H

class Assist : Protocol {

private:
	FirstAidTeam* medics;
	AccessControlTeam* access;

public:
	Assist(FirstAidTeam* m, AccessControlTeam* a);

	void execute();

	void undo();

	void ~Assist();

	void ~Assist();
};

#endif
