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
    return 0;
}

void scenario1()
{
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
    highschool->escalateToEmergency();
    std::cout << std::endl;

    std::cout << "==== Resolve Incident ====\n";
    highschool->resolveIncident();
    std::cout << std::endl;

    std::cout << "==== Issue Not Resolved - ROLLBACK ====\n";
    highschool->rollbackLastAction();
    std::cout << std::endl;

    highschool->printHistory();

    delete highschool;
}

void scneario2()
{
}