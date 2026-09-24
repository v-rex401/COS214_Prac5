#ifndef EVACUATE_H
#define EVACUATE_H

class Evacuate : Protocol {

private:
	SecurityGuards* guards;
	FacilityStaff* facility;
	AccessControlTeam* access;

public:
	Evacuate(SecurityGuards* g, FacilityStaff* f, AccessControlTeam* a);

	void execute();

	void undo();

	void ~Evacuate();

	void ~Evacuate();
};

#endif
