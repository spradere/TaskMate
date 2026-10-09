# TaskMate Embedded C++ Rules

This guide defines the use of C++20 in TaskMate embedded system code. C++ is an optional implementation tool. It is accepted only when it provides a zero-cost abstraction and preserves the existing architecture, deterministic behaviour, and C interoperability.

## Scope

- C++20 may be used in system and HAL implementation code when it gives a clear compile-time or type-safety benefit.
- C remains a supported implementation language for every layer.
- Application tasks are not required to use C++. A target or task may remain entirely in C.
- Introducing C++ into one component must not force unrelated components, services, tasks, generated code, or targets to migrate to C++.
- C++ does not relax layer ownership, dependency rules, interrupt constraints, or the static system design.

## Zero-cost requirement

- Every C++ abstraction must have a cost equivalent to a direct, suitable C implementation for the selected target.
- Source-level elegance is not proof of zero cost. Review the linked image, map, symbols, and disassembly when an abstraction can affect flash, RAM, stack, timing, or interrupt latency.
- C++ abstractions must not add dynamic dispatch, hidden allocation, hidden initialization, implicit registration, unbounded work, or an unexpected runtime-library dependency.
- A zero-cost abstraction must add no generic dispatch state, runtime type data, constructor table, destructor table, guard variable, or persistent state that an equivalent direct implementation does not need.
- Template use must not cause uncontrolled code duplication. Share a non-template implementation when specializations do not require distinct generated code.
- Give repeated template instantiations one reviewed owner when this is needed to prevent duplicate emitted code.
- Prefer compile-time validation and static selection over runtime policy objects or generic dispatch state.
- Reject an abstraction when its generated code or worst-case behaviour cannot be explained and bounded on the reference MCU.
- Zero cost does not require byte-identical output. Any difference in flash, static RAM, stack, execution time, or interrupt latency must be measured, explained, bounded, and accepted for the target.
- Keep a direct implementation or a focused binary reference case when a new abstraction needs an objective cost comparison.

## Approved C++20 features

- Use `constexpr` for compile-time configuration, validation, calculations, and construction of immutable values.
- Use `static_assert` to reject invalid mappings, ranges, type relationships, and target configurations during compilation.
- Use strong types to prevent accidental mixing of identifiers, units, addresses, pins, bit positions, and other distinct domains.
- Keep value-like strong types small, trivially copyable, trivially destructible, and explicit at conversion boundaries.
- Strong types must not use inheritance, hidden state, implicit conversion to their underlying type, or constructors with side effects.
- Use templates for static reuse and specialization when they remove duplication without adding runtime machinery.
- Use concepts to express the minimum requirements of templates and to produce clear compile-time failures.
- Keep concepts and implementation templates private to the owning C++ component unless a reviewed system-level contract requires wider visibility.
- Use explicit functions and explicit state transitions for driver life cycle, interrupt control, scheduling, hardware access, and error reporting.

## Prohibited features and mechanisms

- Do not use RAII. Object construction or destruction must not acquire, release, restore, start, stop, lock, unlock, or otherwise control a resource or system state.
- Do not use exceptions. Compile C++ code with exceptions disabled, and use the existing explicit error contracts.
- Do not use RTTI, `dynamic_cast`, or `typeid`.
- Do not use `malloc`, `calloc`, `realloc`, `free`, `new`, or `delete`, including placement forms and user-defined allocation functions.
- Do not use virtual functions, virtual inheritance, or other implicit runtime polymorphism.
- Do not use global constructors, global destructors, function-local static initialization guards, or any other dynamic initialization before `main`.
- Do not use standard-library containers, strings, streams, smart pointers, `std::function`, or algorithms that allocate or hide unbounded work.
- The initial standard-library allowlist contains only C compatibility headers already accepted by the project. Add a C++ header or facility only after its implementation, runtime dependencies, memory use, timing, and availability have been validated for every affected toolchain.
- Do not use C++20 modules, coroutines, ranges, concurrency facilities, or library containers without a separate rule and an explicit architectural need.

## C and C++ boundary

- `srcs/interfaces/` must remain neutral between C and C++. Its headers and generated contracts must compile as both C and C++.
- Interfaces must expose C-compatible fixed-width types, enums, constants, structures, opaque handles, and function signatures.
- Interfaces must not expose classes, templates, concepts, references, overloaded functions, constructors, destructors, namespaces, or C++ standard-library types.
- `extern "C"` is authorized only as a C++ language-linkage specifier for functions at an intentional C/C++ ABI boundary.
- Shared headers must guard C++ linkage with `#ifdef __cplusplus` so the same declarations remain valid C.
- The `extern "C"` exception does not authorize externally linked objects, ordinary `extern` variable declarations, shared mutable state, or C++ types in an interface.
- In C code, variables with external linkage remain prohibited. File-local state must continue to use `static` and follow the existing ownership rules.
- A C++ implementation must convert between its private strong types and neutral interface types at one explicit boundary.
- C++ linkage details and compiler-specific declarations must not leak into generated records or portable data contracts.
- C++ implementation and policy headers belong to the component that owns them. They must not be placed in `srcs/interfaces/` merely to make them widely reachable.
- Mixed-language boundaries must preserve the existing ownership and dependency direction. They must not bypass build-enforced include rules.

Use this form for a shared function declaration:

```c
#ifdef __cplusplus
extern "C" {
#endif

void hal_exampleControl(uint8_t command);

#ifdef __cplusplus
}
#endif
```

## Low-level and deterministic behaviour

- C++ wrappers around hardware must preserve every required `volatile` read and write. They must not cache, combine, duplicate, or hide register accesses.
- Do not present runtime hardware effects as constant evaluation. `constexpr` may validate addresses and masks, but register access remains explicit runtime work.
- Apply the existing fixed-width integer rules. Make promotions, signedness changes, narrowing conversions, shifts, and mask widths explicit and reviewable.
- Do not hide interrupt enable or disable operations, atomic sections, scheduler transitions, blocking calls, or context changes in constructors, destructors, conversion operators, or overloaded operators.
- Code callable from an ISR must have bounded execution, bounded stack use, no allocation, no blocking operation, and no hidden runtime support call.
- Do not use recursion in embedded firmware C++ code.
- Do not use capturing lambdas. A non-capturing lambda requires the same linkage, cost, lifetime, and call-graph evidence as an equivalent named function.
- Keep indirect calls explicit and limited to existing architectural contracts. Templates and concepts must not introduce a new runtime dispatch path.
- Bound automatic object size and call depth. Review stack impact when templates, inlining, or value copies can enlarge an ISR or thread call path.

## MISRA C++:2023 baseline

- Use MISRA C++:2023 as the initial safety baseline and engineering guide for C++ code.
- Apply every MISRA C++:2023 rule that is relevant to the selected C++20 subset, subject to documented project decisions and tool coverage.
- MISRA C++:2023 targets C++17 and does not fully specify C++20 concepts. Concepts and any other C++20-only feature require TaskMate-specific review rules.
- Do not claim full MISRA C++:2023 compliance for the C++20 codebase. Full normative compliance is a long-term objective, not a current acceptance condition.
- Record intentional deviations when they affect safety, portability, deterministic behaviour, or tool analysis. Do not create deviations merely to silence diagnostics.
- Compiler warnings and one static analyser are useful evidence, but they do not by themselves establish MISRA compliance.

## Build and validation

- Compile C++ translation units in standard C++20 mode with exceptions, RTTI, thread-safe local statics, and exit-time destruction disabled.
- Keep C and C++ compiler options separate. C sources continue to follow the project C language profile.
- Pin and report the compiler version used for size, ABI, and code-generation evidence.
- Reject unexpected allocation, exception, RTTI, guard, constructor-table, destructor-table, and C++ runtime symbols at link validation.
- Add tests for every C++ build rule, mixed-language boundary, compile-time constraint, and generated-code contract introduced by a change.
- Validate changes with the `test1` target. Use the final ELF, map, symbol table, and disassembly to compare flash, static RAM, stack, execution time, and interrupt effects against an equivalent direct implementation when applicable.
- Treat the direct implementation and compiler version as part of the measurement baseline. Record the reason for every accepted material difference.
- Recheck generated code and binary evidence after compiler upgrades because template emission, ABI details, and optimization can change.
