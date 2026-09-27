# CampusGuard 2026   Design Document

**Team Members:**
- Lindo Skosana
- Kayla Falconer
- Vashti

---

# SYSTEM OVERVIEW

CampusGuard 2026 is an automated emergency response management system designed for university campuses. It monitors campus security threats, manages incident escalation lifecycles, coordinates internal responder teams, and dispatches external emergency services. The system handles the entire incident lifecycle from initial threat detection and zone isolation to full emergency escalation, de-escalation, resolution, and rollback. It coordinates subsystem operations across six Gang of Four (GoF) design patterns: Command, Mediator, Adapter, State, Memento, and Facade.

---

# PATTERN 1: COMMAND

## PARTICIPANTS

- Command (interface) = `Protocol` 
- Invoker = `Dispatcher` 
- ConcreteCommands = `Evacuate`, `Deescalate`, `EmergencyEscalation`, `Resolve`, `Assist`, `GrantAccess`, `Isolate` 
- Receivers = `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`, `EmergencyResponder` 

## WHY THIS PATTERN

The Command pattern decouples the entity requesting an action (`EmergencyResponseFacade` or `Dispatcher`) from the receiver entities that execute physical responses on campus (`SecurityGuards`, `FacilityStaff`, `AccessControlTeam`). By encapsulating every physical response protocol into a discrete object implementing the `Protocol` interface, CampusGuard can queue pending actions, log executed commands in sequential history order, and support rollback operations via `undo()` calls.

## `Protocol`

**Role:** Command interface   all concrete commands inherit from this .

| Method | virtual? | Returns | Purpose |
|---|---|---|---|
| `execute()` | pure virtual | void | Executes the concrete command operation on receiver entities . |
| `undo()` | pure virtual | void | Reverses the effects of the executed command on receiver entities . |
| `~Protocol()` | virtual (not pure) | void | Ensures safe polymorphic destruction of concrete command objects . |

Implementation Notes: Base abstract class for all action objects. It declares pure virtual methods so that `Dispatcher` can invoke `execute()` and `undo()` polymorphically .

---

## `Dispatcher`

**Role:** Invoker   holds and fires commands .

**Attributes:**

| Name | Type | Access | Purpose |
|---|---|---|---|
| `commandQueue` | `std::queue<Protocol*>` | private | Stores pending commands scheduled for execution . |
| `commandHistory` | `std::stack<Protocol*>` | private | Stores executed commands to enable sequential undo operations . |

**Owns:** All `Protocol*` pointers in both containers .

| Method | Purpose | Steps |
|---|---|---|
| `issueCommand(cmd : Protocol*)` | Executes a command immediately and logs it into history . | 1. Call `cmd->execute()` . <br> 2. Push `cmd` onto `commandHistory` . <br> 3. Log command execution output . |
| `undoLast()` | Reverses and deletes the most recently executed command . | 1. Check if `commandHistory` is empty; if empty, handle invalid case . <br> 2. Pop top command `cmd` from `commandHistory` . <br> 3. Call `cmd->undo()` and delete `cmd` . |
| `~Dispatcher()` | Cleans up allocated commands . | Iterates through remaining items in `commandQueue` and `commandHistory` and deletes each `Protocol*` pointer . |

Invalid operation case handled here: Calling `undoLast()` when `commandHistory` is empty logs a warning message ("Warning: No commands available to undo") and returns safely without throwing an exception or segfaulting .

---

## `Evacuate`

**Receivers:** `SecurityGuards*`, `FacilityStaff*`, `AccessControlTeam*` 

**Why these receivers:** Evacuating a building requires security personnel to herd occupants, facility staff to verify open physical exits, and access control to unlock all automated perimeter gates .

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `guards` | `SecurityGuards*` | No  |
| `facility` | `FacilityStaff*` | No  |
| `access` | `AccessControlTeam*` | No  |

**Constructor:** `Evacuate(g : SecurityGuards*, f : FacilityStaff*, a : AccessControlTeam*)`

Constructor stores receiver pointers into private attributes for later execution .

**`execute()` steps:**
1. Call `guards->clearBuilding()` .
2. Call `facility->securePremises()` .
3. Call `access->grantEmergencyAccess()` .

**`undo()` steps:**
1. Call `access->revokeAccess("ALL", "EVACUEE")` .
2. Call `guards->issueWarning()` .

**`~Evacuate()`:**
Does it delete any receivers? No. Receivers are owned by `EmergencyResponseFacade` and shared across multiple commands .

---

## `Deescalate`

**Receivers:** `SecurityGuards*` 

**Why these receivers:** Standing down active security patrols directly involves security personnel .

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `guards` | `SecurityGuards*` | No  |

**Constructor:** `Deescalate(s : SecurityGuards*)` initializes `guards` .

**`execute()` steps:**
1. Call `guards->issueWarning()` with stand-down advisory .
2. Reduce security alert status .

**`undo()` steps:**
1. Call `guards->requestBackup()` .

**`~Deescalate()`:** Does not delete `guards` pointer .

---

## `EmergencyEscalation`

**Receivers:** `SecurityGuards*`, `FacilityStaff*`, `AccessControlTeam*`, `FirstAidTeam*`, `EmergencyResponder*` (police, ambulance, fire) 

**Why these receivers:** A campus-wide emergency requires active coordination across all internal response teams and external municipal emergency agencies .

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `guards` | `SecurityGuards*` | No  |
| `facility` | `FacilityStaff*` | No  |
| `access` | `AccessControlTeam*` | No  |
| `medics` | `FirstAidTeam*` | No  |
| `police` | `EmergencyResponder*` | No  |
| `ambulance` | `EmergencyResponder*` | No  |
| `fire` | `EmergencyResponder*` | No  |

**Constructor:** `EmergencyEscalation(s, f, a, m, p, fire, am)` initializes all seven receiver pointers .

**`execute()` steps:**
1. Call `guards->clearBuilding()` .
2. Call `facility->securePremises()` .
3. Call `access->grantEmergencyAccess()` .
4. Call `medics->emergencyEscalation()` .
5. Call `police->respond("CAMPUS", Threat::SHOOTING)` .
6. Call `ambulance->respond("CAMPUS", Threat::MEDICAL_EMERGENCY)` .
7. Call `fire->respond("CAMPUS", Threat::FIRE)` .

**`undo()` steps:**
1. Call `access->revokeAccess("ALL", "EMERGENCY")` .
2. Log cancellation notification for external services .

**`~EmergencyEscalation()`:** Does not delete receiver pointers .

---

## `Resolve`

**Receivers:** `FacilityStaff*`, `AccessControlTeam*` 

**Why these receivers:** Resolving an incident requires facilities to inspect infrastructure and access control to restore standard security permissions .

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `facility` | `FacilityStaff*` | No  |
| `access` | `AccessControlTeam*` | No  |

**Constructor:** `Resolve(f : FacilityStaff*, a : AccessControlTeam*)` initializes `facility` and `access` .

**`execute()` steps:**
1. Call `facility->dispatchMaintanance()` .
2. Call `access->unlockZone("ALL")` .
3. Log incident resolution status .

**`undo()` steps:**
1. Call `access->lockdownZone("ALL")` .

**`~Resolve()`:** Does not delete receiver pointers .

---

## `Assist`

**Receivers:** `FirstAidTeam*`, `AccessControlTeam*` 

**Why these receivers:** Assisting injured individuals requires medical staff to administer care and access control to clear physical access paths for medical vehicles .

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `medics` | `FirstAidTeam*` | No  |
| `access` | `AccessControlTeam*` | No  |

**Constructor:** `Assist(m : FirstAidTeam*, a : AccessControlTeam*)` initializes attributes .

**`execute()` steps:**
1. Call `medics->assesInjury()` .
2. Call `medics->treatInjury()` .
3. Call `access->grantEmergencyAccess()` .

**`undo()` steps:**
1. Log rollback of assistance protocol .

**`~Assist()`:** Does not delete receiver pointers .

---

## `GrantAccess`

**Receivers:** `AccessControlTeam*` 

**Why these receivers:** Directly manipulates access control entry permissions for specific campus sectors .

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `access` | `AccessControlTeam*` | No  |
| `zone` | `std::string` | Yes (value copy)  |
| `role` | `std::string` | Yes (value copy)  |

**Constructor:** `GrantAccess(a : AccessControlTeam*, zone : std::string, role : std::string)` initializes receiver, zone, and role .

**`execute()` steps:**
1. Call `access->grantEmergencyAccess()` or specific zone unlock logic .

**`undo()` steps:**
1. Call `access->revokeAccess(zone, role)` .

**`~GrantAccess()`:** Does not delete `access` pointer .

---

## `Isolate`

**Receivers:** `AccessControlTeam*` 

**Why these receivers:** Isolating a security threat requires automated locking of sector access points .

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `access` | `AccessControlTeam*` | No  |
| `zone` | `std::string` | Yes (value copy)  |

**Constructor:** `Isolate(a : AccessControlTeam*, zone : std::string)` initializes `access` and `zone` .

**`execute()` steps:**
1. Call `access->lockdownZone(zone)` .
2. Log zone isolation status .

Inside `lockdownZone()`, `AccessControlTeam` calls `changed("ZONE_LOCKED")`   this is where Command triggers the Mediator .

**`undo()` steps:**
1. Call `access->unlockZone(zone)` .

**`~Isolate()`:** Does not delete `access` pointer .

---

# PATTERN 2: MEDIATOR

## PARTICIPANTS

- Mediator (interface) = `CommunicationTeam` 
- ConcreteMediator = `CommunicationHub` 
- Colleague (abstract) = `FirstResponder` 
- ConcreteColleagues = `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam` 

## WHY THIS PATTERN

The Mediator pattern eliminates direct, many-to-many dependencies between internal responder teams . Without a mediator, `SecurityGuards` would require direct pointers to `FacilityStaff`, `AccessControlTeam`, and `FirstAidTeam` to alert them of changes, creating tight coupling and potential circular dependencies . `CommunicationHub` centralizes all event notifications so responder classes remain decoupled .

## HOW IT WORKS

When an internal responder experiences a state change or executes an action (e.g., `AccessControlTeam` locking a zone), it calls `this->changed("ZONE_LOCKED")` . The base class `FirstResponder::changed()` forwards this event string to `hub->notify(this, "ZONE_LOCKED")` . The `CommunicationHub` iterates through its list of registered responders, skips the sender instance, and invokes `receive("ZONE_LOCKED")` on every other responder so they can react independently .

---

## `CommunicationTeam`

**Role:** Abstract mediator interface .

| Method | virtual? | Returns | Purpose |
|---|---|---|---|
| `notify(r : FirstResponder*, event : const std::string&)` | pure virtual | void | Defines notification protocol for colleague state changes . |
| `~CommunicationTeam()` | pure virtual | void | Ensures safe polymorphic cleanup of mediator objects . |

Pure virtual destructor needs an out-of-line implementation body in `CommunicationTeam.cpp`: `CommunicationTeam::~CommunicationTeam() {}` .

---

## `CommunicationHub`

**Role:** ConcreteMediator   coordinates all colleague communication .

**Attributes:**

| Name | Type | Owned? | Purpose |
|---|---|---|---|
| `responders` | `std::vector<FirstResponder*>` | No | Holds non-owning references to registered colleagues . |

| Method | virtual? | Purpose | Steps |
|---|---|---|---|
| `notify(r : FirstResponder*, event : const std::string)` | override | Broadcasts event to all colleagues except sender . | 1. Iterate through `responders` vector . <br> 2. Skip element if `elem == r` . <br> 3. Call `elem->receive(event)` . |
| `registerResponder(r : FirstResponder*)` | no | Adds colleague to broadcast list . | Appends `r` to `responders` vector . |
| `removeResponder(r : FirstResponder*)` | no | Removes colleague from list . | Erases `r` from `responders` vector . |
| `~CommunicationHub()` | override | Destructor . | Clears `responders` vector without deleting pointers . |

`~CommunicationHub()` does NOT delete anything in `responders` because responder objects are owned by `EmergencyResponseFacade` .

---

## `FirstResponder`

**Role:** Abstract colleague base class .

**Attributes:**

| Name | Type | Access | Owned? | Purpose |
|---|---|---|---|---|
| `hub` | `CommunicationTeam*` | protected | No | Pointer to mediator hub . |

| Method | virtual? | Purpose |
|---|---|---|
| `FirstResponder(hub : CommunicationTeam*)` |   | Constructs colleague with mediator pointer . |
| `receive(event : const std::string&)` | pure virtual | Handles incoming broadcast events from other colleagues . |
| `changed(event : const std::string&)` | NOT virtual | Forwards internal events to the mediator hub . |
| `~FirstResponder()` | pure virtual | Pure virtual destructor requiring implementation in `.cpp` . |

`changed()` is NOT virtual because forwarding logic to `hub->notify(this, event)` is invariant across all colleague types . `receive()` is pure virtual because each concrete colleague reacts differently to broadcast events .

---

## `SecurityGuards`

**Constructor:** `SecurityGuards(hub : CommunicationTeam*) : FirstResponder(hub) {}` 

| Method | Purpose |
|---|---|
| `receive(event : const std::string&)` | Parses event string and triggers security responses . |
| `clearBuilding()` | Orders building evacuation . |
| `issueWarning()` | Dispatches warning broadcasts . |
| `requestBackup()` | Requests additional security units . |
| `~SecurityGuards()` | Destructor . |

**`receive()`   event handling:**

| Event string | Reaction |
|---|---|
| `"INCIDENT_REPORTED"` | Logs security awareness and increases patrol vigilance . |
| `"EMERGENCY_DECLARED"` | Triggers immediate perimeter security lockdown . |
| `"INCIDENT_RESOLVED"` | Stands down active tactical teams . |
| `"ZONE_LOCKED"` | Deploys guards to monitor locked zone boundaries . |

**`clearBuilding()`:** Prints building evacuation message and calls `changed("BUILDING_CLEARED")` .

**`requestBackup()`:** Calls `changed("BACKUP_NEEDED")` .

---

## `FirstAidTeam`

**Constructor:** `FirstAidTeam(hub : CommunicationTeam*) : FirstResponder(hub) {}` 

| Method | Purpose |
|---|---|
| `receive(event : const std::string&)` | Parses event string and triggers medical responses . |
| `treatInjury()` | Administers medical treatment . |
| `assesInjury()` | Evaluates casualty severity . |
| `emergencyEscalation()` | Prepares medical triage stations . |
| `~FirstAidTeam()` | Destructor . |

**`receive()`   event handling:**

| Event string | Reaction |
|---|---|
| `"INCIDENT_REPORTED"` | Prepares medical equipment and triage kits . |
| `"BACKUP_NEEDED"` | Dispatches medical personnel to requested sector . |
| `"EMERGENCY_DECLARED"` | Prepares field trauma unit for incoming casualties . |

`emergencyEscalation()` does NOT call external municipal ambulances directly. Instead, it prepares internal field stations and calls `changed("MEDICAL_ESCALATED")` so the system facade can handle external adapters .

---

## `FacilityStaff`

**Constructor:** `FacilityStaff(hub : CommunicationTeam*) : FirstResponder(hub) {}` 

| Method | Purpose |
|---|---|
| `receive(event : const std::string&)` | Parses event string and triggers facility operations . |
| `dispatchMaintanance()` | Sends maintenance crews to repair infrastructure . |
| `securePremises()` | Secures building utility lines and emergency doors . |
| `~FacilityStaff()` | Destructor . |

Spelling Note: Method is named `dispatchMaintanance` to match UML specifications .

**`receive()`   event handling:**

| Event string | Reaction |
|---|---|
| `"BUILDING_CLEARED"` | Dispatches facility staff to lock down utility infrastructure . |
| `"INCIDENT_RESOLVED"` | Calls `dispatchMaintanance()` to inspect structural damage . |
| `"ZONE_LOCKED"` | Secures secondary utility access points in locked sector . |

---

## `AccessControlTeam`

**Constructor:** `AccessControlTeam(hub : CommunicationTeam*) : FirstResponder(hub) {}` 

| Method | Purpose |
|---|---|
| `receive(event : const std::string&)` | Parses event string and triggers access door locks . |
| `unlockZone(zone : const std::string&)` | Unlocks electronic access doors in specified zone . |
| `lockdownZone(zone : const std::string&)` | Locks all electronic doors in specified zone . |
| `grantEmergencyAccess()` | Overrides electronic access doors campus-wide . |
| `revokeAccess(zone, role)` | Restores restrictive access permissions . |
| `broadcastRestriction(zone)` | Dispatches entry warning alerts . |
| `~AccessControlTeam()` | Destructor . |

**`receive()`   event handling:**

| Event string | Reaction |
|---|---|
| `"EMERGENCY_DECLARED"` | Unlocks all evacuation turnstiles and emergency exits . |
| `"INCIDENT_RESOLVED"` | Restores normal card-access security rules . |

`lockdownZone()` locks electronic doors and calls `changed("ZONE_LOCKED")` to trigger mediator notifications .

`broadcastRestriction()` calls `changed("RESTRICTION_ENFORCED")` .

---

# PATTERN 3: ADAPTER

## PARTICIPANTS

- Target (interface) = `EmergencyResponder` 
- Adapters = `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter` 
- Adaptees = `Ambulance`, `Police`, `FireFighter` 
- Client = `EmergencyEscalation` command 

## WHY THIS PATTERN

The Adapter pattern converts the incompatible legacy interfaces of external emergency response services (`Ambulance`, `Police`, `FireFighter`) into the unified target interface (`EmergencyResponder`) expected by CampusGuard . This allows CampusGuard to dispatch external services uniformly using `respond(location, threat)` without modifying legacy vendor code .

## THE MISMATCH

| Service | CampusGuard calls | Adaptee provides | What the adapter must translate |
|---|---|---|---|
| Ambulance | `respond(location, threat)` | `dispatch(caseType, lat, lon)` | Translates text location to `lat`/`lon` coordinates and maps `Threat` enum to integer `caseType` . |
| Police | `respond(location, threat)` | `dispatch(location, severity, incidentCode)` | Translates `Threat` enum to numeric severity integer and police radio code string . |
| FireFighter | `respond(location, threat)` | `alertStation(location, buildingNumber)` | Translates text location to integer `buildingNumber` and retrieves ETA metric . |

---

## `EmergencyResponder`

**Role:** Target interface   unified interface expected by CampusGuard .

| Method | virtual? | Returns | Purpose |
|---|---|---|---|
| `respond(location : const std::string&, threat : Threat)` | pure virtual | void | Dispatches responder to target location for specified threat . |
| `getStatus()` | pure virtual | void | Queries operational deployment status of the responder . |
| `~EmergencyResponder()` | pure virtual | void | Pure virtual destructor requiring body in `.cpp` . |

---

## `Ambulance`

**Role:** Adaptee   external service with incompatible interface .

| Method | Returns | Implementation notes |
|---|---|---|
| `dispatch(caseType : int, lat : double, lon : double)` | void | Accepts integer medical case codes and GPS double coordinates . |
| `getUnitAvailability()` | boolean | Returns availability status (simulated boolean) . |

Latitude and longitude are determined by internal lookup logic inside `AmbulanceAdapter`, not inside `Ambulance` . Availability defaults to `true` for demonstration .

---

## `AmbulanceAdapter`

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `adaptee` | `Ambulance*` | YES   deleted in adapter destructor . |

**Constructor:** `AmbulanceAdapter(a : Ambulance*) : adaptee(a) {}` 

**`~AmbulanceAdapter()`:** Calls `delete adaptee;` because the adapter owns its wrapped adaptee instance .

**`respond(location : const std::string&, threat : Threat)`   translation steps:**

Step 1   Convert location string → `lat`/`lon`:
```cpp
double lat = -25.7545, lon = 28.2314; // Default campus coordinates
if (location.find("Engineering") != std::string::npos) { lat = -25.7550; lon = 28.2320; }
else if (location.find("Library") != std::string::npos) { lat = -25.7538; lon = 28.2295; }
```
 

Step 2   Convert `Threat` enum → `caseType` integer:
```cpp
int caseType = 1; // Default medical
if (threat == Threat::SHOOTING) caseType = 5;
else if (threat == Threat::INJURY) caseType = 2;
else if (threat == Threat::MEDICAL_EMERGENCY) caseType = 1;
```
 

Step 3   Call adaptee:
```cpp
adaptee->dispatch(caseType, lat, lon);
``` 

**`getStatus()`:** Checks `adaptee->getUnitAvailability()` and logs unit deployment status .

---

## `Police`

**Role:** Adaptee   external service with incompatible interface .

| Method | Returns | Implementation notes |
|---|---|---|
| `dispatch(location : const std::string, severity : int, incidentCode : std::string)` | void | Accepts text location, integer severity level, and string incident radio code . |
| `confirmDeployment()` | void | Prints deployment confirmation text . |

---

## `PoliceAdapter`

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `adaptee` | `Police*` | YES   deleted in destructor . |

**Constructor:** `PoliceAdapter(p : Police*) : adaptee(p) {}` 

**`~PoliceAdapter()`:** Calls `delete adaptee;` 

**`respond(location : const std::string&, threat : Threat)`   translation steps:**

Step 1   Convert `Threat` → `severity` (int) and `incidentCode` (string):
```cpp
int severity = 1; std::string code = "10-10";
if (threat == Threat::SHOOTING) { severity = 5; code = "10-71"; }
else if (threat == Threat::FIGHT) { severity = 3; code = "10-15"; }
else if (threat == Threat::FIRE) { severity = 4; code = "10-70"; }
``` 

Step 2   Call adaptee:
```cpp
adaptee->dispatch(location, severity, code);
``` 

**`getStatus()`:** Calls `adaptee->confirmDeployment()` .

---

## `FireFighter`

**Role:** Adaptee   external service with incompatible interface .

| Method | Returns | Implementation notes |
|---|---|---|
| `alertStation(location : const std::string&, buildingNumber : int)` | void | Accepts location string and internal integer building identifier . |
| `getResponseETA()` | int | Returns estimated response time in minutes . |

`buildingNumber` is calculated by the adapter's location mapping logic . `getResponseETA()` returns a hardcoded/simulated integer (e.g., 5 minutes) .

---

## `FireFighterAdapter`

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `adaptee` | `FireFighter*` | YES   deleted in destructor . |

**Constructor:** `FireFighterAdapter(f : FireFighter*) : adaptee(f) {}` 

**`~FireFighterAdapter()`:** Calls `delete adaptee;` 

**`respond(location : const std::string&, threat : Threat)`   translation steps:**

Step 1   Convert location → `buildingNumber`:
```cpp
int bNum = 100;
if (location.find("Engineering") != std::string::npos) bNum = 101;
else if (location.find("Library") != std::string::npos) bNum = 202;
``` 

Step 2   Call adaptee:
```cpp
adaptee->alertStation(location, bNum);
``` 

Step 3   Retrieve and display ETA:
```cpp
int eta = adaptee->getResponseETA();