# SYSTEM OVERVIEW

> CampusGuard 2026 is an automated emergency response management system for university campuses. It detects and registers incoming threats, manages the full incident lifecycle across four escalating states (Moderate → Urgent → Emergency → Resolved), coordinates internal responder teams through a centralised mediator, and dispatches external emergency services (police, ambulance, fire) through adapters. The system supports command-based action queuing with undo, memento-based state snapshots for rollback, and a facade that exposes a clean six-method interface hiding all subsystem complexity from the client.

---

# PATTERN 1: COMMAND

## PARTICIPANTS

- Command (interface) = `Protocol`
- Invoker = `Dispatcher`
- ConcreteCommands = `Evacuate`, `Deescalate`, `EmergencyEscalation`, `Resolve`, `Assist`, `GrantAccess`, `Isolate`
- Receivers = `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`, `EmergencyResponder`

## WHY THIS PATTERN

> The Command pattern decouples the entity requesting an action (EmergencyResponseFacade or Dispatcher) from the receiver entities that carry it out (SecurityGuards, FacilityStaff, AccessControlTeam). By wrapping every physical response into a discrete object that implements the Protocol interface, the system can queue pending actions, log executed commands in order, and support rollback via undo() without the invoker knowing anything about the receivers or what they do.

## `Protocol`

**Role:** Command interface — all concrete commands inherit from this.

| Method | virtual? | Returns | Purpose |
|---|---|---|---|
| `execute()` | pure virtual | void | Executes the concrete command operation on its receiver entities. |
| `undo()` | pure virtual | void | Reverses the effects of the previously executed command on its receivers. |
| `~Protocol()` | virtual (not pure) | void | Ensures safe polymorphic destruction of concrete command objects via virtual dispatch. |

> Notes on implementation: Since `execute()` and `undo()` are pure virtual, `Protocol` cannot be instantiated directly. `Dispatcher` calls these polymorphically on any `Protocol*` without knowing the concrete type behind the pointer.

---

## `Dispatcher`

**Role:** Invoker — holds and fires commands.

**Attributes:**

| Name | Type | Access | Purpose |
|---|---|---|---|
| `commandQueue` | `std::queue<Protocol*>` | private | Stores pending commands scheduled for future execution. |
| `commandHistory` | `std::stack<Protocol*>` | private | Stores executed commands in order to support sequential undo operations. |

**Owns:** All `Protocol*` pointers in both containers.

| Method | Purpose | Steps |
|---|---|---|
| `issueCommand(cmd : Protocol*)` | Executes a command immediately and records it. | 1. Call `cmd->execute()`. <br> 2. Push `cmd` onto `commandHistory`. <br> 3. Log that the command was executed. |
| `undoLast()` | Reverses and deletes the most recently executed command. | 1. Check if `commandHistory` is empty — if so, log a warning and return safely. <br> 2. Pop the top `cmd` from `commandHistory`. <br> 3. Call `cmd->undo()`, then `delete cmd`. |
| `~Dispatcher()` | Cleans up all remaining allocated commands. | Pops and deletes every `Protocol*` from both `commandQueue` and `commandHistory` until both are empty. |

> Invalid operation case handled here: If `undoLast()` is called when `commandHistory` is empty, a warning is logged and the method returns immediately without crashing.

---

## `Evacuate`

**Receivers:** `SecurityGuards*`, `FacilityStaff*`, `AccessControlTeam*`

**Why these receivers:** Evacuating a building requires security guards to direct and clear occupants, facility staff to verify and secure utility exits, and access control to unlock all automated perimeter gates so evacuees can exit safely.

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `guards` | `SecurityGuards*` | No |
| `facility` | `FacilityStaff*` | No |
| `access` | `AccessControlTeam*` | No |

**Constructor:** `Evacuate(g : SecurityGuards*, f : FacilityStaff*, a : AccessControlTeam*)`

> Initialises all three receiver pointer attributes by assignment. Does not allocate any receivers — they are injected externally.

**`execute()` steps:**
1. Call `guards->clearBuilding()`.
2. Call `facility->securePremises()`.
3. Call `access->grantEmergencyAccess()`.

**`undo()` steps:**
1. Call `access->revokeAccess("ALL", "EVACUEE")`.
2. Call `guards->issueWarning()`.

**`~Evacuate()`:**
> Does not delete any receivers. All receiver pointers are non-owning aggregated references — the receivers are owned and managed by `EmergencyResponseFacade`. Deleting them here would cause a double-free.

---

## `Deescalate`

**Receivers:** `SecurityGuards*`

**Why these receivers:** De-escalating only requires security guards to stand down and reduce their alert posture. No facility or access changes are needed at this stage.

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `guards` | `SecurityGuards*` | No |

**Constructor:** `Deescalate(s : SecurityGuards*)`

**`execute()` steps:**
1. Call `guards->issueWarning()` with a stand-down advisory message.
2. Reduce the security alert status.

**`undo()` steps:**
1. Call `guards->requestBackup()`.

**`~Deescalate()`:**
Does not delete `guards` pointer — it is a non-owning reference managed externally.

---

## `EmergencyEscalation`

**Receivers:** `SecurityGuards*`, `FacilityStaff*`, `AccessControlTeam*`, `FirstAidTeam*`, `EmergencyResponder*` (police, ambulance, fire)

**Why these receivers:** A full emergency escalation requires every internal team to respond simultaneously, and all three external emergency services must be dispatched. Every subsystem is needed at this severity level.

> Note: This is where Command meets Adapter — the command calls `respond()` on EmergencyResponder* pointers without knowing they are adapters.

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `guards` | `SecurityGuards*` | No |
| `facility` | `FacilityStaff*` | No |
| `access` | `AccessControlTeam*` | No |
| `medics` | `FirstAidTeam*` | No |
| `police` | `EmergencyResponder*` | No |
| `ambulance` | `EmergencyResponder*` | No |
| `fire` | `EmergencyResponder*` | No |

**Constructor:** `EmergencyEscalation(s, f, a, m, p, fire, am)`

**`execute()` steps:**
1. Call `guards->clearBuilding()`.
2. Call `facility->securePremises()`.
3. Call `access->grantEmergencyAccess()`.
4. Call `medics->emergencyEscalation()`.
5. Call `police->respond("CAMPUS", Threat::SHOOTING)`.
6. Call `ambulance->respond("CAMPUS", Threat::MEDICAL_EMERGENCY)`.
7. Call `fire->respond("CAMPUS", Threat::FIRE)`.

**`undo()` steps:**
1. Call `access->revokeAccess("ALL", "EMERGENCY")`.
2. Log cancellation notification for all dispatched external services.

**`~EmergencyEscalation()`:**
Does not delete any receiver pointers — all are non-owning references managed by `EmergencyResponseFacade`.

---

## `Resolve`

**Receivers:** `FacilityStaff*`, `AccessControlTeam*`

**Why these receivers:** Resolution requires facility staff to inspect and restore the physical premises, and access control to unlock all zones and restore normal access rules. Security is stood down separately via the mediator.

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `facility` | `FacilityStaff*` | No |
| `access` | `AccessControlTeam*` | No |

**Constructor:** `Resolve(f : FacilityStaff*, a : AccessControlTeam*)`

**`execute()` steps:**
1. Call `facility->dispatchMaintanance()`.
2. Call `access->unlockZone("ALL")`.
3. Log incident resolution status.

**`undo()` steps:**
1. Call `access->lockdownZone("ALL")`.

**`~Resolve()`:**
Does not delete receiver pointers — they are non-owning references managed externally.

---

## `Assist`

**Receivers:** `FirstAidTeam*`, `AccessControlTeam*`

**Why these receivers:** Medical assistance requires the first aid team to assess and treat injuries on site, and access control to grant emergency entry to the affected zone so medics can reach casualties.

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `medics` | `FirstAidTeam*` | No |
| `access` | `AccessControlTeam*` | No |

**Constructor:** `Assist(m : FirstAidTeam*, a : AccessControlTeam*)`

**`execute()` steps:**
1. Call `medics->assesInjury()`.
2. Call `medics->treatInjury()`.
3. Call `access->grantEmergencyAccess()`.

**`undo()` steps:**
1. Log rollback of the assistance protocol.

**`~Assist()`:**
Does not delete receiver pointers — they are non-owning references managed externally.

---

## `GrantAccess`

**Receivers:** `AccessControlTeam*`

**Why these receivers:** Granting zone access is purely an access control operation — only `AccessControlTeam` manages electronic door locks and role-based permissions.

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `access` | `AccessControlTeam*` | No |
| `zone` | `std::string` | Yes (value) |
| `role` | `std::string` | Yes (value) |

**Constructor:** `GrantAccess(a : AccessControlTeam*, zone : std::string, role : std::string)`

**`execute()` steps:**
1. Call `access->grantEmergencyAccess()`.

**`undo()` steps:**
1. Call `access->revokeAccess(zone, role)`.

**`~GrantAccess()`:**
Does not delete `access` pointer — it is a non-owning reference. The `zone` and `role` strings are value copies and are destroyed automatically.

---

## `Isolate`

**Receivers:** `AccessControlTeam*`

**Why these receivers:** Zone isolation is an access control operation — only `AccessControlTeam` can lock electronic doors and restrict zone entry.

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `access` | `AccessControlTeam*` | No |
| `zone` | `std::string` | Yes (value) |

**Constructor:** `Isolate(a : AccessControlTeam*, zone : std::string)`

**`execute()` steps:**
1. Call `access->lockdownZone(zone)`.
2. Log zone isolation status.

> Note: Inside lockdownZone(), AccessControlTeam calls changed("ZONE_LOCKED") — this is where Command triggers the Mediator.

**`undo()` steps:**
1. Call `access->unlockZone(zone)`.

**`~Isolate()`:**
Does not delete `access` pointer — it is a non-owning reference. The `zone` string is a value copy and is destroyed automatically.

---

---

# PATTERN 2: MEDIATOR

## PARTICIPANTS

- Mediator (interface) = `CommunicationTeam`
- ConcreteMediator = `CommunicationHub`
- Colleague (abstract) = `FirstResponder`
- ConcreteColleagues = `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`

## WHY THIS PATTERN

> Without a mediator, every colleague would need direct pointers to every other colleague to notify them of events — `SecurityGuards` would hold references to `FacilityStaff`, `AccessControlTeam`, and `FirstAidTeam`, and each would reference the others back. This creates a dense many-to-many coupling web. `CommunicationHub` centralises all notifications so each colleague only ever talks to the hub, keeping all responder classes completely decoupled from one another.

## HOW IT WORKS

> When a colleague changes state or completes an action, it calls `this->changed("EVENT_NAME")`. Because `changed()` is not virtual, it always does the same thing for every colleague: it calls `hub->notify(this, "EVENT_NAME")`. The `CommunicationHub` receives the broadcast, iterates through its list of registered responders, skips the sender instance (to avoid notifying itself), and calls `receive("EVENT_NAME")` on every other registered colleague. Each colleague then reacts to the event in its own way through its concrete `receive()` implementation.

---

## `CommunicationTeam`

**Role:** Abstract mediator interface.

| Method | virtual? | Returns | Purpose |
|---|---|---|---|
| `notify(r : FirstResponder*, event : const std::string&)` | pure virtual | void | Defines the broadcast contract — concrete mediators must implement this to route events between colleagues. |
| `~CommunicationTeam()` | pure virtual | void | Ensures safe polymorphic cleanup of mediator subclasses. |

> Note on pure virtual destructor — needs out-of-line definition: Even though `~CommunicationTeam()` is pure virtual, C++ requires an out-of-line body for it because destructors are always called in the inheritance chain. Add `CommunicationTeam::~CommunicationTeam() {}` in the `.cpp` file.

---

## `CommunicationHub`

**Role:** ConcreteMediator — coordinates all colleague communication.

**Attributes:**

| Name | Type | Owned? | Purpose |
|---|---|---|---|
| `responders` | `std::vector<FirstResponder*>` | No | Non-owning list of all registered colleague responders that receive event broadcasts. |

| Method | virtual? | Purpose | Steps |
|---|---|---|---|
| `notify(r : FirstResponder*, event : const std::string)` | override (NOT pure virtual) | Broadcasts the event to all registered colleagues except the sender. | 1. Iterate through `responders`. <br> 2. Skip the element if `elem == r` (the sender). <br> 3. Call `elem->receive(event)` on all others. |
| `registerResponder(r : FirstResponder*)` | no | Adds a colleague to the broadcast list. | Append `r` to the `responders` vector. |
| `removeResponder(r : FirstResponder*)` | no | Removes a colleague from the broadcast list. | Erase `r` from the `responders` vector. |
| `~CommunicationHub()` | override | Destructor — clears the responders list. | Clears the vector without deleting any pointers. |

> Important: Why does ~CommunicationHub() NOT delete anything in responders? The `FirstResponder` objects (SecurityGuards, FirstAidTeam, FacilityStaff, AccessControlTeam) are owned and will be deleted by `EmergencyResponseFacade`. The hub only holds borrowed references to them. Deleting them here would cause a double-free segmentation fault.

---

## `FirstResponder`

**Role:** Abstract colleague base class.

**Attributes:**

| Name | Type | Access | Owned? | Purpose |
|---|---|---|---|---|
| `hub` | `CommunicationTeam*` | protected | No | Non-owning reference to the mediator, used by `changed()` to broadcast events upward. |

| Method | virtual? | Purpose |
|---|---|---|
| `FirstResponder(hub : CommunicationTeam*)` | — | Stores the hub pointer in the protected attribute. |
| `receive(event : const std::string&)` | pure virtual | Each concrete colleague handles incoming events differently — pure virtual forces subclasses to define their own reactions. |
| `changed(event : const std::string&)` | NOT virtual | Always calls `hub->notify(this, event)` to forward internal events up to the mediator. |
| `~FirstResponder()` | pure virtual | Abstract base destructor — requires out-of-line definition in `.cpp`. |

> Why is `changed()` NOT virtual? The forwarding behaviour is fixed and identical for every colleague — they all call `hub->notify(this, event)`. No subclass ever needs a different forwarding logic, so making it non-virtual enforces this uniformity.

> Why is `receive()` pure virtual? Each concrete colleague reacts differently to the same event string. `SecurityGuards` locks down on `"EMERGENCY_DECLARED"` while `AccessControlTeam` unlocks exits. The reaction is inherently type-specific and must be defined by each concrete class.

---

## `SecurityGuards`

**Constructor:** `SecurityGuards(hub : CommunicationTeam*)`

| Method | Purpose |
|---|---|
| `receive(event : const std::string&)` | Dispatches on the event string and triggers the appropriate security reaction. |
| `clearBuilding()` | Directs all occupants to evacuate the building; calls `changed("BUILDING_CLEARED")` to notify colleagues. |
| `issueWarning()` | Issues a stand-down or advisory warning to occupants and staff. |
| `requestBackup()` | Requests additional security reinforcement; calls `changed("BACKUP_NEEDED")` to notify colleagues. |
| `~SecurityGuards()` | Destructor — no owned pointers to clean up. |

**`receive()` — what happens for each event:**

| Event string | Reaction |
|---|---|
| `"INCIDENT_REPORTED"` | Logs security awareness and increases patrol vigilance across campus. |
| `"EMERGENCY_DECLARED"` | Triggers immediate perimeter security lockdown. |
| `"INCIDENT_RESOLVED"` | Stands down all active tactical teams. |
| `"ZONE_LOCKED"` | Deploys guards to monitor the boundaries of the locked zone. |

**`clearBuilding()` — what does it do and what does it call on the mediator?**

> Directs all occupants to evacuate. After clearing, calls `this->changed("BUILDING_CLEARED")` which forwards to `hub->notify(this, "BUILDING_CLEARED")`, notifying `FacilityStaff` to lock down utilities and other colleagues to react accordingly.

**`requestBackup()` — what does it call on the mediator?**

> Calls `this->changed("BACKUP_NEEDED")` which forwards to `hub->notify(this, "BACKUP_NEEDED")`, notifying `FirstAidTeam` to dispatch additional medical personnel to the sector.

---

## `FirstAidTeam`

**Constructor:** `FirstAidTeam(hub : CommunicationTeam*)`

| Method | Purpose |
|---|---|
| `receive(event : const std::string&)` | Dispatches on the event string and triggers the appropriate medical reaction. |
| `treatInjury()` | Administers first aid treatment to injured personnel on site. |
| `assesInjury()` | Evaluates the severity of injuries before deciding on a treatment plan. |
| `emergencyEscalation()` | Prepares the field trauma unit for incoming casualties. |
| `~FirstAidTeam()` | Destructor — no owned pointers to clean up. |

**`receive()` — what happens for each event:**

| Event string | Reaction |
|---|---|
| `"INCIDENT_REPORTED"` | Prepares medical equipment and triage kits. |
| `"BACKUP_NEEDED"` | Dispatches additional medical personnel to the requested sector. |
| `"EMERGENCY_DECLARED"` | Prepares field trauma unit for incoming casualties. |

**`emergencyEscalation()` — does it call the ambulance directly? Explain what it does instead:**

> No — `FirstAidTeam` does not call the ambulance directly. Doing so would couple a colleague directly to an external service, bypassing the Adapter and Command layers. Instead, `emergencyEscalation()` prepares the on-site trauma unit and may call `this->changed("EMERGENCY_DECLARED")` to notify other colleagues. The actual ambulance dispatch is handled by the `EmergencyEscalation` command calling `ambulance->respond()` through the `AmbulanceAdapter`.

---

## `FacilityStaff`

**Constructor:** `FacilityStaff(hub : CommunicationTeam*)`

| Method | Purpose |
|---|---|
| `receive(event : const std::string&)` | Dispatches on the event string and triggers the appropriate facility reaction. |
| `dispatchMaintanance()` | Sends a maintenance crew to inspect and repair structural damage post-incident. |
| `securePremises()` | Locks down utility infrastructure and secondary building access points. |
| `~FacilityStaff()` | Destructor — no owned pointers to clean up. |

> Note: keep the spelling `dispatchMaintanance` — it matches the UML exactly.

**`receive()` — what happens for each event:**

| Event string | Reaction |
|---|---|
| `"BUILDING_CLEARED"` | Dispatches facility staff to lock down utility infrastructure. |
| `"INCIDENT_RESOLVED"` | Calls `dispatchMaintanance()` to inspect structural damage. |
| `"ZONE_LOCKED"` | Secures secondary utility access points within the locked sector. |

---

## `AccessControlTeam`

**Constructor:** `AccessControlTeam(hub : CommunicationTeam*)`

| Method | Purpose |
|---|---|
| `receive(event : const std::string&)` | Dispatches on the event string and triggers the appropriate access control reaction. |
| `unlockZone(zone : const std::string&)` | Unlocks all electronic doors within the specified zone. |
| `lockdownZone(zone : const std::string&)` | Locks all electronic doors in the zone and broadcasts `"ZONE_LOCKED"` via mediator. |
| `grantEmergencyAccess()` | Unlocks all evacuation turnstiles and emergency exits system-wide. |
| `revokeAccess(zone : const std::string&, role : const std::string&)` | Revokes access permissions for the specified role within the specified zone. |
| `broadcastRestriction(zone : const std::string&)` | Broadcasts active access restrictions for a zone to all colleagues via mediator. |
| `~AccessControlTeam()` | Destructor — no owned pointers to clean up. |

**`receive()` — what happens for each event:**

| Event string | Reaction |
|---|---|
| `"EMERGENCY_DECLARED"` | Unlocks all evacuation turnstiles and emergency exits. |
| `"INCIDENT_RESOLVED"` | Restores normal card-access security rules across all zones. |

**`lockdownZone()` — what mediator call does it make after locking?**

> After locking the electronic doors, `lockdownZone()` calls `this->changed("ZONE_LOCKED")`, which forwards to `hub->notify(this, "ZONE_LOCKED")`. This notifies `SecurityGuards` to deploy to the zone boundary and `FacilityStaff` to secure utility access points in that sector.

**`broadcastRestriction()` — what mediator call does it make?**

> Calls `this->changed("ZONE_RESTRICTED")`, which forwards to `hub->notify(this, "ZONE_RESTRICTED")`, informing all colleagues that a zone-level access restriction is now active.

---

---

# PATTERN 3: ADAPTER

## PARTICIPANTS

- Target (interface) = `EmergencyResponder`
- Adapters = `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter`
- Adaptees = `Ambulance`, `Police`, `FireFighter`
- Client = `EmergencyEscalation` (command)

## WHY THIS PATTERN

> The three external emergency services each use a different method signature that is incompatible with what CampusGuard expects. Rather than modifying vendor code or hardcoding service-specific logic into the command, the Adapter wraps each service and translates the single unified call `respond(location, threat)` into whatever format the adaptee requires. The command layer stays clean and the external services stay untouched.

## THE MISMATCH — fill in the table

| Service | CampusGuard calls | Adaptee provides | What the adapter must translate |
|---|---|---|---|
| Ambulance | `respond(location, threat)` | `dispatch(caseType, lat, lon)` | Map location string → GPS lat/lon doubles; map Threat enum → integer caseType. |
| Police | `respond(location, threat)` | `dispatch(location, severity, incidentCode)` | Map Threat enum → integer severity and radio string incidentCode. |
| FireFighter | `respond(location, threat)` | `alertStation(location, buildingNumber)` | Map location string → integer building number. |

---

## `EmergencyResponder`

**Role:** Target interface — what CampusGuard expects of any external service.

| Method | virtual? | Returns | Purpose |
|---|---|---|---|
| `respond(location : const std::string&, threat : Threat)` | pure virtual | void | Dispatches the responder to the given location for the given threat type. |
| `getStatus()` | pure virtual | void | Queries and prints the current operational status of the external service. |
| `~EmergencyResponder()` | pure virtual | void | Ensures safe polymorphic destruction of adapter subclasses. |

---

## `Ambulance`

**Role:** Adaptee — external service with incompatible interface.

| Method | Returns | Implementation notes |
|---|---|---|
| `dispatch(caseType : int, lat : double, lon : double)` | void | Sends the ambulance unit to the GPS coordinates for the given numeric case type. Prints dispatch confirmation. |
| `getUnitAvailability()` | boolean | Returns whether the unit is available. Hardcoded to true for demo. |

> How is lat/lon determined? (Answer: determined by the adapter, not this class)

> How is availability determined? (Answer: hardcoded to true for demo)

---

## `AmbulanceAdapter`

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `adaptee` | `Ambulance*` | YES — delete in destructor |

**Constructor:** `AmbulanceAdapter(a : Ambulance*)`

> Stores the passed `Ambulance*` in the `adaptee` attribute. The adapter takes full ownership of the adaptee at construction.

**`~AmbulanceAdapter()`:**

> Yes, it deletes `adaptee`. The adapter is the sole owner of the `Ambulance` object — no other class holds a reference to it, so deleting it here is correct and necessary.

**`respond(location : const std::string&, threat : Threat)` — translation steps:**

Step 1 — Convert location string → lat/lon:
```
if location contains "Engineering":  lat = 25.7545, lon = 28.2314
if location contains "Library":      lat = 25.7556, lon = 28.2328
else (default):                      lat = 25.7560, lon = 28.2320
```

Step 2 — Convert Threat enum → caseType int:
```
SHOOTING          → 1
INJURY            → 2
FIGHT             → 3
MEDICAL_EMERGENCY → 4
FIRE              → 5
DAMAGED           → 6
```

Step 3 — Call adaptee:
```
adaptee->dispatch(caseType, lat, lon)
```

**`getStatus()`:**

> Calls `adaptee->getUnitAvailability()` and prints whether the ambulance unit is currently available for dispatch.

---

## `Police`

**Role:** Adaptee — external service with incompatible interface.

| Method | Returns | Implementation notes |
|---|---|---|
| `dispatch(location : const std::string, severity : int, incidentCode : std::string)` | void | Dispatches police units to the location with the given severity level and radio incident code. Prints dispatch confirmation. |
| `confirmDeployment()` | void | Logs and prints deployment confirmation for the last dispatched unit. |

---

## `PoliceAdapter`

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `adaptee` | `Police*` | YES |

**Constructor:** `PoliceAdapter(p : Police*)`

**`~PoliceAdapter()`:**
Deletes `adaptee` — the adapter is the sole owner of the `Police` object.

**`respond(location : const std::string&, threat : Threat)` — translation steps:**

Step 1 — Convert Threat → severity (int) and incidentCode (std::string):
```
SHOOTING  → severity = 5, code = "10-71"
FIGHT     → severity = 3, code = "10-10"
FIRE      → severity = 4, code = "10-70"
INJURY    → severity = 2, code = "10-52"
default   → severity = 1, code = "10-00"
```

Step 2 — Call adaptee:
```
adaptee->dispatch(location, severity, incidentCode)
```

**`getStatus()`:**
Calls `adaptee->confirmDeployment()` and prints the current deployment status of the police unit.

---

## `FireFighter`

**Role:** Adaptee — external service with incompatible interface.

| Method | Returns | Implementation notes |
|---|---|---|
| `alertStation(location : const std::string&, buildingNumber : int)` | void | Alerts the fire station to respond to the specified building. Prints alert confirmation. |
| `getResponseETA()` | int | Returns the estimated arrival time in minutes. Hardcoded to 5 for demo. |

> How is buildingNumber determined? (Answer: determined by the adapter)

> How is getResponseETA() determined? (Answer: hardcoded to 5 for demo)

---

## `FireFighterAdapter`

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `adaptee` | `FireFighter*` | YES |

**Constructor:** `FireFighterAdapter(f : FireFighter*)`

**`~FireFighterAdapter()`:**
Deletes `adaptee` — the adapter is the sole owner of the `FireFighter` object.

**`respond(location : const std::string&, threat : Threat)` — translation steps:**

Step 1 — Convert location → buildingNumber:
```
if location contains "Engineering":  buildingNumber = 101
if location contains "Library":      buildingNumber = 202
if location contains "Res":          buildingNumber = 303
else:                                buildingNumber = 100
```

Step 2 — Call adaptee:
```
adaptee->alertStation(location, buildingNumber)
```

Step 3 — Print ETA:
```
int eta = adaptee->getResponseETA()
print ETA
```

**`getStatus()`:**
Calls `adaptee->getResponseETA()` and prints the active dispatch timeline for the fire station response.

---

---

# PATTERN 4: STATE

## PARTICIPANTS

- State (abstract) = `IncidentState`
- ConcreteStates = `Moderate`, `Urgent`, `Emergency`, `Resolved`
- Context = `IncidentControl`

## WHY THIS PATTERN

> An incident passes through four distinct lifecycle phases with different rules for escalation, de-escalation, and validity. Hardcoding this logic with nested if-else chains based on an enum or integer status flag makes the code brittle — adding a new phase or changing a transition requires editing a large conditional block. The State pattern encapsulates each phase's behaviour in its own class, so `IncidentControl` simply delegates to its current state object and states swap themselves out at runtime.

## HOW TRANSITIONS WORK

> The current State object decides what the next state is — not the Context. When `escalate()` or `deescalate()` is called on `IncidentControl`, it delegates to `currentState->escalate(this)` or `currentState->deescalate(this)`. The state object then calls `context->setState(new NextState())` on the context, replacing itself. The context never contains transition logic — it only holds the current state and delegates all decisions to it.

---

## `IncidentState`

**Role:** Abstract state interface.

| Method | virtual? | Returns | Purpose |
|---|---|---|---|
| `escalate(context : IncidentControl*)` | pure virtual | void | Transitions the incident lifecycle to a higher urgency state. |
| `deescalate(context : IncidentControl*)` | pure virtual | void | Transitions the incident lifecycle to a lower urgency state. |
| `getLabel()` | pure virtual | std::string | Returns the string label of the current state. |
| `~IncidentState()` | pure virtual | void | Abstract base destructor — ensures safe polymorphic cleanup of concrete state objects. |

> Note: UML shows getLabel() return type as void — change to std::string in implementation.

> Note on pure virtual destructor — needs out-of-line definition in .cpp: Add `IncidentState::~IncidentState() {}` in `IncidentState.cpp` — pure virtual destructors still require a body because the destructor chain always calls up to the base.

---

## `IncidentControl`

**Role:** Context (State pattern) AND Originator (Memento pattern).

**Attributes:**

| Name | Type | Access | Owned? | Purpose |
|---|---|---|---|---|
| `currentState` | `IncidentState*` | private | YES | Pointer to the active state instance — deleted and replaced on every transition. |
| `threatCount` | `int` | private | — | Running count of currently active threats. |
| `activeThreats` | `std::map<std::string, Threat>` | private | — | Maps location strings to their active threat types. |

**Constructor:** Starts in `Moderate`. Why? Because on startup there are no active threats — `Moderate` is the baseline resting state before any incident is registered.

| Method | virtual? | Returns | Purpose | Steps |
|---|---|---|---|---|
| `IncidentControl()` | — | — | Initialises the context in the `Moderate` state. | Sets `currentState = new Moderate()`, `threatCount = 0`. |
| `~IncidentControl()` | — | — | Destroys the context and its current state. | `delete currentState`. |
| `setState(state : IncidentState*)` | no | void | Replaces the current state with a new one. | 1. `delete currentState`. <br> 2. `currentState = state`. <br> 3. Log state transition. |
| `escalate()` | no | void | Delegates escalation to the current state. | Calls `currentState->escalate(this)`. |
| `deescalate()` | no | void | Delegates de-escalation to the current state. | Calls `currentState->deescalate(this)`. |
| `addThreat(location, threat)` | no | void | Registers a new threat at the given location. | 1. Insert `(location, threat)` into `activeThreats`. <br> 2. Increment `threatCount`. <br> 3. Call `escalate()` if this is the first threat. <br> 4. Log the added threat. |
| `removeThreat(location)` | no | void | Removes a resolved threat by location. | 1. Erase `location` from `activeThreats`. <br> 2. Decrement `threatCount`. <br> 3. Call `deescalate()` if `threatCount` reaches zero. |
| `clearThreats()` | no | void | Clears all active threats. | 1. Clear `activeThreats` map. <br> 2. Reset `threatCount = 0`. <br> 3. Log that all threats were cleared. |
| `getThreatCount()` | no | int | Returns the current number of active threats. | Returns `threatCount`. |
| `getState()` | no | std::string | Returns the current state label. | Returns `currentState->getLabel()`. |
| `createMemento()` | no | `IncidentMemento*` | Captures and returns a snapshot of current state. | Returns `new IncidentMemento(threatCount, activeThreats, currentState->getLabel())`. |
| `restore(memento : IncidentMemento*)` | no | void | Restores internal state from a memento snapshot. | 1. Clear `activeThreats`. <br> 2. Set `activeThreats = memento->getActiveThreats()`. <br> 3. Set `threatCount = memento->getThreatCount()`. <br> 4. Read `label = memento->getStateLabel()`. <br> 5. Call `setState(new CorrectState())` based on the label. |

> Note: UML shows getState() and createMemento() returning void — fix to std::string and IncidentMemento*.

---

## Concrete States — Transition Table

Fill in what each state does for each operation:

| Current State | `escalate()` does | `deescalate()` does | `getLabel()` returns |
|---|---|---|---|
| `Moderate` | `context->setState(new Urgent())` | If `threatCount == 0`, transition to `Resolved`; else log no-op — already at minimum active state. | `"MODERATE"` |
| `Urgent` | `context->setState(new Emergency())` | `context->setState(new Moderate())` | `"URGENT"` |
| `Emergency` | ⚠️ INVALID OPERATION — explain: System is already at maximum severity. Print a warning message and return without any state transition. | `context->setState(new Urgent())` | `"EMERGENCY"` |
| `Resolved` | `context->setState(new Moderate())` — a new threat re-opens a resolved incident. | ⚠️ INVALID OPERATION — explain: Cannot de-escalate an already resolved incident. Print a warning and return without transitioning. | `"RESOLVED"` |

> The two INVALID OPERATION cases are required by the spec. Describe how each is handled sensibly (print warning, do nothing, no crash): Both cases print a descriptive warning string to `std::cout` identifying the invalid operation, then return immediately. No exception is thrown, no state pointer is modified, and no crash occurs. The system remains in its current valid state.

---

## `Moderate`

**`escalate(context : IncidentControl*)`:**
Calls `context->setState(new Urgent())` — transitions the incident to the next severity level.

**`deescalate(context : IncidentControl*)`:**
Checks `context->getThreatCount()`. If zero, calls `context->setState(new Resolved())`. Otherwise logs a warning that the system is already at minimum active state and returns without transitioning.

> Note: deescalate on Moderate should check threatCount before transitioning.

**`getLabel()`:** returns `"MODERATE"`

---

## `Urgent`

**`escalate(context : IncidentControl*)`:**
Calls `context->setState(new Emergency())` — transitions to full emergency level.

**`deescalate(context : IncidentControl*)`:**
Calls `context->setState(new Moderate())` — steps the incident back down to moderate.

**`getLabel()`:** returns `"URGENT"`

---

## `Emergency`

**`escalate(context : IncidentControl*)`:**
> This is invalid — prints `"[Emergency] Already at maximum emergency level — cannot escalate further."` and returns without any state transition. No crash, no state change.

**`deescalate(context : IncidentControl*)`:**
Calls `context->setState(new Urgent())` — steps the incident back down to urgent.

**`getLabel()`:** returns `"EMERGENCY"`

---

## `Resolved`

**`escalate(context : IncidentControl*)`:**
> Note: a new threat re-opens a resolved incident. Calls `context->setState(new Moderate())` to transition back into the active lifecycle.

**`deescalate(context : IncidentControl*)`:**
> This is invalid — prints `"[Resolved] Cannot de-escalate a resolved incident."` and returns without any state transition. The incident remains in the Resolved state.

**`getLabel()`:** returns `"RESOLVED"`

---

---

# PATTERN 5: MEMENTO

## PARTICIPANTS

- Originator = `IncidentControl` (same class as State Context)
- Memento = `IncidentMemento`
- Caretaker = `IncidentHistory`

## WHY THIS PATTERN

> CampusGuard needs to roll back to a previous incident state when `rollbackLastAction()` is called — for example, after a false alarm or an accidentally triggered escalation. The Memento pattern allows `IncidentControl` to save a complete snapshot of its internal fields (`threatCount`, `activeThreats`, `stateLabel`) into an `IncidentMemento` object without exposing any private attributes to outside classes, fully preserving encapsulation.

## HOW OWNERSHIP FLOWS

> `createMemento()` allocates a new `IncidentMemento` on the heap and returns the pointer — ownership transfers to the caller. `push()` receives that pointer and transfers ownership to `IncidentHistory`, which stores it in its `snapshots` stack. `pop()` removes the top pointer from the stack and returns it — ownership transfers back to the caller (the Facade). `restore()` reads the values from the memento into `IncidentControl`'s fields, after which the caller is responsible for deleting the memento pointer.

---

## `IncidentMemento`

**Role:** Stores a value snapshot of IncidentControl's state at a point in time.

**Attributes:**

| Name | Type | Access | Why value copy not pointer? |
|---|---|---|---|
| `threatCount` | `int` | private | A primitive int is always a safe value copy — no aliasing risk, no dangling pointer possible. |
| `activeThreats` | `std::map<std::string, Threat>` | private | `std::map` is a value type — copying it creates a fully independent snapshot with no shared state. |
| `stateLabel` | `std::string` | private | `std::string` is a value type — the copy is independent and cannot be invalidated by changes to the context. |

> Why does the memento store a string label for the state instead of an IncidentState* pointer? If we stored an `IncidentState*`, that pointer would be deleted by `setState()` the moment the context transitions away from that state — the memento would immediately hold a dangling pointer. A string label is a value copy that safely survives indefinitely, and `restore()` can reconstruct the correct concrete state by comparing the label.

**Constructor:** (private — only IncidentControl can call it via friend declaration)
```
IncidentMemento(count : int, threats : std::map<std::string, Threat>, label : std::string)
```

> What does friend class IncidentControl mean and why is it used here? `friend class IncidentControl` grants `IncidentControl` exclusive access to `IncidentMemento`'s private constructor and private attributes. This enforces the Memento pattern's encapsulation rule: only the Originator may create snapshots of its own state. All other classes — including `IncidentHistory` — can only store and return the pointer without inspecting its contents.

| Method | Returns | Purpose |
|---|---|---|
| `getThreatCount()` | int | Returns the snapshotted threat count. |
| `getActiveThreats()` | `std::map<std::string, Threat>` | Returns a copy of the snapshotted active threats map. |
| `getStateLabel()` | std::string | Returns the snapshotted state label string. |

> Note: UML shows all three returning void — fix return types in implementation.

> All three getters should be const methods. Why? The memento is a read-only snapshot — its contents must never change after creation. Marking getters `const` enforces this contractually and allows them to be called on `const IncidentMemento*` references without compiler errors.

---

## `IncidentHistory`

**Role:** Caretaker — stores mementos without inspecting their contents.

**Attributes:**

| Name | Type | Owned? |
|---|---|---|
| `snapshots` | `std::stack<IncidentMemento*>` | YES — owns all pointers |

| Method | Returns | Purpose | Steps |
|---|---|---|---|
| `push(memento : IncidentMemento*)` | void | Accepts ownership of a memento and stores it. | 1. Receive `IncidentMemento*` pointer. <br> 2. Push onto `snapshots` stack (takes ownership). |
| `pop()` | `IncidentMemento*` | Removes and returns the most recent snapshot, transferring ownership to caller. | 1. If stack is empty, return `nullptr`. <br> 2. Get top pointer. <br> 3. Pop and return it — caller is now responsible for deleting it. |
| `isEmpty()` | bool | Reports whether there are any stored snapshots. | Returns `snapshots.empty()`. |
| `~IncidentHistory()` | — | Deletes all remaining stored mementos. | |

> Note: UML shows pop() and isEmpty() returning void — fix return types.

> Invalid operation case for pop(): If `snapshots` is empty when `pop()` is called, return `nullptr` immediately. The caller must check for this before passing the result to `restore()`.

**`~IncidentHistory()`:** Must delete all remaining pointers.
```
while stack not empty:
    delete top
    pop
```

> CRITICAL RULE — what must the Caretaker NEVER do with the memento contents? The Caretaker must never read, inspect, or modify any field inside an `IncidentMemento`. It only stores the pointer and returns it. The internal state captured in the memento is the exclusive concern of `IncidentControl` — the Caretaker treats each pointer as an opaque token.

---

---

# PATTERN 6: FACADE

## PARTICIPANTS

- Facade = `EmergencyResponseFacade`
- Subsystems = `Dispatcher`, `IncidentControl`, `CommunicationHub`, `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`, `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter`, `IncidentHistory`

## WHY THIS PATTERN

> The CampusGuard ecosystem contains six interconnected subsystems — state machines, command queues, mediator hubs, adapters, and memento stacks. Exposing all of these directly to a client would require the client to understand and coordinate every subsystem, creating severe coupling. `EmergencyResponseFacade` hides all of this behind six clean methods so the client interacts with a single object and never touches an individual subsystem directly.

---

## `EmergencyResponseFacade`

**Attributes:**

| Name | Type | Owned? | Purpose |
|---|---|---|---|
| `dispatcher` | `Dispatcher*` | YES | Internal command invoker and history manager. |
| `controller` | `IncidentControl*` | YES | State machine context and memento originator. |
| `hub` | `CommunicationHub*` | YES | Internal mediator hub for colleague broadcasts. |
| `guards` | `SecurityGuards*` | YES | Security team colleague. |
| `medics` | `FirstAidTeam*` | YES | Medical team colleague. |
| `facility` | `FacilityStaff*` | YES | Facility staff colleague. |
| `access` | `AccessControlTeam*` | YES | Access control team colleague. |
| `police` | `EmergencyResponder*` | YES | points to PoliceAdapter |
| `ambulance` | `EmergencyResponder*` | YES | points to AmbulanceAdapter |
| `fire` | `EmergencyResponder*` | YES | points to FireFighterAdapter |
| `history` | `IncidentHistory*` | YES | Memento caretaker history stack. |

---

### Constructor — object construction order (order matters!)

> Why must the hub be constructed before the colleagues? Each colleague's constructor takes a `CommunicationTeam*` parameter and stores it. If the hub does not exist yet when colleagues are constructed, the pointer would be null or uninitialised, causing undefined behaviour the moment any colleague calls `changed()`. The hub must be a valid object before any colleague can reference it.

```
Step 1: hub      = new CommunicationHub()

Step 2: guards   = new SecurityGuards(hub)
        medics   = new FirstAidTeam(hub)
        facility = new FacilityStaff(hub)
        access   = new AccessControlTeam(hub)

Step 3: hub->registerResponder(guards)
        hub->registerResponder(medics)
        hub->registerResponder(facility)
        hub->registerResponder(access)

Step 4: ambulance = new AmbulanceAdapter(new Ambulance())
        police    = new PoliceAdapter(new Police())
        fire      = new FireFighterAdapter(new FireFighter())

Step 5: dispatcher = new Dispatcher()
        controller = new IncidentControl()
        history    = new IncidentHistory()
```

---

### Destructor — destruction order (reverse of construction)

> Why must destruction be in reverse order? Colleagues hold `hub*` pointers, and the hub holds `FirstResponder*` references to the colleagues. If `hub` were deleted first, the colleagues would be left with dangling `hub*` pointers — any destructor or cleanup code that calls `changed()` after that point would crash. Deleting colleagues first removes all live references, then `hub` can be safely destroyed knowing it holds no live pointers.

```
delete history
delete controller
delete dispatcher
delete fire       — adapter destructor deletes adaptee
delete police
delete ambulance
delete access
delete facility
delete medics
delete guards
delete hub        — does NOT delete responders, already deleted above
```

---

### `reportIncident(location : const std::string&, threat : Threat)`

**Subsystems coordinated (must be ≥ 3):**

| Step | Subsystem | What happens | Pattern(s) visible |
|---|---|---|---|
| 1 | `IncidentControl` | `controller->addThreat(location, threat)` registers the threat, increments threatCount, and triggers `escalate()` on the first threat — transitioning from Moderate to Urgent. | State |
| 2 | `IncidentHistory` | `history->push(controller->createMemento())` saves a snapshot of the post-addThreat state (threatCount, activeThreats, stateLabel). | Memento |
| 3 | `CommunicationHub` | `hub->notify(nullptr, "INCIDENT_REPORTED")` broadcasts to all colleagues — SecurityGuards increases patrols, FirstAidTeam prepares kits. | Mediator |
| 4 | `Dispatcher` | `dispatcher->issueCommand(new Isolate(access, location))` isolates the zone by locking it down and logging to command history. | Command |

**Fill in what happens at each step:**
1. `controller->addThreat(location, threat)` — registers the threat in the active threats map and triggers a state escalation if this is the first threat.
2. `history->push(controller->createMemento())` — captures a complete snapshot of the current controller state and stores it in the history stack.
3. `hub->notify(nullptr, "INCIDENT_REPORTED")` — broadcasts the incident to all registered colleagues so they can prepare their respective responses.
4. `dispatcher->issueCommand(new Isolate(access, location))` — locks down the incident zone via `AccessControlTeam`, which internally triggers a second mediator broadcast `"ZONE_LOCKED"`.

---

### `escalateToEmergency()`

**Subsystems coordinated — all 6 patterns visible here:**

| Step | Subsystem | What happens | Pattern(s) visible |
|---|---|---|---|
| 1 | `IncidentControl` | `controller->escalate()` delegates to the current state, which transitions the context to `Emergency` via `setState(new Emergency())`. | State |
| 2 | `IncidentHistory` | `history->push(controller->createMemento())` saves a post-escalation snapshot with stateLabel `"EMERGENCY"`. | Memento |
| 3 | `Dispatcher` | issues EmergencyEscalation command | Command + Adapter |
| 4 | `CommunicationHub` | `hub->notify(nullptr, "EMERGENCY_DECLARED")` broadcasts to all colleagues — AccessControlTeam unlocks exits, FirstAidTeam prepares trauma unit, SecurityGuards locks perimeter. | Mediator |

**Fill in what happens at each step:**
1. `controller->escalate()` — the current state (Urgent) calls `context->setState(new Emergency())`, replacing itself with the Emergency state.
2. `history->push(controller->createMemento())` — captures and stores a snapshot of the emergency state before any commands execute.
3. `dispatcher->issueCommand(new EmergencyEscalation(guards, facility, access, medics, police, ambulance, fire))` — `execute()` clears the building, secures premises, grants emergency access, prepares medics, and calls `respond()` on all three adapter pointers — each adapter translates and forwards to its adaptee.
4. `hub->notify(nullptr, "EMERGENCY_DECLARED")` — all colleagues react: AccessControlTeam unlocks evacuation exits, FirstAidTeam prepares trauma unit, SecurityGuards secures the perimeter.

---

### `resolveIncident()`

**Subsystems coordinated:**

| Step | Subsystem | What happens | Pattern(s) visible |
|---|---|---|---|
| 1 | `Dispatcher` | `dispatcher->issueCommand(new Resolve(facility, access))` dispatches maintenance and unlocks all zones. | Command |
| 2 | `IncidentControl` | `controller->deescalate()` transitions the state through Urgent → Moderate → Resolved (or directly, depending on state). | State |
| 3 | `IncidentHistory` | `history->push(controller->createMemento())` saves a snapshot of the resolved state. | Memento |
| 4 | `CommunicationHub` | `hub->notify(nullptr, "INCIDENT_RESOLVED")` broadcasts resolution — SecurityGuards stands down, AccessControlTeam restores normal access rules. | Mediator |

**Fill in what happens at each step:**
1. `dispatcher->issueCommand(new Resolve(facility, access))` — `execute()` calls `facility->dispatchMaintanance()` and `access->unlockZone("ALL")`, restoring physical access.
2. `controller->deescalate()` — the current state steps down the lifecycle; if threatCount is zero and state reaches Moderate, it transitions to Resolved.
3. `history->push(controller->createMemento())` — saves the post-resolve snapshot for audit purposes.
4. `hub->notify(nullptr, "INCIDENT_RESOLVED")` — all colleagues react: SecurityGuards stands down, AccessControlTeam restores card-access rules, FacilityStaff confirms premises are secure.

---

### `addThreat(location : const std::string&, threat : Threat)`

**Fill in what this method does:**
1. Delegates to `controller->addThreat(location, threat)` — inserts the threat into `activeThreats`, increments `threatCount`, and triggers `escalate()` if this is the first active threat.
2. Optionally logs or prints the registered threat details for the output trace.

---

### `rollbackLastAction()`

**Subsystems coordinated:**

| Step | Subsystem | What happens | Pattern(s) visible |
|---|---|---|---|
| 1 | `IncidentHistory` | `history->pop()` removes and returns the most recent `IncidentMemento*`, transferring ownership to the facade. | Memento |
| 2 | `IncidentControl` | `controller->restore(memento)` reads threatCount, activeThreats, and stateLabel from the memento and reconstructs the correct state object. | Memento + State |
| 3 | `Dispatcher` | `dispatcher->undoLast()` calls `undo()` on the last executed command and deletes it from history. | Command |

**Fill in what happens at each step:**
1. `history->pop()` — returns the last saved `IncidentMemento*`; if `history->isEmpty()` is true, log a warning and skip the restore step.
2. `controller->restore(memento)` — restores `threatCount`, copies `activeThreats`, reads `stateLabel`, and calls `setState(new CorrectState())` based on the label; then `delete memento`.
3. `dispatcher->undoLast()` — pops the most recent command from `commandHistory`, calls its `undo()` method, and deletes the command object.

> Invalid operation case here: If `history->isEmpty()` returns true when `rollbackLastAction()` is called, log `"[Facade] No history to roll back."` and return safely without calling restore or undoLast.

---

### `printHistory()`

> Delegates to `history` — iterates through the stored snapshots stack (without popping) and prints each memento's `stateLabel` and `threatCount` in order from most recent to oldest.

---

---

# OWNERSHIP SUMMARY

Fill in the last column:

| Object | Owner | Deleted in |
|---|---|---|
| `CommunicationHub` | `EmergencyResponseFacade` | `~EmergencyResponseFacade()` |
| `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam` | `EmergencyResponseFacade` | `~EmergencyResponseFacade()` |
| `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter` | `EmergencyResponseFacade` | `~EmergencyResponseFacade()` |
| `Ambulance` | `AmbulanceAdapter` | `~AmbulanceAdapter()` |
| `Police` | `PoliceAdapter` | `~PoliceAdapter()` |
| `FireFighter` | `FireFighterAdapter` | `~FireFighterAdapter()` |
| `Dispatcher` | `EmergencyResponseFacade` | `~EmergencyResponseFacade()` |
| `IncidentControl` | `EmergencyResponseFacade` | `~EmergencyResponseFacade()` |
| `IncidentHistory` | `EmergencyResponseFacade` | `~EmergencyResponseFacade()` |
| `Protocol*` commands | `Dispatcher` | `undoLast()` or `~Dispatcher()` |
| `IncidentState*` | `IncidentControl` | `setState()` or `~IncidentControl()` |
| `IncidentMemento*` | `IncidentHistory` while stored | Transferred to caller on `pop()`; caller must `delete` after `restore()`. |

**Three things that do NOT own their pointers (never delete them):**

| Class | Pointer it holds | Why it doesn't own it |
|---|---|---|
| `CommunicationHub` | `FirstResponder*` in responders | The responders are owned and deleted by `EmergencyResponseFacade` — deleting them in the hub would cause a double-free. |
| All concrete commands | receiver pointers (`guards*`, `access*` etc.) | The receivers are managed externally by the Facade; commands only borrow references to call methods on them during execute/undo. |
| `FirstResponder` | `hub*` | The hub is owned by `EmergencyResponseFacade` — `FirstResponder` only holds a borrowed reference to call `notify()` on it. |

---

---

# PATTERN INTERACTION — HOW THEY CONNECT

> A single call to `reportIncident()` immediately touches four patterns in sequence: the Facade coordinates, State reacts to the new threat and escalates, Memento captures the new state, Mediator broadcasts to all colleagues, and Command issues the zone isolation action — which itself triggers a second Mediator broadcast from inside `lockdownZone()`. `escalateToEmergency()` extends this further by adding the Adapter layer, as the EmergencyEscalation command calls `respond()` on three adapter pointers that each translate and forward to a differently-shaped adaptee.

## reportIncident() flow:

```
main() calls: system.reportIncident("Engineering Block A", Threat::FIRE)

[FACADE]
    ↓
    [STATE]   — controller->addThreat() inserts the threat into activeThreats, increments
                 threatCount to 1, and triggers escalate() on the first threat.
                 State transition: Moderate → setState(new Urgent())
    ↓
    [MEMENTO] — history->push(controller->createMemento()) saves snapshot:
                 { threatCount=1, activeThreats={"Engineering Block A": FIRE}, stateLabel="URGENT" }
    ↓
    [MEDIATOR] — hub->notify(nullptr, "INCIDENT_REPORTED") broadcasts to all colleagues:
                 SecurityGuards logs awareness and increases patrols;
                 FirstAidTeam prepares triage kits;
                 FacilityStaff and AccessControlTeam react based on their receive() tables.
    ↓
    [COMMAND] — dispatcher->issueCommand(new Isolate(access, "Engineering Block A"))
                 execute() calls access->lockdownZone("Engineering Block A")
                 inside that command's execute():
    ↓
    [MEDIATOR again] — lockdownZone() calls this->changed("ZONE_LOCKED")
                        hub->notify(access, "ZONE_LOCKED") fires:
                        SecurityGuards deploys to zone boundary;
                        FacilityStaff secures secondary utility access points in the sector.
```

## escalateToEmergency() flow:

```
main() calls: system.escalateToEmergency()

[FACADE]
    ↓
    [STATE]   — controller->escalate() → currentState->escalate(this)
                 Urgent calls context->setState(new Emergency())
                 Transition: Urgent → Emergency
    ↓
    [MEMENTO] — history->push(controller->createMemento()) saves snapshot:
                 { threatCount=1, activeThreats={"Engineering Block A": FIRE}, stateLabel="EMERGENCY" }
    ↓
    [COMMAND] — EmergencyEscalation::execute() calls respond() on all three adapter pointers:
        [ADAPTER] — police->respond("CAMPUS", Threat::SHOOTING) →
                    PoliceAdapter maps Threat::SHOOTING → severity=5, incidentCode="10-71"
                    → calls adaptee->dispatch("CAMPUS", 5, "10-71")
        [ADAPTER] — ambulance->respond("CAMPUS", Threat::MEDICAL_EMERGENCY) →
                    AmbulanceAdapter maps MEDICAL_EMERGENCY→caseType=4, "CAMPUS"→lat=25.7560, lon=28.2320
                    → calls adaptee->dispatch(4, 25.7560, 28.2320)
        [ADAPTER] — fire->respond("CAMPUS", Threat::FIRE) →
                    FireFighterAdapter maps "CAMPUS"→buildingNumber=100
                    → calls adaptee->alertStation("CAMPUS", 100), prints ETA of 5 mins
    ↓
    [MEDIATOR] — hub->notify(nullptr, "EMERGENCY_DECLARED") broadcasts to all colleagues:
                 AccessControlTeam unlocks all evacuation turnstiles and emergency exits;
                 FirstAidTeam prepares field trauma unit for incoming casualties;
                 SecurityGuards triggers immediate perimeter lockdown.
```

---

---

# VP-GENERATED CODE FIXES

After generating .h files from VP, fix these before writing any .cpp files:

| File | Problem | Fix |
|---|---|---|
| `CommunicationHub.h` | `notify()` marked `= 0` | Remove `= 0` — it is a concrete override, not pure virtual |
| All four colleague `.h` files | `receive()` marked `= 0` | Remove `= 0` |
| All seven command `.h` files | `execute()` and `undo()` marked `= 0` | Remove `= 0` |
| All four concrete state `.h` files | `escalate()`, `deescalate()`, `getLabel()` marked `= 0` | Remove `= 0` |
| `IncidentState.h` | `~IncidentState()` is `= 0` | Keep `= 0` but add out-of-line definition in `.cpp` |
| `FirstResponder.h` | `~FirstResponder()` is `= 0` | Keep `= 0` but add out-of-line definition in `.cpp` |
| `EmergencyResponder.h` | `~EmergencyResponder()` is `= 0` | Keep `= 0` but add out-of-line definition in `.cpp` |
| `CommunicationTeam.h` | `~CommunicationTeam()` is `= 0` | Keep `= 0` but add out-of-line definition in `.cpp` |
| `IncidentControl.h` | `getState()` returns void | Change to `std::string` |
| `IncidentControl.h` | `createMemento()` returns void | Change to `IncidentMemento*` |
| `IncidentHistory.h` | `pop()` returns void | Change to `IncidentMemento*` |
| `IncidentHistory.h` | `isEmpty()` returns void | Change to `bool` |
| `IncidentMemento.h` | `getActiveThreats()` returns void | Change to `std::map<std::string, Threat>` |
| `IncidentMemento.h` | `getStateLabel()` returns void | Change to `std::string` |
| `IncidentState.h` and all concrete states | `getLabel()` returns void | Change to `std::string` |
| `FacilityStaff.h` | `dispatchMaintanance` spelling | Keep exactly as is — matches UML |
| `FirstAidTeam.h` | `assesInjury` spelling | Keep exactly as is — matches UML |

---

# RUNTIME SCENARIOS

## Scenario 1:

**Title:** Fire in Engineering Block — Full Escalation and Resolution

**Patterns demonstrated:** Facade, State, Memento, Mediator, Command, Adapter

**Story:**
> A fire is detected in Engineering Block A. The system registers the threat, escalates from Moderate to Urgent, isolates the zone, then escalates to full Emergency — dispatching police, ambulance, and fire services. Once the fire is contained, the incident is resolved and all teams stand down.

**Steps in code:**
```
system.reportIncident("Engineering Block A", Threat::FIRE)
system.escalateToEmergency()
system.resolveIncident()
```

**What the tutor should see in the output:**
- `[IncidentControl] Threat added: Engineering Block A — FIRE. ThreatCount: 1.`
- `[State] Moderate → Urgent`
- `[Memento] Snapshot saved: URGENT, threats=1`
- `[Mediator] INCIDENT_REPORTED → SecurityGuards, FirstAidTeam, FacilityStaff, AccessControlTeam`
- `[Command] Isolate executed — Engineering Block A locked down`
- `[Mediator] ZONE_LOCKED → SecurityGuards, FacilityStaff`
- `[State] Urgent → Emergency`
- `[Memento] Snapshot saved: EMERGENCY, threats=1`
- `[Command] EmergencyEscalation executed`
- `[Adapter] PoliceAdapter dispatching — severity=5, code=10-71`
- `[Adapter] AmbulanceAdapter dispatching — caseType=5, lat=25.7560, lon=28.2320`
- `[Adapter] FireFighterAdapter — Building 101 alerted. ETA: 5 mins`
- `[Mediator] EMERGENCY_DECLARED → all colleagues`
- `[Command] Resolve executed — maintenance dispatched, zones unlocked`
- `[State] Emergency → Urgent → Moderate → Resolved`
- `[Memento] Snapshot saved: RESOLVED, threats=0`
- `[Mediator] INCIDENT_RESOLVED → SecurityGuards, AccessControlTeam, FacilityStaff`

---

## Scenario 2:

**Title:** False Alarm — Rollback After Incorrect Escalation

**Patterns demonstrated:** Facade, Command (undo), Memento, State

**Story:**
> A fight is reported in the Library. The system registers the threat and isolates the zone. Before further escalation, it is determined to be a false alarm. The last action is rolled back — the zone is unlocked, the state snapshot is restored, and the threat is removed.

**Steps in code:**
```
system.reportIncident("Library", Threat::FIGHT)
system.addThreat("Library", Threat::FIGHT)
system.rollbackLastAction()
```

**What the tutor should see in the output:**
- `[IncidentControl] Threat added: Library — FIGHT. ThreatCount: 1.`
- `[State] Moderate → Urgent`
- `[Memento] Snapshot saved: URGENT, threats=1`
- `[Mediator] INCIDENT_REPORTED → all colleagues`
- `[Command] Isolate executed — Library locked down`
- `[Mediator] ZONE_LOCKED → SecurityGuards, FacilityStaff`
- `[Rollback] Popping last memento snapshot.`
- `[Memento] Restoring: MODERATE, threats=0`
- `[State] Restored → Moderate`
- `[Command] Isolate undo — Library unlocked`

