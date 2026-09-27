# Doxygen Documentation for Public APIs

## Scope

This audit evaluates adding Doxygen documentation to public APIs declared in header files. It
considers code readability, maintenance cost, and usefulness for TaskMate.

## Overall assessment

Adding Doxygen documentation to public APIs in `.h` files is appropriate for TaskMate. Header files
are the natural place to describe the contract visible to a caller.

The rule should be more precise than requiring documentation for every public function:

> Document in header files the parts of a public contract that are not fully expressed by the
> declaration.

This includes preconditions, units, limits, return semantics, side effects, concurrency rules, and
execution context. Comments that only restate a function name should be avoided.

The existing coding style already follows this principle. It requires comments to explain intent,
constraints, hardware behaviour, timing, or non-obvious ordering, without repeating the code.

## Readability

### Benefits in header files

Public API documentation lets a caller understand a contract without opening the corresponding
implementation. This is especially useful in TaskMate because headers represent architectural
boundaries:

- tasks access kernel and hardware services through `sysCall`;
- `sysCall` delegates to `sysCore` or HAL;
- `interfaces` provides portable shared contracts.

Several current declarations do not express enough information on their own. For example,
`sc_threadGetInfo()` does not indicate whether output pointers may be null, whether outputs are
modified on failure, how identifiers are numbered, who owns the returned string, or whether the
function may be called from interrupt context.

Likewise, `sc_stringCopy()` does not state whether its size includes the null terminator, whether
the destination is always terminated, whether source and destination may overlap, or how null
pointers are handled.

These properties belong to the public contract and should be documented in the header.

### Risk of excessive documentation

Mechanical documentation of every declaration can add noise. A comment such as the following adds
almost no information:

```c
/**
 * Get the run level.
 *
 * @return The run level.
 */
uint8_t sc_runLevelGet(void);
```

This style increases the distance between declarations and makes an API harder to scan. To preserve
readability:

1. Retain the existing functional groups.
2. Use short comments for simple functions.
3. Reserve detailed blocks for non-trivial contracts.
4. Do not require `@param` text when it adds no semantic information.
5. Document public types, constants, and macros as well as functions.

The Doxygen comments should complement the existing header structure, not replace it.

## Maintenance cost

### Initial cost

The initial cost would be significant because most headers currently contain only a Doxygen file
block. Detailed API descriptions are still uncommon. A repository-wide conversion in one change
would have several drawbacks:

- a large diff that is difficult to review;
- a risk of documenting assumed rather than guaranteed behaviour;
- possible contradictions between comments and implementations;
- immediate maintenance of a large body of new text;
- confusion between documentation improvements and actual contract changes.

A mechanical campaign that adds a standard block to every prototype is therefore not recommended.

### Recurring cost

The recurring cost should remain low when comments describe stable properties:

- units;
- valid ranges;
- ownership and lifetime of data;
- results on failure;
- task or interrupt context restrictions;
- blocking behaviour and atomicity;
- required driver or system state;
- required relationships between paired calls.

The cost becomes high when header documentation describes internal algorithms. Such details may
change during refactoring even when the public contract remains unchanged.

A useful separation is:

> The `.h` file explains what the caller may rely on. The `.c` file explains how and why the
> implementation satisfies that contract.

The primary maintenance risk is stale documentation. In an embedded RTOS, an incorrect statement
about atomicity, interrupt context, pointer lifetime, or a counter unit can be more dangerous than
no comment. Documentation must therefore be updated in the same change as the affected contract.
A disagreement between a header and its implementation should be treated as a functional defect
in code review.

## Usefulness for TaskMate

### C declarations express few constraints

A declaration such as `err_codes_t sc_rtcRead(hal_rtc_time_t *time)` does not say whether `time` is
mandatory, whether it is modified on failure, or which driver state is required.

Similarly, `hal_usartRead(uint8_t *data)` does not make it possible to distinguish the contract for
no available data, a hardware error, an invalid argument, or an uninitialized driver.

### Embedded constraints need explicit contracts

TaskMate APIs may need to state properties that are not visible in a C type:

- blocking or non-blocking behaviour;
- safety in interrupt context;
- whether interrupts are masked;
- atomicity on an 8-bit CPU;
- delay and counter units;
- RAM or program-memory storage;
- pointer ownership and lifetime;
- execution-time bounds when they are contractual.

These details are directly useful to callers of an RTOS running on AVR.

### Documentation reinforces architectural boundaries

Public comments can prevent callers from depending on a detail of HAL or `sysCore`. They can state
the guarantees of the local abstraction without exposing its selected target implementation.

### Doxygen provides navigable documentation

Even without publishing HTML documentation immediately, Doxygen comments are useful through IDE
tooltips, symbol navigation, source searches, code review, and future reference generation.

## Existing model

`srcs/interfaces/hal_atomic.h` is a good model. Its documentation explains guarantees that cannot be
deduced from the prototypes:

- the complete interrupt state is captured;
- capture and masking have no intervening window;
- nested calls preserve each caller's state;
- calls from interrupt context are supported;
- the returned token must be passed unchanged to the matching end call;
- the exact captured state is restored.

This is the recommended level of detail. The comments describe the contract without documenting the
target-specific instructions used to implement it.

## Recommended scope

Documentation should be added in this order:

1. Task-visible APIs in `srcs/system/sysCall/*.h` and `srcs/tmLibc/*.h`.
2. Portable contracts in `srcs/interfaces/*.h`, including non-trivial types, enums, constants, and
   macros.
3. Shared HAL APIs, especially contracts with initialization, interrupt, or atomicity constraints.
4. Internal APIs shared by multiple translation units when their headers represent stable
   contracts.

Heavy documentation is not necessary for:

- obvious functions local to one module;
- task headers that contain only an entry point with no special contract;
- implementation details;
- generated code, unless its source generator is updated.

A declaration in a header is not automatically public to tasks. Public visibility must be
understood relative to the architectural layer that owns the header.

## Recommended content

A function block should answer only the questions relevant to its contract. For example:

```c
/**
 * Read one byte from the USART receive queue.
 *
 * This function does not block.
 *
 * @param[out] data Destination for the received byte. Must not be NULL.
 *                  It is unchanged when no byte is available.
 *
 * @retval ERR_NO_ERROR A byte was written to data.
 * @retval ERR_NO_DATA No byte is currently available.
 * @retval ERR_INVALID_ARGUMENT data is NULL.
 *
 * @note Call from task context only.
 */
err_codes_t sc_usartRead(uint8_t *data);
```

This example is illustrative. The exact return values and guarantees must be verified against the
implementation before they are added to the project.

Useful Doxygen content includes:

- `@param[in]`, `@param[out]`, or `@param[in,out]` when direction clarifies the contract;
- `@retval` when distinct results have distinct meanings;
- units and valid ranges for numeric values;
- output behaviour on failure;
- ownership and lifetime of pointers;
- permitted task or interrupt context;
- blocking or non-blocking behaviour;
- initialization preconditions;
- atomicity guarantees or their absence;
- relationships between paired calls.

## Proposed policy

The following policy would provide useful documentation without unnecessary ceremony:

> Every public API must have a useful description of its contract in its header. Parameters and
> return values are documented when semantic information cannot be derived unambiguously from the
> declaration. Applicable constraints concerning units, ranges, lifetime, initialization,
> atomicity, concurrency, interrupt context, and error behaviour are always documented. Comments do
> not repeat the symbol name or type.

## Recommendation

Adopt Doxygen documentation for public APIs with the following conditions:

- prioritize contract quality rather than complete Doxygen coverage;
- include important public types, macros, and constants;
- do not duplicate implementation details in headers;
- proceed incrementally as each API is changed or reviewed;
- begin with `sysCall` and `interfaces`, which define the most important boundaries;
- use `hal_atomic.h` as the model for the appropriate level of detail;
- verify consistency between comments and implementations during code review.

The expected result is improved API readability and safer use of embedded contracts. The initial
cost is moderate to high if applied broadly, but the recurring cost should remain low when
documentation is concise, stable, and updated together with the code.
