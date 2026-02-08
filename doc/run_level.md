# 🔀 Run Levels

---

This note briefly describes the run level mechanism used by TaskMate
to control system initialisation and runtime activation of modules.

The system implements run levels to control and sequence the initialisation
of modules during system startup. Each module is assigned a run level according to its role:

- **RUN_NONE**: Not started automatically; can be manually launched later via the system CLI.
- **RUN_CORE**: Start only the minimal critical components required for the system to function safely.
- **RUN_DRIVER**: Initialise hardware drivers needed by higher-level services and tasks.
- **RUN_SERVICE**: Launch system services that depend on drivers but are still internal to the OS.
- **RUN_USER**: Start user tasks.

This mechanism is crucial for maintaining a deterministic and controlled startup sequence,
ensuring that dependencies are properly satisfied before launching higher-level components.
It also allows dynamic system management by enabling selective start/stop operations at runtime,
enhancing flexibility and robustness, especially for debugging, recovery, and partial system restarts.
