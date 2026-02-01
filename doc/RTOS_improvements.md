### 🚀 RTOS upgrade

This note describes the technical conditions required for TaskMate to evolve
into a deterministic RTOS.

**Future improvements will focus on achieving predictable timing behaviour by addressing key aspects:**

- **Latency control:** minimising the time between an interrupt event and task resumption.
- **Jitter reduction:** ensuring consistent timing intervals across scheduler cycles.
- **Deterministic preemption:** guaranteeing that high-priority tasks always execute within bounded response times.
- **Protected I/O operations:** introducing mechanisms to temporarily shield critical drivers or sections from preemption.
- **System profiling tools:** adding real-time metrics for CPU load, context-switch latency, and ISR duration.

Once these mechanisms are implemented and validated, TaskMate will evolve into
a **deterministic RTOS** suitable for demanding embedded applications where
timing precision and reliability are critical.
