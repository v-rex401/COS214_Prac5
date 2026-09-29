TARGET = campusguard

# ResponderComponent, Iterator and UnitState have no matching .cpp -
# they're abstract/header-only, so there's no .o to build for them.
OBJS = main.o \
AccessControlTeam.o Ambulance.o AmbulanceAdapter.o Assist.o \
CommunicationHub.o Deescalate.o Dispatcher.o Emergency.o \
Evacuate.o FacilityStaff.o FireFighter.o \
FireFighterAdapter.o FirstAidTeam.o FirstResponder.o GrantAccess.o \
IncidentControl.o IncidentHistory.o IncidentMemento.o Isolate.o \
Moderate.o Police.o PoliceAdapter.o Resolve.o Resolved.o \
SecurityGuards.o Urgent.o EmergencyResponseFacade.o

$(TARGET): $(OBJS)
	g++ -std=c++11 -g -o $(TARGET) $(OBJS)

main.o: main.cpp
	g++ -std=c++11 -g -c main.cpp

EmergEmergencyResponseFacade.o:EmergencyResponseFacade.cpp
	g++ -std=c++11 -g -cEmergencyResponseFacade.cpp

AccessControlTeam.o: AccessControlTeam.cpp
	g++ -std=c++11 -g -c AccessControlTeam.cpp

Ambulance.o: Ambulance.cpp
	g++ -std=c++11 -g -c Ambulance.cpp

AmbulanceAdapter.o: AmbulanceAdapter.cpp
	g++ -std=c++11 -g -c AmbulanceAdapter.cpp

Assist.o: Assist.cpp
	g++ -std=c++11 -g -c Assist.cpp

CommunicationHub.o: CommunicationHub.cpp
	g++ -std=c++11 -g -c CommunicationHub.cpp

Deescalate.o: Deescalate.cpp
	g++ -std=c++11 -g -c Deescalate.cpp

Dispatcher.o: Dispatcher.cpp
	g++ -std=c++11 -g -c Dispatcher.cpp

Emergency.o: Emergency.cpp
	g++ -std=c++11 -g -c Emergency.cpp

Evacuate.o: Evacuate.cpp
	g++ -std=c++11 -g -c Evacuate.cpp

FacilityStaff.o: FacilityStaff.cpp
	g++ -std=c++11 -g -c FacilityStaff.cpp

FireFighter.o: FireFighter.cpp
	g++ -std=c++11 -g -c FireFighter.cpp

FireFighterAdapter.o: FireFighterAdapter.cpp
	g++ -std=c++11 -g -c FireFighterAdapter.cpp

FirstAidTeam.o: FirstAidTeam.cpp
	g++ -std=c++11 -g -c FirstAidTeam.cpp

FirstResponder.o: FirstResponder.cpp
	g++ -std=c++11 -g -c FirstResponder.cpp

GrantAccess.o: GrantAccess.cpp
	g++ -std=c++11 -g -c GrantAccess.cpp

IncidentControl.o: IncidentControl.cpp
	g++ -std=c++11 -g -c IncidentControl.cpp

IncidentHistory.o: IncidentHistory.cpp
	g++ -std=c++11 -g -c IncidentHistory.cpp

IncidentMemento.o: IncidentMemento.cpp
	g++ -std=c++11 -g -c IncidentMemento.cpp

Isolate.o: Isolate.cpp
	g++ -std=c++11 -g -c Isolate.cpp

Moderate.o: Moderate.cpp
	g++ -std=c++11 -g -c Moderate.cpp

Police.o: Police.cpp
	g++ -std=c++11 -g -c Police.cpp

PoliceAdapter.o: PoliceAdapter.cpp
	g++ -std=c++11 -g -c PoliceAdapter.cpp

Resolve.o: Resolve.cpp
	g++ -std=c++11 -g -c Resolve.cpp

Resolved.o: Resolved.cpp
	g++ -std=c++11 -g -c Resolved.cpp

SecurityGuards.o: SecurityGuards.cpp
	g++ -std=c++11 -g -c SecurityGuards.cpp

Urgent.o: Urgent.cpp
	g++ -std=c++11 -g -c Urgent.cpp

clean:
	rm -f *.o $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

run: $(TARGET)
	./$(TARGET)