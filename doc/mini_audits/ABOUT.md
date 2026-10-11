# Mini audit rules

Mini audits are short snapshots for simple, focused questions. Use `doc/audits/` for investigations that need a detailed analysis or a multi-step resolution plan. A mini audit must fit on one rendered page. Keep it concise, with no large tables or code blocks.

Use this structure, in order:

1. Header: title, audit date, current version tag, and current HEAD commit.
2. `Question`: the specific issue being examined and its scope.
3. `Problems found in code`: concrete findings with source paths, including a clear statement when no defect was found.
4. `Solution`: a proportionate action or recommendation, with any relevant limits.

Write in English. Keep each prose paragraph or list item on one source line. Follow the snapshot lifecycle in [audit rules](../audits/ABOUT.md): strike through resolved findings with `~~` and delete the report when all findings are resolved.
