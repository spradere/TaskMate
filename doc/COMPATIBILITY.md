# COMPATIBILITY.md

## 🎯 Purpose

This document defines the compatibility policy for the embedded system **TaskMate OS**
to ensure long-term maintainability, stable upgrades, and a predictable developer experience.

---

## 1. 🚧 Scope

The following rules apply to these system components:

- Public API (functions accessible to user tasks)
- System message formats (e.g., task scheduler, error codes, IPC)
- System calls (`sysCallXXX()`)
- Driver interfaces (`driverX_init/start/stop`)
- Exposed data structures
- Initialization scripts (`*_init.rc`, used by autoCode)
- Serial CLI protocol

**Not directly covered** by this policy:

- Internal kernel code (`sysCore/`)
- Platform-specific implementation files (`AVR/`, `x86/`)
- Private driver internals (as long as interface stays stable)
- Experimental modules not documented or flagged as unstable

---

## 2. ♻️ Compatibility Policy

### 2.1 Public API (User Task Layer)

- New functions may be added freely if non-breaking.
- Changing an existing function signature is **forbidden** without a major version bump.
- Deprecated functions must be marked clearly and retained for **at least two minor versions**.

### 2.2 System Calls (`sysCallXXX()`)

- Names and signatures **must not change** in minor versions.
- Deprecated system calls must:
  - be marked as such in headers
  - return safely or redirect to a neutral fallback

### 2.3 Exposed Structures

- Modifications to shared structs (`task_t`, `module_t`, etc.) must be:
  - versioned (e.g., `task_v2_t`)
  - or extended in a non-breaking way (add fields at the end, use paddings)

### 2.4 Message & Text Protocols

- System messages (`MSG_XXX`) must be stable across minor versions.
- Any format change must:
  - include a version field
  - or be auto-detected and backward-compatible

### 2.5 Initialization Scripts (`*_init.rc`)

- All keywords are **reserved and documented**.
- Removing a keyword is not allowed unless:
  - an alias is provided
  - it fails gracefully on older systems
- Adding new keywords is allowed if non-breaking.

### 2.6 Serial CLI Protocol

- CLI commands and options must remain stable whenever possible.
- Deprecated commands must suggest replacements.
- A version command (`--version`, `--compat-level`) must be available for tooling.

---

## 3. 🏷️ Versioning Scheme

- Uses **MAJOR.MINOR.PATCH** inspired by [SemVer](https://semver.org/)
- A **MAJOR** version change (e.g., 1.x → 2.x) allows for breaking changes.
- **MINOR** versions (e.g., 1.4 → 1.5) must be backward compatible.
- **PATCH** versions must only include bug fixes, no new features or behavioral changes.

---

## 4. 🔃 Rollback & Migration

- Each stable release must support rollback to the previous version without loss of functionality (on compatible hardware).
- Configuration scripts (`*.rc`) must remain readable by the previous release when possible.
- If not possible, provide a **migration tool or script**.

---

## 5. 🛠️ Compatibility Testing

Each stable build must include regression tests for:

- Loading tasks built with version N–1
- Parsing `*.rc` scripts written for N–1
- Interacting with CLI tools from the previous version

---

## 6. ✍️ Revision History

| Version | Date       | Changes                                 |
|---------|------------|------------------------------------------|
| 1.0     | 2025-05-28 | Initial version of the compatibility charter |

---

## 7. ✒️ Contact / Changes

This document is part of the official TaskMate repository.
Any modification must:

- be discussed among contributors,
- be mentioned in the changelog,
- trigger a major version update if breaking.

