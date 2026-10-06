#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#

"""Reject unnamed numeric literals in TaskMate C code."""

from pathlib import Path
import re


# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

SOURCE_ROOT = Path(__file__).resolve().parents[2] / "srcs"
NUMBER = re.compile(
    r"(?<![A-Za-z_0-9.])(?:0[xX][0-9A-Fa-f]+|0[bB][01]+|[0-9]+)[uUlL]*(?![A-Za-z_0-9.])"
)
NAMED_DEFINE = re.compile(r"\s*#define\s+[A-Z][A-Za-z0-9_]*\b(?!\()")
NAMED_ENUM = re.compile(r"\s*(?:enum\s*\{\s*)?[A-Z][A-Z0-9_]*\s*=")


# ---------------------------------------------------------------------------
# Functions
# ---------------------------------------------------------------------------

def code_only(source: str) -> str:
    """Hide comments and C string or character contents, retaining line positions."""
    result = []
    state = "code"
    index = 0
    while index < len(source):
        char = source[index]
        next_char = source[index + 1] if index + 1 < len(source) else ""
        if state == "code":
            if char == "/" and next_char in ("/", "*"):
                result.extend("  ")
                state = "line" if next_char == "/" else "block"
                index += 2
                continue
            if char in ('"', "'"):
                result.append(" ")
                state = char
            else:
                result.append(char)
        elif state == "line":
            result.append("\n" if char == "\n" else " ")
            if char == "\n":
                state = "code"
        elif state == "block":
            result.append("\n" if char == "\n" else " ")
            if char == "*" and next_char == "/":
                result.append(" ")
                index += 1
                state = "code"
        else:
            result.append("\n" if char == "\n" else " ")
            if char == "\\" and next_char:
                result.append("\n" if next_char == "\n" else " ")
                index += 1
            elif char == state:
                state = "code"
        index += 1
    return "".join(result)


def main() -> int:
    violations = []
    for path in sorted(SOURCE_ROOT.rglob("*")):
        if path.suffix not in (".c", ".h"):
            continue
        source = path.read_text()
        masked = code_only(source).splitlines()
        original = source.splitlines()
        in_named_define = False
        for line_number, (line, raw) in enumerate(zip(masked, original), 1):
            named_define = bool(NAMED_DEFINE.match(line) or NAMED_ENUM.match(line))
            if not (in_named_define or named_define):
                for match in NUMBER.finditer(line):
                    if match.group() not in ("0U", "1U"):
                        violations.append(f"{path}:{line_number}: {match.group()}")
            in_named_define = (in_named_define or named_define) and raw.rstrip().endswith("\\")
    if violations:
        print("\n".join(violations))
        return 1
    print("TaskMate C numeric literals: passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
