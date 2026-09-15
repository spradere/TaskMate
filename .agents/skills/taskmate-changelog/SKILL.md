---
name: taskmate-changelog
description: >-
  Quickly review recent TaskMate changes and propose or apply concise CHANGELOG
  entries. Use for changelog maintenance or release preparation, not for
  CHANGELOG_AUTOCODE_SYNTAX.
---

# TaskMate changelog

Read `conf/changelog.md` for the entry format and selection policy. The changelog
is informative and best-effort: missing a change is acceptable because a later
invocation can catch it.

## Fast proposal

Proposal-only is the default. Do not edit files before explicit approval.

1. Read only the unversioned entries at the beginning of `CHANGELOG`.
2. Inspect only the subjects of the latest 30 non-merge commits. This rolling
   window lets a later invocation reconsider an omitted recent milestone.
3. Inspect changed paths, diffs, or final definitions only for plausible or
   ambiguous candidates.
4. Exclude minor changes and milestones already represented by an unversioned
   entry. Group related commits into a few important milestones.
5. Present the scan range and exact candidate lines, then wait for approval.

Do not run builds, tests, static analysis, full-history scans, or repository-wide
diff statistics for the normal workflow. Do not verify that the checkout is fully
up to date.

Each candidate must be English, one physical line, at most 80 characters, and
start with exactly one tab followed by `[build]`, `[arch]`, `[sys]`, `[doc]`, or
`[test]` and one space.

## Application

After explicit approval:

1. Insert the approved lines at byte zero in their presented order, without a
   blank separator or changes to existing content.
2. Add a release heading only when explicitly requested.
3. Inspect the small `CHANGELOG` diff and report the inserted lines.

README `Project Stats` are optional. Update them only when explicitly requested;
prefer existing build artefacts and do not run `bmake` unless the user asks for a
fresh measurement.

Never edit `CHANGELOG_AUTOCODE_SYNTAX`. Never commit, push, or create a Git tag
unless the user explicitly requests that separate action.
