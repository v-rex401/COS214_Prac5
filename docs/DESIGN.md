# CampusGuard 2026 Design Document

## Team Members
- Lindo Skosana
- Kayla Falconer
- Vashti Pillay

---

## System Overview

**CampusGuard 2026** is an automated emergency response management system designed for university campuses. It monitors campus security threats, manages incident escalation lifecycles, coordinates internal responder teams, and dispatches external emergency services.

The system handles the entire incident lifecycle from initial threat detection and zone isolation to full emergency escalation, de-escalation, resolution, and rollback. It coordinates subsystem operations across six Gang of Four (GoF) design patterns:

1. **Command** — Encapsulate and execute emergency protocols
2. **Mediator** — Decouple responder team communication
3. **Adapter** — Integrate legacy external emergency services
4. **State** — Manage incident lifecycle transitions
5. **Memento** — Capture and restore system state snapshots
6. **Facade** — Provide unified high-level interface

---

## Pattern 1: Command

### Participants

| Role | Implementation |
|------|-----------------|
| **Command (interface)** | `Protocol` |
| **Invoker** | `Dispatcher` |
| **ConcreteCommands** | `Evacuate`, `Deescalate`, `EmergencyEscalation`, `Resolve`, `Assist`, `GrantAccess`, `Isolate` |
| **Receivers** | `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`, `EmergencyResponder` |

### Why This Pattern

The Command pattern decouples the entity requesting an action (EmergencyResponseFacade or Dispatcher) from the receiver entities that execute physical responses on campus (SecurityGuards, FacilityStaff, AccessControlTeam).

By encapsulating every physical response protocol into a discrete object implementing the `Protocol` interface, CampusGuard can:
- Queue pending actions
- Log executed commands in sequential history order
- Support rollback operations via `undo()` calls

---

### Protocol (Command Interface)

**Role:** Base abstract class for all action objects

| Method | Virtual? | Returns | Purpose |
|--------|----------|---------|---------|
| `execute()` | pure virtual | void | Executes the concrete command operation on receiver entities. |
| `undo()` | pure virtual | void | Reverses the effects of the executed command on receiver entities. |
| `~Protocol()` | virtual (not pure) | void | Ensures safe polymorphic destruction of concrete command objects. |

**Implementation Notes:** Declares pure virtual methods so that Dispatcher can invoke `execute()` and `undo()` polymorphically.

---

### Dispatcher (Invoker)

**Role:** Holds, executes, and tracks commands

#### Attributes

| Name | Type | Access | Purpose |
|------|------|--------|---------|
| `commandQueue` | `std::queue<Protocol*>` | private | Stores pending commands scheduled for execution. |
| `commandHistory` | `std::stack<Protocol*>` | private | Stores executed commands to enable sequential undo operations. |

**Ownership:** Takes full ownership of all heap-allocated `Protocol*` pointers pushed to its queue or history stack.

#### Methods

| Method | Purpose | Steps |
|--------|---------|-------|
| `issueCommand(cmd : Protocol*)` | Executes a command immediately and logs it into history. | 1. Call `cmd->execute()`. 2. Push `cmd` onto `commandHistory`. 3. Log command execution output. |
| `undoLast()` | Reverses and deletes the most recently executed command. | 1. Check if `commandHistory` is empty; if empty, log warning and return safely. 2. Pop top command `cmd` from `commandHistory`. 3. Call `cmd->undo()`. 4. `delete cmd;` |
| `~Dispatcher()` | Cleans up remaining allocated commands. | Pops and deletes all pointers in `commandQueue` and `commandHistory` until both containers are empty. |

#### Destructor Implementation

```cpp
Dispatcher::~Dispatcher() {
    while (!commandQueue.empty()) {
        delete commandQueue.front();
        commandQueue.pop();
    }
    while (!commandHistory.empty()) {
        delete commandHistory.top();
        commandHistory.pop();
    }
}
```

---

### Concrete Commands

#### Evacuate

**Receivers:** `SecurityGuards*`, `FacilityStaff*`, `AccessControlTeam*`

**Why these receivers:** Evacuating a building requires security personnel to herd occupants, facility staff to verify open physical exits, and access control to unlock all automated perimeter gates.

| Attribute | Type | Owned? | Purpose |
|-----------|------|--------|---------|
| `guards` | `SecurityGuards*` | No | Security personnel reference |
| `facility` | `FacilityStaff*` | No | Facility staff reference |
| `access` | `AccessControlTeam*` | No | Access control reference |

**Constructor:** `Evacuate(g : SecurityGuards*, f : FacilityStaff*, a : AccessControlTeam*)`

**execute() steps:**
1. Call `guards->clearBuilding()`
2. Call `facility->securePremises()`
3. Call `access->grantEmergencyAccess()`

**undo() steps:**
1. Call `access->revokeAccess("ALL", "EVACUEE")`
2. Call `guards->issueWarning()`

**Destructor:** `~Evacuate()` does not delete any receiver pointers. Receivers are aggregated dependencies managed externally.

---

#### Deescalate

**Receivers:** `SecurityGuards*`

| Attribute | Type | Owned? |
|-----------|------|--------|
| `guards` | `SecurityGuards*` | No |

**Constructor:** `Deescalate(s : SecurityGuards*)`

**execute() steps:**
1. Call `guards->issueWarning()` with stand-down advisory
2. Reduce security alert status

**undo() steps:**
1. Call `guards->requestBackup()`

**Destructor:** `~Deescalate()` does not delete `guards` pointer.

---

#### EmergencyEscalation

**Receivers:** `SecurityGuards*`, `FacilityStaff*`, `AccessControlTeam*`, `FirstAidTeam*`, `EmergencyResponder*` (police, ambulance, fire)

| Attribute | Type | Owned? |
|-----------|------|--------|
| `guards` | `SecurityGuards*` | No |
| `facility` | `FacilityStaff*` | No |
| `access` | `AccessControlTeam*` | No |
| `medics` | `FirstAidTeam*` | No |
| `police` | `EmergencyResponder*` | No |
| `ambulance` | `EmergencyResponder*` | No |
| `fire` | `EmergencyResponder*` | No |

**Constructor:** `EmergencyEscalation(s, f, a, m, p, fire, am)`

**execute() steps:**
1. Call `guards->clearBuilding()`
2. Call `facility->securePremises()`
3. Call `access->grantEmergencyAccess()`
4. Call `medics->emergencyEscalation()`
5. Call `police->respond("CAMPUS", Threat::SHOOTING)`
6. Call `ambulance->respond("CAMPUS", Threat::MEDICAL_EMERGENCY)`
7. Call `fire->respond("CAMPUS", Threat::FIRE)`

**undo() steps:**
1. Call `access->revokeAccess("ALL", "EMERGENCY")`
2. Log cancellation notification for external services

**Destructor:** `~EmergencyEscalation()` does not delete receiver pointers.

---

#### Resolve

**Receivers:** `FacilityStaff*`, `AccessControlTeam*`

| Attribute | Type | Owned? |
|-----------|------|--------|
| `facility` | `FacilityStaff*` | No |
| `access` | `AccessControlTeam*` | No |

**Constructor:** `Resolve(f : FacilityStaff*, a : AccessControlTeam*)`

**execute() steps:**
1. Call `facility->dispatchMaintanance()`
2. Call `access->unlockZone("ALL")`
3. Log incident resolution status

**undo() steps:**
1. Call `access->lockdownZone("ALL")`

**Destructor:** `~Resolve()` does not delete receiver pointers.

---

#### Assist

**Receivers:** `FirstAidTeam*`, `AccessControlTeam*`

| Attribute | Type | Owned? |
|-----------|------|--------|
| `medics` | `FirstAidTeam*` | No |
| `access` | `AccessControlTeam*` | No |

**Constructor:** `Assist(m : FirstAidTeam*, a : AccessControlTeam*)`

**execute() steps:**
1. Call `medics->assesInjury()`
2. Call `medics->treatInjury()`
3. Call `access->grantEmergencyAccess()`

**undo() steps:**
1. Log rollback of assistance protocol

**Destructor:** `~Assist()` does not delete receiver pointers.

---

#### GrantAccess

**Receivers:** `AccessControlTeam*`

| Attribute | Type | Owned? |
|-----------|------|--------|
| `access` | `AccessControlTeam*` | No |
| `zone` | `std::string` | Yes (value copy) |
| `role` | `std::string` | Yes (value copy) |

**Constructor:** `GrantAccess(a : AccessControlTeam*, zone : std::string, role : std::string)`

**execute() steps:**
1. Call `access->grantEmergencyAccess()`

**undo() steps:**
1. Call `access->revokeAccess(zone, role)`

**Destructor:** `~GrantAccess()` does not delete `access` pointer.

---

#### Isolate

**Receivers:** `AccessControlTeam*`

| Attribute | Type | Owned? |
|-----------|------|--------|
| `access` | `AccessControlTeam*` | No |
| `zone` | `std::string` | Yes (value copy) |

**Constructor:** `Isolate(a : AccessControlTeam*, zone : std::string)`

**execute() steps:**
1. Call `access->lockdownZone(zone)`
2. Log zone isolation status

**undo() steps:**
1. Call `access->unlockZone(zone)`

**Destructor:** `~Isolate()` does not delete `access` pointer.

---

## Pattern 2: Mediator

### Participants

| Role | Implementation |
|------|-----------------|
| **Mediator (interface)** | `CommunicationTeam` |
| **ConcreteMediator** | `CommunicationHub` |
| **Colleague (abstract)** | `FirstResponder` |
| **ConcreteColleagues** | `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam` |

### Why This Pattern

The Mediator pattern eliminates direct, many-to-many dependencies between internal responder teams. Without a mediator, `SecurityGuards` would require direct pointers to `FacilityStaff`, `AccessControlTeam`, and `FirstAidTeam` to alert them of state updates, causing severe structural coupling.

`CommunicationHub` centralizes all event notifications so responder classes remain completely decoupled.

### How It Works

When an internal responder changes state or completes an action, it calls `this->changed("EVENT_NAME")`. The base method `FirstResponder::changed()` forwards this call to `hub->notify(this, "EVENT_NAME")`. The `CommunicationHub` iterates through registered responders, skips the sender instance, and invokes `receive("EVENT_NAME")` on all other registered colleagues.

---

### CommunicationTeam (Abstract Mediator Interface)

| Method | Virtual? | Returns | Purpose |
|--------|----------|---------|---------|
| `notify(r : FirstResponder*, event : const std::string&)` | pure virtual | void | Defines broadcast protocol for colleague events. |
| `~CommunicationTeam()` | pure virtual | void | Ensures safe polymorphic cleanup. |

**Out-of-line destructor implementation:**
```cpp
CommunicationTeam::~CommunicationTeam() {}
```

---

### CommunicationHub (ConcreteMediator)

**Role:** Coordinates colleague communication

#### Attributes

| Name | Type | Owned? | Purpose |
|------|------|--------|---------|
| `responders` | `std::vector<FirstResponder*>` | No | Non-owning vector of registered colleagues. |

#### Methods

| Method | Virtual? | Purpose | Steps |
|--------|----------|---------|-------|
| `notify(r : FirstResponder*, event : const std::string&)` | override | Broadcasts event to all colleagues except sender. | 1. Iterate through `responders`. 2. Skip element if `elem == r`. 3. Call `elem->receive(event)`. |
| `registerResponder(r : FirstResponder*)` | no | Adds colleague to broadcast list. | Appends `r` to `responders`. |
| `removeResponder(r : FirstResponder*)` | no | Removes colleague from list. | Erases `r` from `responders`. |
| `~CommunicationHub()` | override | Destructor. | Clears vector without deleting pointers. |

---

### FirstResponder (Abstract Colleague Base Class)

#### Attributes

| Name | Type | Access | Owned? | Purpose |
|------|------|--------|--------|---------|
| `hub` | `CommunicationTeam*` | protected | No | Non-owning reference to mediator. |

#### Methods

| Method | Virtual? | Purpose |
|--------|----------|---------|
| `FirstResponder(hub : CommunicationTeam*)` | No | Constructs colleague with mediator reference. |
| `receive(event : const std::string&)` | pure virtual | Handles incoming broadcast notifications. |
| `changed(event : const std::string&)` | NOT virtual | Forwards internal events to `hub->notify(this, event)`. |
| `~FirstResponder()` | pure virtual | Abstract base destructor. |

---

### SecurityGuards (ConcreteColleague)

**Constructor:** `SecurityGuards(hub : CommunicationTeam*) : FirstResponder(hub) {}`

| Event String | Reaction |
|--------------|----------|
| `"INCIDENT_REPORTED"` | Logs security awareness and increases patrol vigilance. |
| `"EMERGENCY_DECLARED"` | Triggers immediate perimeter security lockdown. |
| `"INCIDENT_RESOLVED"` | Stands down active tactical teams. |
| `"ZONE_LOCKED"` | Deploys guards to monitor locked zone boundaries. |

---

### FirstAidTeam (ConcreteColleague)

**Constructor:** `FirstAidTeam(hub : CommunicationTeam*) : FirstResponder(hub) {}`

| Event String | Reaction |
|--------------|----------|
| `"INCIDENT_REPORTED"` | Prepares medical equipment and triage kits. |
| `"BACKUP_NEEDED"` | Dispatches medical personnel to requested sector. |
| `"EMERGENCY_DECLARED"` | Prepares field trauma unit for incoming casualties. |

---

### FacilityStaff (ConcreteColleague)

**Constructor:** `FacilityStaff(hub : CommunicationTeam*) : FirstResponder(hub) {}`

| Event String | Reaction |
|--------------|----------|
| `"BUILDING_CLEARED"` | Dispatches facility staff to lock down utility infrastructure. |
| `"INCIDENT_RESOLVED"` | Calls `dispatchMaintanance()` to inspect structural damage. |
| `"ZONE_LOCKED"` | Secures secondary utility access points in locked sector. |

---

### AccessControlTeam (ConcreteColleague)

**Constructor:** `AccessControlTeam(hub : CommunicationTeam*) : FirstResponder(hub) {}`

| Event String | Reaction |
|--------------|----------|
| `"EMERGENCY_DECLARED"` | Unlocks all evacuation turnstiles and emergency exits. |
| `"INCIDENT_RESOLVED"` | Restores normal card-access security rules. |

**Special Note:** `lockdownZone()` locks electronic doors and calls `changed("ZONE_LOCKED")` to trigger mediator notifications.

---

## Pattern 3: Adapter

### Participants

| Role | Implementation |
|------|-----------------|
| **Target (interface)** | `EmergencyResponder` |
| **Adapters** | `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter` |
| **Adaptees** | `Ambulance`, `Police`, `FireFighter` |
| **Client** | `EmergencyEscalation` command |

### Why This Pattern

The Adapter pattern converts incompatible legacy interfaces of external emergency response services (Ambulance, Police, FireFighter) into the unified target interface (EmergencyResponder) expected by CampusGuard.

This allows CampusGuard to dispatch external services uniformly using `respond(location, threat)` without modifying vendor code.

---

### EmergencyResponder (Target Interface)

| Method | Virtual? | Returns | Purpose |
|--------|----------|---------|---------|
| `respond(location : const std::string&, threat : Threat)` | pure virtual | void | Dispatches responder to target location. |
| `getStatus()` | pure virtual | void | Queries operational status. |
| `~EmergencyResponder()` | pure virtual | void | Virtual destructor. |

---

### Ambulance & AmbulanceAdapter

**Adaptee Methods:** `dispatch(caseType : int, lat : double, lon : double)`, `getUnitAvailability()`

**Ownership:** `AmbulanceAdapter` owns its wrapped `Ambulance*` adaptee and deletes it in `~AmbulanceAdapter()`.

**respond(location, threat) translation:**
1. Maps location string to double GPS coordinates (lat, lon)
2. Maps Threat enum to integer caseType
3. Calls `adaptee->dispatch(caseType, lat, lon)`

---

### Police & PoliceAdapter

**Adaptee Methods:** `dispatch(location : string, severity : int, incidentCode : string)`, `confirmDeployment()`

**Ownership:** `PoliceAdapter` owns its wrapped `Police*` adaptee and deletes it in `~PoliceAdapter()`.

**respond(location, threat) translation:**
1. Maps Threat enum to integer severity and radio string incidentCode
   - Example: `Threat::SHOOTING` → `severity = 5`, `code = "10-71"`
2. Calls `adaptee->dispatch(location, severity, code)`

---

### FireFighter & FireFighterAdapter

**Adaptee Methods:** `alertStation(location : string, buildingNumber : int)`, `getResponseETA()`

**Ownership:** `FireFighterAdapter` owns its wrapped `FireFighter*` adaptee and deletes it in `~FireFighterAdapter()`.

**respond(location, threat) translation steps:**

1. Maps text location to integer building identifier:
```cpp
int bNum = 100;
if (location.find("Engineering") != std::string::npos) bNum = 101;
else if (location.find("Library") != std::string::npos) bNum = 202;
```

2. Invokes adaptee:
```cpp
adaptee->alertStation(location, bNum);
```

3. Queries ETA:
```cpp
int eta = adaptee->getResponseETA();
std::cout << "[FireFighterAdapter] Station alerted for Building " << bNum << ". ETA: " << eta << " mins.\n";
```

**getStatus():** Queries `adaptee->getResponseETA()` and prints active dispatch timeline.

---

## Pattern 4: State

### Participants

| Role | Implementation |
|------|-----------------|
| **Context** | `IncidentContext` |
| **State (interface)** | `IncidentState` |
| **ConcreteStates** | `NormalState`, `AlertState`, `EmergencyState`, `ResolvedState` |

### Why This Pattern

An active campus incident transitions through distinct lifecycle phases:

```
Normal → Alert → Emergency → Resolved
```

Hardcoding lifecycle logic via nested if-else blocks causes fragile code. The State pattern encapsulates state-specific rules inside dedicated class objects, enabling `IncidentContext` to dynamically swap its execution behavior at runtime.

---

### IncidentState (Abstract State Interface)

| Method | Virtual? | Returns | Purpose |
|--------|----------|---------|---------|
| `handleThreat(ctx : IncidentContext*, threat : Threat)` | pure virtual | void | Evaluates and processes incoming threat triggers. |
| `escalate(ctx : IncidentContext*)` | pure virtual | void | Transitions incident lifecycle to higher urgency. |
| `deescalate(ctx : IncidentContext*)` | pure virtual | void | Transitions incident lifecycle to lower urgency. |
| `resolve(ctx : IncidentContext*)` | pure virtual | void | Concludes active threat and initiates cleanup. |
| `getStateName()` | pure virtual | std::string | Returns active state label string. |
| `~IncidentState()` | virtual | void | Virtual base destructor. |

---

### IncidentContext (Context)

**Role:** Holds current state reference and delegates operations

#### Attributes

| Name | Type | Access | Owned? | Purpose |
|------|------|--------|--------|---------|
| `currentState` | `IncidentState*` | private | YES | Pointer to active state instance. |
| `activeThreat` | `Threat` | private | No | Active incident threat level. |
| `location` | `std::string` | private | No | Incident physical location. |

#### Methods

| Method | Purpose | Implementation Steps |
|--------|---------|---------------------|
| `IncidentContext()` | Default constructor. | `currentState = new NormalState();` |
| `setState(newState : IncidentState*)` | State transition helper. | 1. `delete currentState;` 2. `currentState = newState;` |
| `handleThreat(t : Threat, loc : std::string)` | Processes new threat event. | Updates fields and calls `currentState->handleThreat(this, t)`. |
| `escalate()` | Escalates system state. | Calls `currentState->escalate(this)`. |
| `deescalate()` | De-escalates system state. | Calls `currentState->deescalate(this)`. |
| `resolve()` | Resolves active incident. | Calls `currentState->resolve(this)`. |
| `~IncidentContext()` | Destructor. | `delete currentState;` |

---

### NormalState (ConcreteState)

**handleThreat():** 
- Logs initial threat event 
- Transitions: `ctx->setState(new AlertState());`

**escalate():** 
- Logs warning that an active threat must occur before escalation.

**deescalate() / resolve():** 
- Logs no-op advisory.

---

### AlertState (ConcreteState)

**handleThreat():** 
- Evaluates severity
- Transitions directly to `EmergencyState` if threat is high

**escalate():** 
- Transitions: `ctx->setState(new EmergencyState());`

**deescalate():** 
- Transitions back: `ctx->setState(new NormalState());`

**resolve():** 
- Transitions: `ctx->setState(new ResolvedState());`

---

### EmergencyState (ConcreteState)

**handleThreat():** 
- Logs ongoing severe emergency
- Alerts tactical backups

**escalate():** 
- Logs warning that system is already at maximum emergency level

**deescalate():** 
- Transitions back: `ctx->setState(new AlertState());`

**resolve():** 
- Transitions: `ctx->setState(new ResolvedState());`

---

### ResolvedState (ConcreteState)

**handleThreat():** 
- Re-opens incident: `ctx->setState(new AlertState());`

**escalate():** 
- Logs warning to re-open incident first

**deescalate():** 
- Transitions baseline: `ctx->setState(new NormalState());`

**resolve():** 
- Logs warning that incident is already resolved

---

## Pattern 5: Memento

### Participants

| Role | Implementation |
|------|-----------------|
| **Originator** | `IncidentTracker` |
| **Memento** | `IncidentMemento` |
| **Caretaker** | `IncidentHistory` |

### Why This Pattern

CampusGuard requires state restoration during system undo operations and historical auditing. The Memento pattern captures internal state snapshots of `IncidentTracker` without exposing private fields or violating encapsulation.

---

### IncidentMemento (Memento)

**Role:** Immutable state snapshot container

#### Attributes

| Name | Type | Access | Purpose |
|------|------|--------|---------|
| `stateSnapshot` | `std::string` | private | State name at snapshot creation. |
| `zoneSnapshot` | `std::string` | private | Active zone at snapshot creation. |
| `timestamp` | `std::string` | private | Recorded snapshot timestamp. |

**Access Control:** All constructors are private and declare `friend class IncidentTracker;`. 

**Public getters:** `getStateSnapshot()`, `getZoneSnapshot()`, and `getTimestamp()` provide read-only access.

---

### IncidentTracker (Originator)

**Role:** Creates and restores from mementos

| Method | Returns | Purpose |
|--------|---------|---------|
| `createMemento(timestamp : std::string)` | `IncidentMemento*` | Allocates and returns new `IncidentMemento(currentStateName, activeZone, timestamp)`. |
| `restore(m : const IncidentMemento*)` | void | Restores internal `currentStateName` and `activeZone` from memento fields. |
| `setState(state : std::string, zone : std::string)` | void | Updates internal tracker fields prior to snapshot creation. |

---

### IncidentHistory (Caretaker)

**Role:** Maintains memento storage lifecycle

#### Attributes

| Name | Type | Access | Owned? | Purpose |
|------|------|--------|--------|---------|
| `history` | `std::vector<IncidentMemento*>` | private | YES | Stack storage for historical mementos. |

#### Methods

| Method | Purpose / Implementation Steps |
|--------|-------|
| `pushMemento(m : IncidentMemento*)` | Appends memento pointer to history vector. |
| `popMemento()` | Pops and returns last `IncidentMemento*`. Returns `nullptr` if empty. |
| `~IncidentHistory()` | Iterates through history vector and deletes all `IncidentMemento*` pointers. |

---

## Pattern 6: Facade

### Participants

| Role | Implementation |
|------|-----------------|
| **Facade** | `EmergencyResponseFacade` |
| **Subsystems** | `Dispatcher`, `CommunicationHub`, `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`, `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter`, `IncidentContext`, `IncidentHistory` |

### Why This Pattern

The CampusGuard ecosystem contains complex inter-pattern interactions across state machines, command queues, adapters, and mediator hubs. Exposing these individual components to high-level driver interfaces creates severe architectural coupling.

`EmergencyResponseFacade` provides a simple, unified interface:
- `reportIncident`
- `escalateEmergency`
- `isolateZone`
- `resolveIncident`
- `undoLastAction`

This hides system mechanics behind clean method calls.

---

### Lifecycle & Memory Ownership Architecture

To prevent double-free segmentation faults and dangling pointers:

1. **Subsystem Responders** (`SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`) are passed into `EmergencyResponseFacade` via pointer injection. The Facade maintains **Aggregation (Non-owning)** relationships with these responders and does not delete them in its destructor.

2. **Internal Orchestrators & Adapters** (`Dispatcher`, `CommunicationHub`, `IncidentContext`, `IncidentHistory`, `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter`) are **owned by EmergencyResponseFacade** and freed in `~EmergencyResponseFacade()`.

---

### EmergencyResponseFacade

#### Attributes

| Name | Type | Access | Owned? | Purpose |
|------|------|--------|--------|---------|
| `dispatcher` | `Dispatcher*` | private | YES | Internal command invoker and history manager. |
| `hub` | `CommunicationHub*` | private | YES | Internal mediator hub. |
| `guards` | `SecurityGuards*` | private | No | Aggregated security team responder. |
| `medics` | `FirstAidTeam*` | private | No | Aggregated medical team responder. |
| `facility` | `FacilityStaff*` | private | No | Aggregated facility staff responder. |
| `access` | `AccessControlTeam*` | private | No | Aggregated access control team responder. |
| `ambulance` | `EmergencyResponder*` | private | YES | External ambulance adapter. |
| `police` | `EmergencyResponder*` | private | YES | External police adapter. |
| `fire` | `EmergencyResponder*` | private | YES | External firefighter adapter. |
| `context` | `IncidentContext*` | private | YES | State pattern context object. |
| `history` | `IncidentHistory*` | private | YES | Memento caretaker history stack. |

---

### Workflow Methods

#### reportIncident(threat : Threat, zone : std::string)

1. Updates state machine: `context->handleThreat(threat, zone)`
2. Broadcasts event via mediator: `hub->notify(nullptr, "INCIDENT_REPORTED")`
3. Issues zone containment command: `dispatcher->issueCommand(new Isolate(access, zone))`
4. Creates snapshot: `history->pushMemento(tracker->createMemento("TIMESTAMP"))`

#### escalateEmergency()

1. Escalates state machine: `context->escalate()`
2. Instantiates `EmergencyEscalation` command containing subsystem responders and adapter pointers (guards, facility, access, medics, police, fire, ambulance)
3. Executes command via `dispatcher->issueCommand(escalationCmd)`
4. Pushes state snapshot to history

#### isolateZone(zone : std::string)

1. Creates and executes command: `dispatcher->issueCommand(new Isolate(access, zone))`

#### resolveIncident()

1. Updates state machine: `context->resolve()`
2. Executes resolution command: `dispatcher->issueCommand(new Resolve(facility, access))`
3. Broadcasts resolution via mediator: `hub->notify(nullptr, "INCIDENT_RESOLVED")`

#### undoLastAction()

1. Invokes rollback: `dispatcher->undoLast()`
2. Restores prior state snapshot from `history->popMemento()`

---

### Destructor Implementation

```cpp
EmergencyResponseFacade::~EmergencyResponseFacade() {
    // Delete internally owned orchestrators, adapters, and state managers
    delete dispatcher;
    delete hub;
    delete context;
    delete history;
    delete ambulance;
    delete police;
    delete fire;

    // Subsystem responders (guards, medics, facility, access) are non-owning 
    // aggregated references owned by the main application driver and are NOT deleted here.
}
```

---

## Summary

CampusGuard 2026 integrates six Gang of Four design patterns to create a robust, maintainable emergency response system:

| Pattern | Purpose | Key Benefit |
|---------|---------|------------|
| **Command** | Encapsulate emergency protocols | Queue, log, and undo actions |
| **Mediator** | Decouple responder teams | Eliminate hard dependencies |
| **Adapter** | Integrate external services | Uniform interface for diverse responders |
| **State** | Manage incident lifecycle | Clean state transitions |
| **Memento** | Capture system snapshots | Enable undo and auditing |
| **Facade** | Unify complex subsystems | Simple, clean public API |

The system prioritizes **memory safety**, **decoupling**, and **flexibility** to handle university campus emergencies effectively.