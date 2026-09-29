#include <iostream>
#include <sstream>
#include <string>

#include "CommunicationHub.h"
#include "SecurityGuards.h"
#include "AccessControlTeam.h"
#include "FacilityStaff.h"
#include "FirstAidTeam.h"
#include "Dispatcher.h"
#include "Evacuate.h"
#include "Assist.h"
#include "Deescalate.h"
#include "PoliceAdapter.h"
#include "AmbulanceAdapter.h"
#include "Police.h"
#include "Ambulance.h"
#include "IncidentControl.h"
#include "EmergencyResponseFacade.h"

void scenario1();
void scenario2();

int main()
{
    scenario1();
    scenario2();
    return 0;
}

void scenario1()
{
    std::cout << "==== Scenario 1 ====\n";
    // Response Object to interact with
    EmergencyResponseFacade *highschool = new EmergencyResponseFacade();

    // The threat at the school
    enum Threat chemicalLabFire = FIRE;

    std::cout << "==== Report Incident ====\n";
    // Report the incident
    highschool->reportIncident("Chemistry Lab", chemicalLabFire);
    std::cout << std::endl;

    std::cout << "==== Adding A Threat ====\n";
    // New threat at school
    enum Threat medicalEmergency = MEDICAL_EMERGENCY;
    highschool->addThreat("Chemical Lab", medicalEmergency);
    std::cout << std::endl;

    std::cout << "==== Escalate  ====\n";
    highschool->escalateToEmergency("Chemistry Lab", FIRE);
    std::cout << std::endl;

    std::cout << "==== Resolve Incident ====\n";
    highschool->resolveIncident();
    std::cout << std::endl;

    std::cout << "==== Issue Not Resolved - ROLLBACK ====\n";
    highschool->rollbackLastAction();
    std::cout << std::endl;

    highschool->printHistory();
    std::cout << std::endl;

    delete highschool;
}

void scenario2()
{
    std::cout << "==== Scenario 2 ====\n";
    // Response Object to interact with
    EmergencyResponseFacade *campus = new EmergencyResponseFacade();

    // The threat on campus
    enum Threat libraryInjury = INJURY;

    std::cout << "==== Report Incident ====\n";
    // Report the incident
    campus->reportIncident("Library", libraryInjury);
    std::cout << std::endl;

    std::cout << "==== Adding A Threat ====\n";
    // New threat on campus
    enum Threat auditoriumFight = FIGHT;
    campus->addThreat("Auditorium", auditoriumFight);
    std::cout << std::endl;

    std::cout << "==== Rollback  ====\n";
    campus->rollbackLastAction();
    std::cout << std::endl;

    std::cout << "==== Escalate ====\n";
    campus->escalateToEmergency("Library", INJURY);
    std::cout << std::endl;
    campus->escalateToEmergency("Library", INJURY);  //should show that it cannot escalate further
    std::cout << std::endl;

    std::cout << "==== Rollback ====\n";
    campus->rollbackLastAction();  //should rollback escalation
    std::cout << std::endl;

    std::cout << "==== Resolve ====\n";
    campus->resolveIncident();  //should show that there are 0 threats
    std::cout << std::endl;

    std::cout << "==== Adding A Threat ====\n";
    // New threat on campus
    enum Threat cafeteriaDamaged = DAMAGED;
    campus->addThreat("Cafeteria", cafeteriaDamaged);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "==== Rollback ====\n";
    campus->rollbackLastAction();
    std::cout << std::endl;
    campus->rollbackLastAction();

    delete campus;
}