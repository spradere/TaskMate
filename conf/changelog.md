# TaskMate changelog policy

## Managed files

- The release history is `CHANGELOG`.
- The project measurements are in the `Project Stats` section of `README.md`.
- `CHANGELOG_AUTOCODE_SYNTAX` is manual and must never be edited by this workflow.

## Version boundary

- A released section starts with `TaskMate <major>.<minor>` on an unindented line.
- The corresponding Git tag is `v<major>.<minor>`.
- Existing `CHANGELOG` lines are immutable: never edit, delete, reorder, or retag them.
- Additions may insert new entries or an explicitly approved release heading only.
- Insert every approved entry batch at the beginning of the file, in presented order.
- Do not place blank lines between entries. Blank lines are allowed only immediately
  before and after an unindented `TaskMate` version heading.

## Entry format

- Write each entry in English on one physical line of at most 80 characters.
- Start each entry with exactly one tab, one category, and one space.
- Use one primary category: `[build]`, `[arch]`, `[sys]`, `[doc]`, or `[test]`.
- Use concise lowercase imperative text after the category.
- Combine related commits into one important project milestone.

Categories have these meanings:

- `[build]`: build orchestration, target selection, tooling, or generation pipeline.
- `[arch]`: architectural boundaries, layer contracts, or structural changes.
- `[sys]`: runtime, kernel, syscall, HAL, driver, service, or autoCode behaviour.
- `[doc]`: important maintained documentation, excluding audits.
- `[test]`: important test infrastructure or materially broader coverage.

## Selection

Include only important changes that affect behaviour, architecture, maintained
documentation, the build pipeline, or meaningful verification coverage.

Exclude:

- merge commits;
- minor bug fixes;
- formatting-only changes;
- spelling-only changes;
- audit generation, modification, renaming, or deletion;
- transient work absent from the final tree;
- changes already represented by an existing entry.

Commit subjects are evidence, not summaries. Inspect changed files, representative
diffs, final code, and the existing changelog whenever classification is unclear.

## Project Stats

An approved `CHANGELOG` update must update the existing `Project Stats` section of
`README.md` in the same patch. Preserve its Markdown and HTML structure.

Measure the default target with `bmake` before proposing exact values. Use:

- the latest reachable Git release tag for the displayed version;
- `git rev-list --count HEAD` for commits;
- the first `SUM:` file count in `build/cloc_raw` for source files;
- `code_total` in `build/cloc_data` for lines of code;
- `Flash` and `RAM` used values in the default target mem_data file.

Never guess or reuse stale measurements. If the build or extraction fails, do not
modify either managed file.

## Approval

The default operation is proposal-only. Show the exact proposed insertions and
`Project Stats` replacement, then wait for explicit user approval. Approval permits
editing only `CHANGELOG` and the `Project Stats` section of `README.md`; it never
permits a commit, push, tag, or edit of `CHANGELOG_AUTOCODE_SYNTAX`.
