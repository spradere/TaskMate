---
name: taskmate-changelog
description: >-
  Analyze TaskMate history and propose or apply important CHANGELOG entries with
  verified README project statistics. Use for changelog maintenance or release
  preparation, not for CHANGELOG_AUTOCODE_SYNTAX.
---

# TaskMate changelog

Read `conf/changelog.md` completely and treat it as the source of truth. If it is
missing or ambiguous, stop before editing.

## Proposal

Proposal-only is the default. Do not edit files during this phase.

1. Record the initial Git status and preserve unrelated work.
2. Read `CHANGELOG` and locate its first `TaskMate <major>.<minor>` heading.
3. Map that heading to `v<major>.<minor>` and verify the Git tag exists.
4. Inspect the range from that tag to `HEAD`, excluding merge commits. Use commit
   subjects, name-status data, diff statistics, representative diffs, and final
   definitions or call paths when needed.
5. Compare candidate milestones semantically with every existing unversioned entry.
   Do not propose a duplicate merely because its wording differs.
6. Apply the policy importance and exclusion rules. Classify each surviving
   milestone with one primary category.
7. Run the default `bmake` build and extract every `Project Stats` value from the
   sources named by policy. Treat a completed `Build complete` message as required
   evidence. Do not treat a dry run as a measurement.
8. Present the analyzed range, excluded-change summary, exact candidate lines, exact
   `Project Stats` replacement, and files that would change. Then wait.

Every candidate line must contain one leading tab and be at most 80 characters,
including indentation and category. Keep the text in English and on one physical
line.

## Application

Apply only after explicit user approval of the current proposal.

1. Verify `CHANGELOG` and `README.md` have not changed since the proposal. Stop and
   re-propose if either changed.
2. Insert the approved entries at the beginning of `CHANGELOG`, in their presented
   order. Do not add a blank separator: the previous first entry must follow the new
   batch immediately. Do not change any byte from the previous file content.
3. Add a `TaskMate <major>.<minor>` heading only when the user explicitly approves
   that release operation. Never rewrite an existing heading.
4. Replace only the existing `Project Stats` heading and values in `README.md`,
   using the measurements shown in the approved proposal.
5. Verify allowed categories, leading tabs, single-line entries, the 80-character
   limit, and that blank lines occur only around release headings. Run
   `git diff --check` and inspect the complete diff of both managed files.
6. Report the inserted entries, measured statistics, validation results, and any
   hardware-validation limit.

Never edit `CHANGELOG_AUTOCODE_SYNTAX`. Never commit, push, or create a Git tag unless
the user requests that separate action explicitly.
