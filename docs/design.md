# CampusGuard Architecture & Design Specification

## 1. Executive Summary & System Goals

**CampusGuard** is a multi-pattern, real-time emergency-coordination platform for campus security, medical responders, and facility staff. The system handles incident reports, status transitions, threat containment, building access controls, and external service escalations.

The primary objective of this architecture is to achieve low-coupling, high-cohesion, and zero-overhead C++ execution while fulfilling all constraints of COS 214 Practical 5.

* **Language Standard:** C++11
* **Build & Deployment Environment:** Makefile & Docker (`docker compose up --build`)
* **Architectural Principle:** Single unified application flow (no isolated pattern menus)

---

## 2. Design Pattern Matrix & Role Assignments

The application integrates six Gang of Four (GoF) patterns into a single coordinated pipeline:

| Pattern | System Role | Class Hierarchy / Participants | Primary Responsibility |
| --- | --- | --- | --- |
| **Mediator** | Central Coordination Hub | `CommunicationTeam` (Abstract)<br>

<br>`Dispatcher` (Concrete) | Coordinates two-way communication between responder units without $N \times N$ direct dependencies. |
| **Command** | Action Protocol Requests | `Protocol` (Abstract)<br>

<br>`Evacuate`, `Deescalate`, `EmergencyEscalation`, `Resolve`, `Assist` (Concrete) | Encapsulates responder operations into executable, loggable, and cancellable objects. |
| **Adapter** | External System Bridge | `ExternalService` (Target Interface)<br>

<br>`ExternalPoliceAdapter` (Adapter)<br>

<br>`LegacyPoliceAPI` (Adaptee) | Wraps legacy third-party emergency interfaces to match internal CampusGuard standard calls. |
| **Facade** | Subsystem Entry Point | `CampusEmergencyFacade` | Exposes unified high-level emergency workflows (`handleCampusWideEmergency()`) hiding multi-step subsystem logic. |
| **State** | Incident Lifecycle | `IncidentState` (Abstract)<br>

<br>`ReportedState`, `ModerateState`, `EmergencyState`, `ResolvedState` (Concrete) | Manages status-dependent operation behavior and state progression. |
| **Strategy** | Dynamic Response Algorithms | `ResponseStrategy` (Abstract)<br>

<br>`RapidContainmentStrategy`, `FullCampusEvacuationStrategy`, `SilentSecurityStrategy` (Concrete) | Selects and swaps response/routing algorithms dynamically based on incident keywords and threat levels. |

---

## 3. Defense of Selected Patterns vs. Rejected Alternatives

### Chosen Additional Patterns: State + Strategy

#### State Pattern

* **Problem Solved:** Prevents large, centralised `if`/`else` or `switch` chains for checking status during protocol execution, directly satisfying Rule 8 of the spec.
* **Runtime Value:** Enforces strict state progression (`Reported` → `Moderate` / `Emergency` → `Resolved`). Invalid operations (e.g., executing an emergency protocol on a resolved incident) are rejected cleanly.

#### Strategy Pattern

* **Problem Solved:** Decouples the decision engine from execution code, adhering strictly to the Open/Closed Principle (OCP).
* **Runtime Value:** Allows runtime swapping of dispatch logic based on incident keywords (e.g., switching from `RapidContainment` to `FullCampusEvacuation` during active threat escalation).

### Justification for Rejecting Alternative Patterns

* **Composite Pattern:** Over-engineers incidents into heavy parent-child tree structures. Flat incident containers with lightweight threat vectors eliminate tree-traversal performance overhead.
* **Decorator Pattern:** Wrapping active incidents creates nested object layers, making real-time state tracking and memory cleanups convoluted and error-prone.
* **Observer Pattern:** Replaced by Mediator. Passive broadcasting in emergency coordination leads to uncoordinated event loops; a central Mediator (`Dispatcher`) actively directs response interactions.
* **Abstract Factory / Template Method:** These patterns only address object creation or rigid inheritance loops; they do not aid runtime execution logic during live emergency updates.

---

## 4. Core Domain Modeling: Incident & Threat Grid

To maintain high performance and explicit state transitions, the `Incident` class acts as the Context for both the State and Strategy patterns while holding a lightweight threat matrix.

### Threat Tracking & Deterministic Resolution

```cpp
enum class ThreatType {
    NONE,
    FIRE,
    FIGHT,
    SHOOTING,
    MEDICAL_EMERGENCY
};

struct ThreatLocation {
    std::string zone; // e.g., "Building A - Floor 2"
    ThreatType threat;
    bool isResolved;
};

```

### Deterministic State Transitions

* `Incident` maintains a `std::vector<ThreatLocation>` and an aggregate `threatLevel` integer.
* When sub-threats are marked as resolved (`isResolved = true`), `Incident::evaluateState()` checks remaining threats.
* Only when all registered threats are cleared does the incident automatically transition to `ResolvedState`.

---

## 5. End-to-End Execution Pipeline

A single emergency report triggers the integrated six-pattern workflow:

```plaintext
[Client Call] ──> [CampusEmergencyFacade] 
                          │
                          ▼ (1. Set Incident State & Keywords)
                     [Incident] ──> [EmergencyState]
                          │
                          ▼ (2. Evaluate Keywords & Swap Strategy)
                [ResponseStrategy]
                          │
                          ▼ (3. Pass Protocol Requests)
                    [Dispatcher] (Mediator)
                          │
               ┌──────────┴──────────┐
               ▼                     ▼
      [EvacuateCommand]   [EmergencyEscalationCommand]
               │                     │
               ▼ (Executes)          ▼ (Translates API)
       [SecurityGuards]    [ExternalPoliceAdapter] ──> [Legacy Police API]

```

1. **Facade Entry:** `CampusEmergencyFacade` receives an incident trigger from the client.
2. **State Transition:** `Incident` transitions to `EmergencyState` based on reported threat severity.
3. **Strategy Selection:** Context keywords (`FIRE`, `SHOOTING`) select `FullCampusEvacuationStrategy`.
4. **Mediator Coordination:** Strategy issues protocols to `Dispatcher` (Mediator), coordinating `SecurityGuards`, `FirstAidTeam`, and `FacilityStaff` without direct cross-team references.
5. **Command Execution:** `Dispatcher` invokes concrete `Protocol` commands (`Evacuate`, `EmergencyEscalation`).
6. **Adapter Bridging:** `EmergencyEscalation` routes off-campus requests through `ExternalPoliceAdapter` to communicate with external municipal systems.

---

## 6. Memory Ownership & C++ Standard Compliance

### Rules & Lifetime Management

* **Virtual Destructors:** All base classes (`CommunicationTeam`, `FirstResponder`, `Protocol`, `IncidentState`, `ResponseStrategy`, `ExternalService`) must declare `virtual ~BaseClass() = default;` to prevent undefined deletion behavior and Valgrind leaks.
* **Composition (◆) vs. Association (→):**
* `Incident` owns its current `IncidentState` and `ResponseStrategy` (Composition: deletes on transition/destruction).
* `Dispatcher` owns queued/active `Protocol` objects (Composition).
* `Dispatcher` holds non-owning pointers (Association) to `FirstResponder` teams (`FirstResponders*`). Do not call `delete` on responders inside the Mediator.


* **Circular Reference Prevention:** `FirstResponder` colleagues reference the Mediator using a non-owning pointer/reference to avoid reference loops.

---

## 7. Submission Deliverables Checklist

* [ ] **UML Diagram Portfolio:**
* [ ] 1 Complete System Class Diagram (with stereotypes `<<Mediator>>`, `<<Colleague>>`, etc.)
* [ ] 1 Sequence Diagram: Command + Mediator interaction flow
* [ ] 1 Sequence Diagram: Facade + Adapter workflow
* [ ] 1 Behavioral Diagram (State Diagram for `Incident` or Activity Diagram for `ResponseStrategy`)


* [ ] **Build & Environment Artifacts:**
* [ ] Makefile compiling with `-std=c++11`
* [ ] Dockerfile & `docker-compose.yml` working with `docker compose up --build`


* [ ] **Verification & Debugging:**
* [ ] Valgrind memory log demonstrating zero leaks
* [ ] GDB session investigation document