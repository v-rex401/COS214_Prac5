#ifndef RESOLVE_H
#define RESOLVE_H

class Resolve : Protocol {

private:
	FacilityStaff* facility;
	AccessControlTeam* access;

public:
	void execute();

	void undo();

	Resolve(FacilityStaff* f, AccessControlTeam* a);

	void ~Resolve();
};

#endif
