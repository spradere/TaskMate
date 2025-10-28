### 🏛️ System Architecture and Isolation

The new TaskMate layer configuration provides a **stronger isolation between system components**
(drivers, SysCore, and tasks) Each layer communicates through **well-defined interfaces**,
preventing direct access to the hardware or core system logic.

User tasks can still **benefit from all system features** —
such as messaging, timing, I/O, and services —
but always through indirect calls via the SysCall, SysServices and HAL layers.
This design significantly improves **stability** and **portability**.
