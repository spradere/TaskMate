# 🧭 Code and files prefixes — One-page guidelines

## 🏷️ In TaskMate, prefixes are used deliberately and sparingly.

- Each prefix corresponds to a **clear conceptual responsibility**
- No prefix overlaps with another
- No prefix is decorative or redundant

---

## 📋 Below is a concise overview of the existing prefixes and their intent.

### ● `tm_` — Project identity

**Everything that is specific to TaskMate itself**

- Acts as a global namespace
- Protects against external symbol collisions
- Serves as a mental anchor: “this code belongs to the TaskMate core”

👉 To be used **sparingly** and only when the project identity matters.

---

### ● `sc_` — Security boundary (system calls)

**Controlled and limited access from tasks to system functionality**

- Exposes a restricted API to user tasks
- Clearly symbolises a privilege boundary

At a glance, it reads as:

> “This code is not the kernel, but it is talking to it.”

---

### ● `gpio_` — Logical-to-physical middleware

**An intermediate semantic layer between software logic and hardware signals**

- Not HAL
- Not application code
- Not a pure driver

👉 Using a dedicated prefix explicitly acknowledges the existence of an
**intermediate semantic layer**, rather than hiding it inside unrelated components.

---

### ● `err_` — Global error system

**Cross-cutting error handling infrastructure**

- Transversal by nature
- Easy to grep
- Ideal for auditing, logging, and diagnostics

Most importantly:

👉 A transversal concern **justifies** a transversal prefix.

---

### ● `mod_` — Module database

**System-wide description of threads and drivers**

- Provides a global view of the system
- Acts as a single source of truth
- Implements a quasi *“kernel metadata”* logic

Using a dedicated prefix avoids ambiguity:

👉 These symbols **describe the system as a whole**, not just one of its parts.

---

### ● `rl_` — Run levels

**System lifecycle management**

- Small and well-defined domain
- Strong conceptual meaning
- Explicit lifecycle logic

👉 A minimal prefix for a **fundamental OS primitive**.

---

### ● `auto_` — Generated files (autoCode)

The `auto_` prefix is **reserved exclusively for generated file names.**

- It does **not apply to symbols in code**
- It is used only for files produced by the `autoCode` tool
- It clearly marks files as **generated artefacts**, not hand-written source code

This prefix exists to make the distinction explicit between:

1. human-maintained code
2. machine-generated code

Unlike other prefixes, `auto_` is not a **conceptual namespace** inside the codebase.
It is a **file-level convention**, used for traceability, tooling, and maintenance clarity.

---

## Summary and why this matters

A prefix is not decoration.

> **A prefix replaces a comment — it must not become noise.**

TaskMate prefixes provide:

- A clear taxonomy
- A short and controlled list
- Strong semantic meaning
- Zero over-engineering
- Real room for future evolution

▶️ **Prefixes name conceptual boundaries**
