#!/usr/bin/env python3
"""Detect multiple definitions of the same struct/class/union/enum tag.

A type is a DEFINITION (as opposed to a forward declaration) when its tag is
followed by a body, e.g.:

    struct Foo { ... };              -- definition
    struct Foo {};                   -- definition (empty placeholder)
    struct Foo { };                  -- definition (empty placeholder)
    typedef struct Foo { ... } Foo;  -- definition
    typedef struct { ... } Foo;      -- definition (anonymous, name = alias)
    struct Foo;                      -- forward declaration (ONLY thing ignored)

An empty body (``struct Foo {};``) is a closed definition, not a declaration:
two of them in different files are a duplicate definition (a TU including both
fails with a redefinition), and one shadowing a real ``struct Foo {...}`` is a
hazard. Only a true forward declaration (``struct Foo;``) is ignored.

Defining the same tag in two different translation units is not a compiler
error (they never see each other), but it is a maintenance hazard: a future
file that includes both definitions will fail to compile with an ODR
redefinition, and an opaque placeholder shadowing a real type hides the
canonical definition. This script flags every tag that has more than one
definition anywhere in the tree, so those duplicates can be consolidated.

Exit code is 0 when no duplicate definitions are found, 1 otherwise.
"""

import argparse
import os
import re
import sys

SRC_EXTENSIONS = {".h", ".hh", ".hpp", ".hxx", ".c", ".cc", ".cpp", ".cxx"}

# Tag kinds and their keywords. enum needs care: "enum Foo : int { ... }".
KEYWORDS = ("struct", "class", "union")

# Matches "typedef struct Foo {" or "typedef struct Foo Foo;" (named typedef).
_TYPEDEF_OPEN = re.compile(r"\btypedef\s+(struct|class|union|enum)\b")

# Matches a type/namespace scope opener that opens a body with '{':
#   struct Foo { | class Foo : public Bar { | union Foo { | enum Foo { |
#   namespace Foo { | namespace Foo::Bar { | namespace { | enum class Foo {
# The tag is captured; it may be empty for an anonymous namespace.
_SCOPE_OPEN = re.compile(
    r"\b(?P<kw>namespace|struct|class|union|enum)\b"
    r"\s*(?:\bclass\b\s*)?"
    r"(?P<tag>[A-Za-z_][A-Za-z0-9_:]*|)"
    r"\s*(?::\s*[^{;]*)?\s*\{"
)

# Keywords that open a scope we track (tag lookup is by struct/class/union/enum;
# namespace is tracked for qualification only).
_TYPE_KW = {"struct", "class", "union", "enum"}


def strip_comments(text):
    """Remove // and /* */ comments and string literals (naive but adequate
    for scanning tag definitions; does not touch real code)."""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        nxt = text[i + 1] if i + 1 < n else ""
        if c == "/" and nxt == "/":
            end = text.find("\n", i)
            i = end if end != -1 else n
        elif c == "/" and nxt == "*":
            end = text.find("*/", i + 2)
            i = end + 2 if end != -1 else n
        elif c == '"':
            j = i + 1
            while j < n:
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == '"':
                    break
                j += 1
            out.append(" " * (j - i + 1))
            i = j + 1
        else:
            out.append(c)
            i += 1
    return "".join(out)


def find_definitions(text):
    """Yield (kind, qualified_tag, is_empty) for every definition found.

    ``is_empty`` is True when the body is empty (``struct X {};``), which is an
    empty placeholder definition. Only a forward declaration (``struct X;``) is
    not yielded. Tracks namespace/class/union/enum scope via brace depth so
    that, e.g. ``NuMemoryPool::IVisitor`` and ``NuMemoryManager::IVisitor`` are
    treated as distinct types instead of a duplicate.
    """
    text = strip_comments(text)
    n = len(text)
    depth = 0  # total brace depth
    # stack of (tag, close_depth) for each opened type/namespace scope
    scopes = []  # list of path components
    closes = []  # parallel: depth at which that scope's body closes
    defs = []
    i = 0
    while i < n:
        c = text[i]
        if c == "{":
            depth += 1
            i += 1
            continue
        if c == "}":
            depth -= 1
            # pop any scopes whose body closes at this depth
            while closes and closes[-1] == depth:
                closes.pop()
                scopes.pop()
            i += 1
            continue
        if c == ";" or c.isspace():
            i += 1
            continue

        m = _SCOPE_OPEN.match(text, i)
        if m:
            kw = m.group("kw")
            tag = m.group("tag")
            i = m.end()
            if kw in _TYPE_KW and tag:
                # Scan the body between '{' and its matching '}'. An empty body
                # (``struct X {};``) is an opaque placeholder: it is a closed
                # definition, not a forward declaration, so it can still clash
                # with a real definition -- report it as a placeholder.
                j = m.end()
                bd = 1
                while j < n:
                    if text[j] == "{":
                        bd += 1
                    elif text[j] == "}":
                        bd -= 1
                        if bd == 0:
                            break
                    j += 1
                qname = "::".join(scopes + [tag])
                defs.append((kw, qname, text[m.end() : j].strip() == ""))
                scopes.append(tag)
                closes.append(depth)  # this scope closes when depth returns here
            elif kw == "namespace":
                # anonymous namespace has an empty tag
                scopes.append(tag if tag else "(anonymous)")
                closes.append(depth)
            else:
                # a type with no tag. For an anonymous ``typedef struct {...} N;``
                # the trailing typedef alias is the real name -- record it so a
                # ``struct N {...}`` definition elsewhere is caught as a dup.
                if kw in _TYPE_KW:
                    prefix = text[max(0, m.start() - 16) : m.start()]
                    if re.search(r"\btypedef\s*$", prefix):
                        j = m.end()
                        bd = 1
                        while j < n:
                            if text[j] == "{":
                                bd += 1
                            elif text[j] == "}":
                                bd -= 1
                                if bd == 0:
                                    break
                            j += 1
                        k = j + 1
                        tail = ""
                        while k < n and text[k] != ";":
                            tail += text[k]
                            k += 1
                        for alias in re.findall(r"[A-Za-z_][A-Za-z0-9_]*", tail):
                            defs.append(
                                (kw, "::".join(scopes + [alias]), text[m.end() : j].strip() == "")
                            )
                # keep scopes/closes parallel so pop stays in sync
                scopes.append("(anon)")
                closes.append(depth)
            depth += 1  # we just consumed '{'
            continue

        i += 1
    return defs


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("src", help="source root directory to scan (e.g. src/)")
    ap.add_argument(
        "--verbose",
        action="store_true",
        help="print every definition found, not just duplicates",
    )
    args = ap.parse_args()

    root = os.path.abspath(args.src)
    if not os.path.isdir(root):
        print(f"error: not a directory: {root}", file=sys.stderr)
        return 2

    # tag -> dict(file -> count)
    by_tag = {}
    total = 0
    empty = {}  # tag -> True if any definition has an empty body
    for dirpath, _dirnames, filenames in os.walk(root):
        for fn in filenames:
            ext = os.path.splitext(fn)[1].lower()
            if ext not in SRC_EXTENSIONS:
                continue
            path = os.path.join(dirpath, fn)
            try:
                with open(path, "r", encoding="utf-8", errors="replace") as fh:
                    text = fh.read()
            except OSError as e:
                print(f"warning: cannot read {path}: {e}", file=sys.stderr)
                continue
            defs = find_definitions(text)
            for kw, tag, is_empty in defs:
                total += 1
                key = (kw, tag)
                if is_empty:
                    empty[key] = True
                entry = by_tag.setdefault(key, {})
                entry[path] = entry.get(path, 0) + 1

    if args.verbose:
        print(f"scanned {total} definitions\n")

    duplicates = {
        key: files for key, files in by_tag.items() if sum(files.values()) > 1
    }

    if not duplicates:
        print("OK: no duplicate type definitions found")
        return 0

    print("Found type(s) defined in more than one place:\n")
    for kw, tag in sorted(duplicates, key=lambda k: k[1]):
        files = duplicates[(kw, tag)]
        n = sum(files.values())
        empty_mark = "  [empty placeholder(s)]" if (kw, tag) in empty else ""
        print(f"  {kw} {tag}  ({n} definitions){empty_mark}")
        for path, count in sorted(files.items()):
            rel = os.path.relpath(path, os.path.dirname(root))
            print(f"      {rel}:{count}")
        print()
    return 1


if __name__ == "__main__":
    sys.exit(main())
