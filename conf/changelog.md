# TaskMate changelog policy

`CHANGELOG` is a concise, informative history, not a source of truth. Completeness
is best-effort; later invocations may recover omitted milestones.

## Entry format

- Keep existing lines unchanged.
- Insert new entries at the beginning of the file, in presented order and without
  blank lines between entries.
- Write each entry in English on one physical line of at most 80 characters.
- Start with exactly one tab, one category, and one space.
- Use one primary category: `[build]`, `[arch]`, `[sys]`, `[doc]`, or `[test]`.
- Use concise lowercase imperative text and combine related commits.

Categories:

- `[build]`: build orchestration, target selection, tooling, or generation.
- `[arch]`: architectural boundaries, contracts, or structural changes.
- `[sys]`: runtime, kernel, syscall, HAL, driver, service, or autoCode behaviour.
- `[doc]`: important maintained documentation, excluding audits.
- `[test]`: important test infrastructure or materially broader coverage.

## Fast incremental selection

Inspect only the subjects of the latest 30 non-merge commits. This bounded rolling
window lets later invocations reconsider an omitted recent milestone. Inspect
changed paths or detailed diffs only for plausible or ambiguous candidates.

Exclude merges, minor fixes, formatting, spelling, audits, transient changes, and
milestones already represented by an unversioned entry.

Do not run builds, tests, static analysis, tag validation, full-history scans, or
repository-wide diff statistics for normal changelog maintenance. Assume the
checkout is sufficiently current.

## Release headings and optional statistics

A released section uses an unindented `TaskMate <major>.<minor>` heading, with
blank lines allowed only immediately around it. Add a heading only when explicitly
approved and never rewrite an existing heading.

README `Project Stats` are outside the default changelog update. Change them only
when explicitly requested. Existing build artefacts may be used as best-effort
measurements; run `bmake` only when fresh measurements are explicitly requested.

## Approval and boundaries

Proposal-only is the default. Apply entries only after explicit approval. Never
edit `CHANGELOG_AUTOCODE_SYNTAX`, commit, push, or create a Git tag unless the user
requests that separate action.
