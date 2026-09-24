# CampusGuard 2026 — Design Rationale & Pattern Documentation

## System Overview

CampusGuard is a campus incident-response coordination system. It manages the full lifecycle
of a security or medical incident — from the moment a threat is reported, through escalation
and external service dispatch, to resolution and rollback. The system integrates on-campus
first responders (SecurityGuards, FirstAidTeam, FacilityStaff, AccessControlTeam) with
external emergency services (Ambulance, Police, FireFighter) through a unified command and
communication architecture.

The design uses six GoF patterns: Command, Mediator, Adapter, Facade, State, and Memento.
Each pattern was chosen to solve a specific coupling or complexity problem in the domain.
They do not operate independently — a single incident workflow touches all six.

---

## Pattern 1: Command

### Design Problem Solved

Incident-response actions need to be first-class objects. A tutor asking a guard to evacuate
a building and a system event triggering an area lockdown are both actions — they must be
queued, logged, and reversed without the invoker (Dispatcher) needing to know what each
action does. Without Command, the Dispatcher would need a separate method for every possible
action, creating tight coupling between the invoker and every receiver in the system.

### Why Command Over a Simpler Alternative

A simpler alternative would be to have the Dispatcher call methods directly on receivers:
`guards->clearBuilding()`, `access->lockdownZone()`. This works for one action but becomes
unmanageable as the number of actions grows, makes undo impossible without a tangled stack
of booleans, and couples the Dispatcher to every receiver class. Command encapsulates each
action as an object, making undo a natural consequence of storing the object after execution.

### GoF Participants in This Design

| GoF Role | CampusGuard Class |
|---|---|
| Command (interface) | `Protocol` |
| Invoker | `Dispatcher` |
| ConcreteCommand | `Evacuate`, `Deescalate`, `EmergencyEscalation`, `Resolve`, `Assist`, `GrantAccess`, `Isolate` |
| Receiver | `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`, `EmergencyResponder` |

### How Each Concrete Command Works

**`Evacuate`**
Receivers: `SecurityGuards`, `FacilityStaff`, `AccessControlTeam`
Justification: Evacuating a building requires guards to physically clear the space, facility
staff to secure entry points, and access control to open emergency exits. All three must act
in coordination — this command orchestrates all three in `execute()`.

**`Deescalate`**
Receivers: `SecurityGuards`
Justification: De-escalation is a guard-only action — positioning personnel and issuing
verbal warnings to contain a minor disturbance before it grows. Only SecurityGuards are
qualified and dispatched for this.

**`EmergencyEscalation`**
Receivers: `SecurityGuards`, `FacilityStaff`, `AccessControlTeam`, `FirstAidTeam`,
`EmergencyResponder` (police, ambulance, fire — via adapters)
Justification: This is the most critical command — it activates every internal responder
AND contacts all three external services. This is where Command and Adapter intersect:
`execute()` calls `police->respond()`, `ambulance->respond()`, `fire->respond()` through
the `EmergencyResponder` target interface. The command does not know it is talking to adapters.

**`Resolve`**
Receivers: `FacilityStaff`, `AccessControlTeam`
Justification: Resolving an incident means restoring normal operations — unlocking zones,
dispatching maintenance, and broadcasting the all-clear. Guards are not needed; this is
an administrative and facilities action.

**`Assist`**
Receivers: `FirstAidTeam`, `AccessControlTeam`
Justification: Medical assistance requires medics to be deployed AND access control to open
medical zones. If medics assess the injury as critical, they call `escalateSituation()`
internally, which signals the mediator — the Facade then issues an `EmergencyEscalation`.

**`GrantAccess`**
Receivers: `AccessControlTeam`
Justification: Granting access to a specific zone for a specific role is a targeted,
reversible action. `undo()` calls `revokeAccess()` with the same zone and role.
This command is used when first responders need access to areas normally restricted.

**`Isolate`**
Receivers: `AccessControlTeam`
Justification: Isolating a dangerous area from civilians is the inverse of GrantAccess —
it locks a zone and broadcasts the restriction. Inside `lockdownZone()`, the
AccessControlTeam calls `changed("ZONE_LOCKED")`, triggering the mediator to notify
FacilityStaff to secure adjacent entry points.

### Key Design Decision: Commands Don't Own Receivers

Every concrete command stores raw pointers to its receivers but does NOT delete them in
its destructor. The Facade owns all receiver objects. Commands are created, executed, and
either undone or destroyed by the Dispatcher — receivers outlive all commands.

### Key Design Decision: `Protocol::undo()` is Pure Virtual

Every command must be reversible. Making `undo()` pure virtual enforces this at compile
time. A command that genuinely cannot be undone (e.g., a dispatched ambulance) still
implements `undo()` — it prints a log message explaining why the reversal is partial.
Silent no-ops are not permitted.

---

## Pattern 2: Mediator

### Design Problem Solved

The four on-campus first responders need to react to each other's actions without knowing
about each other. Without a mediator, `SecurityGuards::clearBuilding()` would need to
directly call `FacilityStaff::securePremises()` and `AccessControlTeam::lockdownZone()`,
creating a many-to-many dependency web. Adding a fifth responder type would require
modifying every existing class. The mediator centralises all inter-colleague communication.

### Why Mediator Over Direct Coupling

Direct coupling between colleagues means every class depends on every other. Four responders
produce up to twelve directed dependencies. With a mediator there are four — each colleague
knows only the hub. This also means the hub's `notify()` method is the single place where
"when X happens, Y and Z should react" logic lives, making it easy to audit and modify.

### GoF Participants in This Design

| GoF Role | CampusGuard Class |
|---|---|
| Mediator (interface) | `CommunicationTeam` |
| ConcreteMediator | `CommunicationHub` |
| Colleague (abstract) | `FirstResponder` |
| ConcreteColleague | `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam` |

### How the Mediator Coordinates

When a colleague's state changes in a domain-significant way, it calls:
```
this->changed("EVENT_STRING");
```
`changed()` is defined on `FirstResponder` and is NOT virtual — it always does the same
thing: forward to `hub->notify(this, event)`. The hub iterates all registered responders,
skips the sender, and calls `receive(event)` on everyone else. Each concrete colleague
implements `receive()` differently — this is the pure virtual method that varies.

**Example flow — `Isolate` command executes:**
1. `Isolate::execute()` calls `access->lockdownZone("Engineering Block A")`
2. Inside `lockdownZone()`, after locking: `changed("ZONE_LOCKED")`
3. `FirstResponder::changed()` calls `hub->notify(access, "ZONE_LOCKED")`
4. `CommunicationHub::notify()` skips `access` (the sender), calls `receive("ZONE_LOCKED")`
   on `guards`, `medics`, and `facility`
5. `FacilityStaff::receive("ZONE_LOCKED")` → `securePremises()`
6. The other two acknowledge and log

This is a real domain collaboration triggered by one command — not a demonstration pattern.

### Key Design Decision: `changed()` is Not Virtual

`changed()` must always forward to the hub without variation — making it virtual would
allow a subclass to override it and break the mediator contract. Only `receive()` is virtual
because reacting to notifications is what differs between responder types.

### Key Design Decision: Hub Does Not Own Responders

`CommunicationHub` stores `std::vector<FirstResponder*>` but does not delete them. The Facade
owns all responders. The hub's destructor is trivial. This prevents double-delete and makes
ownership explicit and traceable.

---

## Pattern 3: Adapter

### Design Problem Solved

CampusGuard needs to contact three external emergency services — an ambulance dispatch
system, a police communications system, and a fire department alert system. Each has its
own incompatible API designed independently of CampusGuard. The system cannot modify these
external interfaces. Without adapters, `EmergencyEscalation` would need to know about GPS
coordinates, police incident codes, and fire department building numbers — tightly coupling
a core domain class to three external implementation details.

### Why Adapter Over Direct Integration

If `EmergencyEscalation` called `ambulance->dispatch(lat, lon, caseType)` directly, the
command would need to know how to convert a campus location string to GPS coordinates. It
would also become impossible to swap the ambulance service for a different provider without
modifying `EmergencyEscalation`. The adapter isolates the translation logic — the command
only calls `respond(location, threat)` on an `EmergencyResponder*`.

### GoF Participants in This Design

| GoF Role | CampusGuard Class |
|---|---|
| Target (interface) | `EmergencyResponder` |
| Adapter | `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter` |
| Adaptee | `Ambulance`, `Police`, `FireFighter` |
| Client | `EmergencyEscalation` (command), `EmergencyResponseFacade` |

### The Interface Mismatch — Why Each Adapter is Justified

**`AmbulanceAdapter`**
- Target expects: `respond(const std::string& location, Threat threat)`
- Adaptee provides: `dispatch(double lat, double lon, int caseType)`
- Translation: Location string → GPS coordinates (campus building lookup table).
  `Threat` enum → integer case type (1=trauma, 2=cardiac, 3=general).

**`PoliceAdapter`**
- Target expects: `respond(const std::string& location, Threat threat)`
- Adaptee provides: `requestBackup(const std::string& incidentCode, int priority)`
- Translation: `Threat` enum → standard police 10-code string
  (e.g., `SHOOTING` → `"10-71"`, `FIRE` → `"10-70"`).
  `Threat` severity → integer priority (1=highest, 3=lowest).

**`FireFighterAdapter`**
- Target expects: `respond(const std::string& location, Threat threat)`
- Adaptee provides: `alertStation(const std::string& location, const std::string& buildingName, int buildingNumber)`
- Translation: Location string → building name string + integer building number
  (campus building registry lookup).

### Key Design Decision: Adapters Own Their Adaptees

Each adapter takes a raw `Adaptee*` in its constructor and deletes it in its destructor.
The Facade creates adapters as: `new AmbulanceAdapter(new Ambulance())`. The `Ambulance`
is created solely to be wrapped — it has no independent existence in the system. The adapter
is the single owner. When the Facade deletes the adapter, the adaptee is also deleted.

### Key Design Decision: `EmergencyResponder` Methods are Pure Virtual

`respond()` and `getStatus()` are pure virtual on `EmergencyResponder`. This forces every
adapter to provide a real translation implementation — a forgotten `respond()` override
would be a compile error, not a silent no-op.

---

## Pattern 4: Facade

### Design Problem Solved

Without a Facade, `main.cpp` would need to construct and coordinate a Dispatcher,
IncidentControl, CommunicationHub, four colleagues, three adapters with their adaptees, and
an IncidentHistory — in the correct order, with correct ownership. A single incident report
would require the client to call `controller->addThreat()`, `hub->notify()`,
`dispatcher->issueCommand(new Isolate(...))` — knowing about six subsystems to perform one
logical operation. The Facade reduces this to `system.reportIncident(location, threat)`.

### Why Facade Over Exposing Subsystems

The alternative is to let `main.cpp` coordinate everything directly. This means `main.cpp`
becomes a god function that knows the internal structure of the entire system. Any change
to the subsystem interaction sequence requires modifying `main.cpp`. The Facade contains
this coordination knowledge in one place — subsystems remain independently usable (you can
still call `dispatcher->issueCommand()` directly if needed), but the common workflows are
one-liners.

### GoF Participants in This Design

| GoF Role | CampusGuard Class |
|---|---|
| Facade | `EmergencyResponseFacade` |
| Subsystems | `Dispatcher`, `IncidentControl`, `CommunicationHub`, `SecurityGuards`, `FirstAidTeam`, `FacilityStaff`, `AccessControlTeam`, `AmbulanceAdapter`, `PoliceAdapter`, `FireFighterAdapter`, `IncidentHistory` |

### Facade Operations — Subsystems Coordinated per Method

**`reportIncident(location, threat)`** — 4 subsystems:
1. `IncidentControl::addThreat()` — registers threat, triggers state escalation
2. `IncidentHistory::push(controller->createMemento())` — snapshots state before response
3. `CommunicationHub::notify(nullptr, "INCIDENT_REPORTED")` — mediator broadcasts to all colleagues
4. `Dispatcher::issueCommand(new Isolate(...))` — issues containment command

**`escalateToEmergency()`** — 4 subsystems:
1. `IncidentControl::escalate()` — forces state upward
2. `IncidentHistory::push(controller->createMemento())` — snapshots pre-escalation state
3. `Dispatcher::issueCommand(new EmergencyEscalation(...))` — triggers all responders + all adapters
4. `CommunicationHub::notify(nullptr, "EMERGENCY_DECLARED")` — broadcast

**`resolveIncident()`** — 4 subsystems:
1. `Dispatcher::issueCommand(new Resolve(...))` — restore access, dispatch maintenance
2. `IncidentControl::clearThreats()` — clears all threats, triggers Resolved state
3. `IncidentHistory::push(controller->createMemento())` — snapshots resolved state
4. `CommunicationHub::notify(nullptr, "INCIDENT_RESOLVED")` — broadcast

**`rollbackLastAction()`** — 3 subsystems:
1. `IncidentHistory::pop()` — retrieves last snapshot
2. `IncidentControl::restore(memento)` — restores state and threat map
3. `Dispatcher::undoLast()` — undoes last command

### Key Design Decision: Facade is the Composition Root

The Facade constructs every object in the system in its constructor and destroys every
object in its destructor. `main.cpp` only creates and destroys a `EmergencyResponseFacade`.
This makes the entire object graph's lifetime deterministic — one Facade created, one
Facade destroyed, all memory accounted for. Valgrind will show zero leaks.

---

## Pattern 5: State

### Design Problem Solved

An incident passes through distinct severity phases — Moderate, Urgent, Emergency, Resolved.
The behaviour of `IncidentControl` differs fundamentally in each phase: what commands should
be issued, what constitutes a valid operation, how to respond to a new threat. Without State,
`IncidentControl` would use a cascade of `if/else` or `switch` statements checking an enum,
spreading state-specific logic throughout the class. Adding a new state (e.g., `Contained`)
would require modifying every existing branch.

### Why State Over an Enum + Switch

An enum approach means `IncidentControl::escalate()` looks like:
```cpp
if (currentState == MODERATE) { ... }
else if (currentState == URGENT) { ... }
```
This is duplicated in `escalate()`, `deescalate()`, `addThreat()`, and every method that
depends on current state. The State pattern moves each state's behaviour into its own class.
`IncidentControl` calls `currentState->escalate(this)` — it never conditionally branches
on state. Adding `Contained` means adding one class, not modifying twelve methods.

### GoF Participants in This Design

| GoF Role | CampusGuard Class |
|---|---|
| Context | `IncidentControl` |
| State (abstract) | `IncidentState` |
| ConcreteState | `Moderate`, `Urgent`, `Emergency`, `Resolved` |

### State Transition Logic

States drive their own transitions by calling `ctx->setState(new NextState())`. The Context
never decides what comes next — the current state does. This is the critical design insight:

| Current State | `escalate()` | `deescalate()` |
|---|---|---|
| `Moderate` | → `Urgent` | → `Resolved` (if no threats) |
| `Urgent` | → `Emergency` | → `Moderate` |
| `Emergency` | **INVALID** — logs warning, no transition | → `Urgent` |
| `Resolved` | → `Moderate` (incident re-opens) | **INVALID** — logs warning |

`Emergency::escalate()` and `Resolved::deescalate()` are the required invalid-operation
cases handled sensibly — they print a descriptive warning and do nothing, rather than
crashing or silently corrupting state.

### Key Design Decision: `setState()` Deletes the Old State

When a transition occurs, `IncidentControl::setState(new NextState())` deletes the previous
`IncidentState*` before replacing it. This means state objects have a lifetime exactly equal
to the period they are active. No state lingers. No double-delete is possible because
`currentState` is always replaced atomically.

### Key Design Decision: `IncidentControl` Also Serves as Originator (Memento)

`IncidentControl` is both the State Context and the Memento Originator. This is intentional
and correct — both roles concern the same data (current state, threat count, active threats).
Splitting them into two classes would require either duplicating the data or creating
unnecessary coupling between two classes that always need the same information.

---

## Pattern 6: Memento

### Design Problem Solved

CampusGuard must support rollback — if an emergency escalation turns out to be a false
alarm, the system must be able to restore the exact incident state that existed before the
escalation, including the threat map and the severity level. Without Memento, rollback would
require either re-running the scenario from the start, or exposing `IncidentControl`'s
private data to an external class that stores it — violating encapsulation.

### Why Memento Over Exposing State Externally

The alternative is to have `IncidentHistory` store copies of `IncidentControl`'s data
directly — but this requires `IncidentHistory` to know about the internal structure of
`IncidentControl`. If `IncidentControl` adds a new field, `IncidentHistory` must also be
updated. Memento keeps the snapshot opaque to the Caretaker — only the Originator knows
how to read it back.

### GoF Participants in This Design

| GoF Role | CampusGuard Class |
|---|---|
| Originator | `IncidentControl` |
| Memento | `IncidentMemento` |
| Caretaker | `IncidentHistory` |

### What the Memento Stores

`IncidentMemento` stores three value copies — not pointers:
- `int threatCount` — copy of the count at snapshot time
- `std::map<std::string, Threat> activeThreats` — full copy of the threat map
- `std::string stateLabel` — the string label of the current state ("MODERATE" etc.)

**Why a string label and not `IncidentState*`:**
If we stored `IncidentState* currentState` in the memento, we would be storing a pointer
to the live state object. When `IncidentControl` transitions to Emergency and deletes the
Urgent object, the memento's pointer becomes dangling. Restoring from it is undefined
behaviour. The string label is a stable, copyable representation. `restore()` reconstructs
the correct `IncidentState` object from the label using `new Moderate()` etc.

### Ownership of Memento Pointers

Since mementos are kept as pointers (as preferred):

```
createMemento() → allocates IncidentMemento* → Facade receives pointer
Facade → history->push(ptr) → IncidentHistory takes ownership
history->pop() → returns ptr, transfers ownership to Facade
Facade → controller->restore(ptr) → IncidentControl reads + deletes ptr
```

Every `new IncidentMemento` has exactly one matching `delete` in `IncidentControl::restore()`.
`IncidentHistory::~IncidentHistory()` deletes any mementos that were never popped
(e.g., if the program ends while snapshots remain).

### Key Design Decision: Caretaker Never Inspects the Memento

`IncidentHistory::push()` stores the pointer. `IncidentHistory::pop()` returns it.
`IncidentHistory` never calls any getter on `IncidentMemento`. All interpretation of memento
contents is done exclusively in `IncidentControl::restore()`. This is the fundamental Memento
contract — the Caretaker is opaque to the memento's contents.

### Invalid Operation Case

`IncidentHistory::pop()` when the stack is empty returns `nullptr` with a warning message.
`EmergencyResponseFacade::rollbackLastAction()` checks for `nullptr` before calling `restore()`.
`IncidentControl::restore()` also guards against `nullptr`. Two defensive layers ensure
no crash and a clear user-visible warning.

---

## How the Six Patterns Interact

The patterns do not operate independently. A single incident workflow exercises all six:

```
main() calls: system.reportIncident("Engineering Block A", Threat::FIRE)
│
├─ [FACADE] EmergencyResponseFacade::reportIncident()
│     │
│     ├─ [STATE] IncidentControl::addThreat() → escalate()
│     │               → Moderate::escalate() → ctx->setState(new Urgent())
│     │
│     ├─ [MEMENTO] history->push(controller->createMemento())
│     │               → IncidentMemento* allocated, snapshot of Urgent state
│     │
│     ├─ [MEDIATOR] hub->notify(nullptr, "INCIDENT_REPORTED")
│     │               → SecurityGuards::receive() → moves to location
│     │               → FirstAidTeam::receive() → prepares kit
│     │               → AccessControlTeam::receive() → prepares for lockdown
│     │
│     └─ [COMMAND] dispatcher->issueCommand(new Isolate(access, "Engineering Block A"))
│                     → Isolate::execute()
│                     → access->lockdownZone("Engineering Block A")
│                     → [MEDIATOR] access->changed("ZONE_LOCKED")
│                     → hub->notify(access, "ZONE_LOCKED")
│                     → FacilityStaff::receive("ZONE_LOCKED") → securePremises()
│
main() calls: system.escalateToEmergency()
│
├─ [FACADE] EmergencyResponseFacade::escalateToEmergency()
│     │
│     ├─ [STATE] IncidentControl::escalate()
│     │               → Urgent::escalate() → ctx->setState(new Emergency())
│     │
│     ├─ [MEMENTO] history->push(controller->createMemento())
│     │               → snapshot of Emergency state saved
│     │
│     ├─ [COMMAND] dispatcher->issueCommand(new EmergencyEscalation(...))
│     │     └─ EmergencyEscalation::execute()
│     │           ├─ access->lockdownZone(location)     [internal receiver]
│     │           ├─ guards->clearBuilding()            [internal receiver]
│     │           ├─ medics->escalateSituation()        [internal receiver]
│     │           ├─ [ADAPTER] police->respond(...)
│     │           │     └─ PoliceAdapter::respond()
│     │           │           → threatToIncidentCode(FIRE) → "10-70"
│     │           │           → adaptee->requestBackup("10-70", 1)
│     │           ├─ [ADAPTER] ambulance->respond(...)
│     │           │     └─ AmbulanceAdapter::respond()
│     │           │           → locationToCoords("Engineering Block A") → lat/lon
│     │           │           → adaptee->dispatch(lat, lon, 3)
│     │           └─ [ADAPTER] fire->respond(...)
│     │                 └─ FireFighterAdapter::respond()
│     │                       → extractBuildingName() → "Engineering Block"
│     │                       → adaptee->alertStation(..., "Engineering Block", 14)
│     │
│     └─ [MEDIATOR] hub->notify(nullptr, "EMERGENCY_DECLARED")
│                     → all colleagues receive and react
│
main() calls: system.rollbackLastAction()
│
└─ [FACADE] EmergencyResponseFacade::rollbackLastAction()
      ├─ [MEMENTO] history->pop() → returns Emergency snapshot pointer
      ├─ [STATE+MEMENTO] controller->restore(snapshot)
      │                   → reads label "EMERGENCY" → new Emergency()
      │                   → restores threatCount and activeThreats
      │                   → deletes snapshot pointer
      └─ [COMMAND] dispatcher->undoLast()
                    → pops EmergencyEscalation from history
                    → EmergencyEscalation::undo() → unlocks zones, stands down
                    → delete cmd
```

---

## Ownership Policy Summary

| Object | Owner | Lifetime |
|---|---|---|
| All `FirstResponder` subclasses | `EmergencyResponseFacade` | Entire program lifetime |
| `CommunicationHub` | `EmergencyResponseFacade` | Entire program lifetime |
| `Dispatcher` | `EmergencyResponseFacade` | Entire program lifetime |
| `IncidentControl` | `EmergencyResponseFacade` | Entire program lifetime |
| `IncidentHistory` | `EmergencyResponseFacade` | Entire program lifetime |
| All Adapter objects | `EmergencyResponseFacade` | Entire program lifetime |
| All Adaptee objects | Their respective Adapter | As long as adapter lives |
| `Protocol*` commands | `Dispatcher` | Until undone or Dispatcher destroyed |
| `IncidentState*` | `IncidentControl` | Until next transition or controller destroyed |
| `IncidentMemento*` | `IncidentHistory` while stored, then `IncidentControl::restore()` | Until `restore()` is called |

One rule governs all of the above: **every `new` has exactly one matching `delete`,
traceable through the ownership chain without any shared pointers or ambiguous responsibility.**

---

## Additional Design Decisions

### Forward Declarations Over Includes in Headers

Every header uses forward declarations (`class SecurityGuards;`) instead of including other
headers where the full class definition is not needed. Full includes only appear in `.cpp`
files. This prevents circular include chains (particularly important between `IncidentControl`
and `IncidentState`, which each reference the other) and reduces compilation time.

### `= default` Destructors on Concrete Leaf Classes

Concrete commands, concrete states, and concrete colleagues use `~ClassName() override = default`
rather than defining an empty destructor body. This is semantically identical but explicitly
signals that the destructor is intentionally trivial — the class does not own any heap
resources that need manual cleanup.

### String Events in the Mediator

The mediator uses `std::string` event tokens (`"ZONE_LOCKED"`, `"EMERGENCY_DECLARED"`) rather
than an enum or integer code. This trades a small runtime overhead for complete extensibility —
adding a new event requires no changes to `CommunicationTeam` or `CommunicationHub`. Any
colleague can signal any event string and any colleague can react to any subset of strings
in its `receive()` implementation.

### The Facade Registers Colleagues with the Hub

Colleagues are constructed before the hub registers them — `hub->registerResponder(guards)`
is called explicitly in the Facade constructor after all colleagues are built. This order
dependency is intentional and documented. If a colleague were constructed after registration,
the hub would hold a dangling pointer during the window between construction and registration.
The Facade controls this order, making it a single point of correctness.
