#!/usr/bin/env python3
"""disasm_diff.py -- diff one symbol's disassembly between build and original.

Disassembles ``<symbol>`` from both binaries (default
``bazel-bin/src/libTTapp.so`` vs ``res/libTTapp.so``) with Capstone (AT&T
syntax, like objdump), aligns the
two instruction streams by content, and prints a patch-style diff.

No binutils required: ELF32 parsing, symbol lookup and Itanium demangling
are implemented in pure Python in this file; disassembly is done with the
``capstone`` package. There is deliberately no ``objdump``/``nm``/``c++filt``
dependency and no ``--objdump`` flag.

Why content alignment instead of a plain line diff: including the address
offset in the compared text poisons the diff -- a single inserted or
deleted instruction shifts every later offset, so difflib gives up and
emits "delete everything, then add everything". Instead each instruction
is compared by its normalized asm text only (mnemonic + operands, absolute
addresses stripped, self jumps reduced to offsets); the per-side offsets
are shown for reference but never compared. Common instructions
(``movswl (%eax),%ecx``, ``call <foo>``, ...) then act as anchors and only
the real divergences show up as hunks with context.

Normalization of the asm text (so the diff shows code differences, not
load addresses):
  * call/jump targets outside the function keep only ``<symbol>``;
  * branch/jump targets inside the same function keep only ``<+0xNN>``;
  * the symbol name is shown in the diff header instead of a header row.

Row markers (same spirit as scripts/objdiff-cli.py):
  * ``' '``  unchanged -- identical on both sides (context only).
  * ``'-'``  orig-only -- in the ORIGINAL, missing from your build.
  * ``'+'``  build-only -- in your BUILD, absent from the original.
  * ``'~'``  orig version of a paired modified instruction (same mnemonic,
    operands differ -- the ``'>'`` line below it is the build version).
  * ``'>'``  build version of the paired ``'~'`` instruction.

Usage:
    python3 scripts/disasm_diff.py SYMBOL [BUILD_BIN] [ORIG_BIN]
                                     [--show-bytes] [--with-bytes]
                                     [--no-color] [--context N]

    SYMBOL may be mangled (``_Z9GetMenuIDv``) or unmangled/demangled
    (``GetMenuID``, ``GetMenuID()``); unmangled names are resolved against
    the symbol tables of both binaries before disassembling.

Defaults:
    BUILD_BIN = bazel-bin/src/libTTapp.so
    ORIG_BIN  = res/libTTapp.so

Exit codes:
    0  disassemblies are identical (after normalization)
    1  differences found (the diff is printed)
    2  error (symbol missing from one/both binaries, disassembly failed, ...)
"""

import argparse
import difflib
import os
import re
import struct
import sys

try:
    from capstone import (
        CS_ARCH_X86,
        CS_MODE_32,
        CS_OPT_SYNTAX,
        CS_OPT_SYNTAX_ATT,
        CS_GRP_JUMP,
        CS_GRP_CALL,
        CS_OP_IMM,
        Cs,
    )
except ImportError:
    sys.stderr.write("error: the 'capstone' package is required "
                     "(pip install capstone)\n")
    sys.exit(2)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

SHN_UNDEF = 0
STT_FUNC = 2
STB_GLOBAL = 1
STB_WEAK = 2
STB_LOCAL = 0
SHF_EXECINSTR = 0x4

_COLOR = {"ctx": "90", "del": "31", "add": "32", "mod": "33", "hunk": "36"}


def _c(text, kind, on):
    if not on or not text:
        return text
    return "\x1b[%sm%s\x1b[0m" % (_COLOR[kind], text)


# --------------------------------------------------------------------------- #
# minimal ELF32 parser (little-endian 32-bit only -- both our binaries are)
# --------------------------------------------------------------------------- #
class Elf32:
    def __init__(self, path):
        with open(path, "rb") as f:
            self.data = f.read()
        d = self.data
        if d[:4] != b"\x7fELF" or d[4] != 1 or d[5] != 1:
            raise ValueError("%s: not a 32-bit little-endian ELF" % path)
        (e_phoff, self.e_shoff, e_flags, _, _, _,
         self.e_shentsize, self.e_shnum,
         self.e_shstrndx) = struct.unpack_from("<IIIHHHHHH", d, 0x1C)
        self.sections = []  # (name, type, flags, addr, offset, size)
        shstr = self._sh(self.e_shstrndx)
        for i in range(self.e_shnum):
            off = self.e_shoff + i * self.e_shentsize
            (sh_name, sh_type, sh_flags, sh_addr, sh_offset,
             sh_size) = struct.unpack_from("<IIIIII", d, off + 0)
            name = self._cstr(d, shstr[4] + sh_name)
            self.sections.append((name, sh_type, sh_flags, sh_addr,
                                  sh_offset, sh_size))

    def _sh(self, i):
        off = self.e_shoff + i * self.e_shentsize
        return struct.unpack_from("<IIIIIIIIII", self.data, off)

    @staticmethod
    def _cstr(data, off):
        end = data.index(b"\x00", off)
        return data[off:end].decode("ascii", "replace")

    def section(self, name):
        for s in self.sections:
            if s[0] == name:
                return s
        return None

    def symbols(self):
        """Defined function symbols: [(name, addr, size, bind)]."""
        out = []
        for tab, strs in ((".symtab", ".strtab"), (".dynsym", ".dynstr")):
            ssec, strsec = self.section(tab), self.section(strs)
            if ssec is None or strsec is None:
                continue
            _, _, _, _, off, size = ssec[:6]
            sbase = strsec[4]
            for pos in range(off, off + size, 16):
                (st_name, st_value, st_size, st_info, st_other,
                 st_shndx) = struct.unpack_from("<IIIBBH", self.data, pos)
                if st_shndx == SHN_UNDEF or st_name == 0:
                    continue
                if st_info & 0xF != STT_FUNC:
                    continue
                bind = st_info >> 4
                if bind not in (STB_GLOBAL, STB_WEAK, STB_LOCAL):
                    continue
                sec = self.sections[st_shndx] if st_shndx < len(
                    self.sections) else None
                if sec is None or not (sec[2] & SHF_EXECINSTR):
                    continue
                name = self._cstr(self.data, sbase + st_name)
                if not name or name.startswith("$"):
                    continue
                out.append((name, st_value, st_size, bind))
        # Deduplicate (static + dynamic tables may overlap); prefer globals
        # and the largest size.
        best = {}
        for name, addr, size, bind in out:
            key = (name, addr)
            if key not in best or (best[key][3] > bind or
                                   best[key][2] < size):
                best[key] = (name, addr, size, bind)
        return sorted(best.values(), key=lambda s: s[1])

    def code_at(self, addr, size):
        for _, _, flags, saddr, soff, ssize in self.sections:
            if flags & SHF_EXECINSTR and saddr <= addr < saddr + ssize:
                lo = soff + (addr - saddr)
                return self.data[lo:lo + size]
        raise ValueError("address 0x%x not in an executable section" % addr)


# --------------------------------------------------------------------------- #
# minimal Itanium demangler (enough to match base names for resolution)
# --------------------------------------------------------------------------- #
_OPERATORS = {
    "nw": "new", "na": "new[]", "dl": "delete", "da": "delete[]",
    "pl": "+", "mi": "-", "ml": "*", "dv": "/", "rm": "%",
    "an": "&", "or": "|", "er": "^", "aN": "&=", "aO": "|=",
    "aS": "<<=", "aR": ">>=", "rS": ">>", "lS": "<<",
    "ad": "&(unary)", "nt": "~", "ps": "+", "ng": "-", "de": "*",
    "eq": "==", "ne": "!=", "lt": "<", "gt": ">", "le": "<=",
    "ge": ">=", "nt": "!", "pp": "++", "mm": "--", "cl": "()",
    "ix": "[]", "at": "->", "rs": ">>=", "ls": "<<=",
    "co": "~", "oo": "||", "aa": "&&", "cm": ",",
}


def _parse_name(s, i):
    """Parse one <len><chars> component at s[i:]; return (text, next_i)."""
    j = i
    while j < len(s) and s[j].isdigit():
        j += 1
    n = int(s[i:j] or 0)
    return s[j:j + n], j + n


def demangle(mangled):
    """Best-effort demangle; returns (base_name, display)."""
    if not mangled.startswith("_Z"):
        return mangled, mangled
    s = mangled[2:]
    try:
        if s.startswith("N"):
            parts = []
            i = 1
            while i < len(s) and s[i] != "E":
                if s[i] == "C" or (s[i] == "D" and i + 1 < len(s)
                                   and s[i + 1] in "012"):
                    # constructor / destructor of the previous component
                    tag, i = s[i:i + 2], i + 2
                    prev = parts[-1] if parts else "?"
                    parts.append(("~" if tag[0] == "D" else "") + prev)
                elif s[i].isdigit():
                    comp, i = _parse_name(s, i)
                    parts.append(comp)
                else:
                    # substitution / template args / operator: skip token
                    m = re.match(r"S[A-Za-z0-9_]*_|T_", s[i:])
                    if m:
                        i += len(m.group(0))
                        parts.append("?")
                    else:
                        break
            base = parts[-1] if parts else mangled
            return base, "::".join(parts) + "(...)"
        else:
            if s[:1].isdigit():
                comp, _ = _parse_name(s, 0)
                return comp, comp + "(...)"
            m = re.match(r"([A-Za-z]+)(.*)", s)
            if m and m.group(1) in _OPERATORS:
                return "operator" + _OPERATORS[m.group(1)], \
                    "operator" + _OPERATORS[m.group(1)] + "(...)"
    except (ValueError, IndexError):
        pass
    return mangled, mangled


def base_name(demangled_display):
    """``GetMenuID()`` / ``ns::GetMenuID(...)`` -> ``GetMenuID``."""
    head = re.split(r"\s*\(", demangled_display.strip(), maxsplit=1)[0]
    return head.split("::")[-1].strip("~ ")


def resolve_symbol(query, elves):
    """Resolve a possibly-unmangled query to the mangled symbol to diff.

    An exact (mangled) hit present in at least one binary wins immediately.
    Otherwise all defined function symbols are demangled internally and the
    query (also un-parenthesized) is matched against the demangled base
    name, case-sensitively first, then case-insensitively. Ambiguous matches
    (overloads) are reported, not guessed.
    """
    syms = {}  # mangled -> [(elf_index, bind)]
    for idx, elf in enumerate(elves):
        for name, addr, size, bind in elf.symbols():
            syms.setdefault(name, []).append((idx, bind))
    if query in syms:
        return query

    dem = {n: demangle(n) for n in syms}
    q = query.strip()
    q_base = base_name(q)

    def matches(n):
        base, _ = dem[n]
        return base in (q, q_base)

    exact = sorted(n for n in syms if matches(n))
    if len(exact) == 1:
        return exact[0]
    if len(exact) > 1:
        both = [n for n in exact
                if len({i for i, _ in syms[n]}) > 1] or exact
        if len(both) == 1:
            return both[0]
        glob = [n for n in both
                if any(b in (STB_GLOBAL, STB_WEAK) for _, b in syms[n])]
        if len(glob) == 1:
            return glob[0]
        sys.stderr.write(
            "error: '%s' is ambiguous; matches: %s\n"
            "pass the mangled name explicitly\n"
            % (query, ", ".join(
                "%s (%s)" % (n, dem[n][1]) for n in (glob or both))))
        sys.exit(2)

    ci = q_base.lower()
    fuzzy = sorted(n for n in syms
                   if dem[n][0].lower() == ci or n.lower() == ci)
    if len(fuzzy) == 1:
        sys.stderr.write("note: '%s' resolved to '%s' (%s)\n"
                         % (query, fuzzy[0], dem[fuzzy[0]][1]))
        return fuzzy[0]
    if len(fuzzy) > 1:
        sys.stderr.write(
            "error: '%s' is ambiguous; matches: %s\n"
            "pass the mangled name explicitly\n"
            % (query, ", ".join(
                "%s (%s)" % (n, dem[n][1]) for n in fuzzy)))
        sys.exit(2)

    sys.stderr.write("error: symbol '%s' not found in %s\n"
                     % (query, " or ".join(e.path for e in elves)))
    sys.exit(2)


# --------------------------------------------------------------------------- #
# disassembly via capstone
# --------------------------------------------------------------------------- #

def symbol_at(elves_syms, addr):
    """Mangled name of the function containing addr, or None."""
    best = None
    for name, saddr, ssize in elves_syms:
        if saddr <= addr < saddr + (ssize or 1):
            if best is None or saddr > best[1]:
                best = (name, saddr, ssize)
    return best[0] if best else None


# Capstone's AT&T syntax appends size suffixes (calll, addl, movl) where
# GNU objdump prints the bare mnemonic (call, add, mov). Strip the suffix
# for those mnemonics so the output reads like objdump.
_NOSUFFIX = {
    "call", "jmp", "ret", "nop", "push", "pop", "pusha", "popa",
    "leave", "lea", "loop", "loope", "loopne", "jcxz", "jecxz",
    "int", "into", "iret", "hlt", "xchg",
}


def att_mnemonic(raw):
    """Strip Capstone's AT&T size suffix for mnemonics objdump prints bare."""
    if raw == "calll":
        return "call"
    m = re.match(r"^([a-z]+)([bwlq])$", raw)
    if m and m.group(1) in _NOSUFFIX:
        return m.group(1)
    return raw


def disassemble(elf, all_syms, symbol):
    """Disassemble one function; return [(offset, bytes_hex, asm_text)].

    Branch targets inside the function become ``<+0xNN>``; call/jump
    targets elsewhere become ``<name>`` (absolute addresses stripped).
    """
    hits = [(n, a, s) for n, a, s, _ in elf.symbols() if n == symbol]
    if not hits:
        sys.stderr.write("error: symbol '%s' not found in %s\n"
                         % (symbol, elf.path))
        sys.exit(2)
    _, addr, size = hits[0][:3]
    if not size:
        # Size-0 symbol: extend to the next function in the same binary.
        following = [a for _, a, _, _ in elf.symbols() if a > addr]
        end = min(following) if following else addr + 4096
        size = end - addr
    code = elf.code_at(addr, size)

    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.syntax = CS_OPT_SYNTAX_ATT
    md.detail = True

    flat = [(n, a, s or 1) for n, a, s, _ in elf.symbols()]
    out = []
    pos = 0
    for ins in md.disasm(code, addr):
        if ins.address - addr > pos:
            # Capstone skipped undecodable bytes: show them as data so
            # nothing silently disappears.
            gap = code[pos:ins.address - addr]
            out.append((pos, gap.hex(" "),
                        ".byte " + ", ".join("0x%02x" % c for c in gap)))
        op = ins.op_str
        if ins.groups and (CS_GRP_JUMP in ins.groups or
                           CS_GRP_CALL in ins.groups):
            try:
                ops = list(ins.operands)
            except Exception:
                ops = []
            if ops and ops[0].type == CS_OP_IMM:
                tgt = ops[0].imm & 0xFFFFFFFF
                if addr <= tgt < addr + size:
                    ref = "<+0x%x>" % (tgt - addr)
                else:
                    name = symbol_at(flat, tgt)
                    ref = "<%s>" % name if name else "<0x%x>" % tgt
                op = re.sub(r"\b0x[0-9a-f]+\b", ref, op, count=1)
        text = (att_mnemonic(ins.mnemonic) + ("  " + op if op else "")).strip()
        text = re.sub(r"\s+", " ", text)
        # objdump style: no space after operand commas ("mov %ebx,(%esp)").
        text = re.sub(r",\s+", ",", text)
        # <sym+0xNN> can't occur (we emit refs ourselves), but normalize
        # any residual self-name refs defensively.
        text = re.sub(r"<%s\+0x0*([0-9a-fA-F]+)>" % re.escape(symbol),
                      r"<+0x\1>", text)
        out.append((ins.address - addr, ins.bytes.hex(" "), text))
        pos = ins.address - addr + ins.size
    if pos < len(code):
        gap = code[pos:]
        out.append((pos, gap.hex(" "),
                    ".byte " + ", ".join("0x%02x" % c for c in gap)))
    if not out:
        sys.stderr.write("error: symbol '%s' has no instructions in %s\n"
                         % (symbol, elf.path))
        sys.exit(2)
    return out


# --------------------------------------------------------------------------- #
# content alignment + patch-style rendering (offsets shown, never compared)
# --------------------------------------------------------------------------- #
def key_of(insn, with_bytes=False):
    """Comparison key: normalized asm text (offsets never compared)."""
    _, raw_bytes, text = insn
    key = re.sub(r"\s+", " ", text).strip()
    if with_bytes:
        key += "\t" + raw_bytes
    return key


def mnemonic(insn):
    """First token of the asm text (the opcode), or ''."""
    text = insn[2]
    return text.split()[0] if text.split() else ""


def align(o_ins, b_ins, with_bytes=False):
    """Content-align the two streams; return rows of (tag, o_idx, b_idx).

    Tags: ' ' equal, '-' orig-only, '+' build-only, '~' modified pair
    (same mnemonic, operands differ). Replace blocks are paired up
    positionally; leftovers become -/+ rows.
    """
    o_keys = [key_of(i, with_bytes) for i in o_ins]
    b_keys = [key_of(i, with_bytes) for i in b_ins]
    sm = difflib.SequenceMatcher(None, o_keys, b_keys, autojunk=False)
    rows = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for i, j in zip(range(i1, i2), range(j1, j2)):
                rows.append((" ", i, j))
        elif tag == "delete":
            for i in range(i1, i2):
                rows.append(("-", i, None))
        elif tag == "insert":
            for j in range(j1, j2):
                rows.append(("+", None, j))
        elif tag == "replace":
            for k in range(max(i2 - i1, j2 - j1)):
                i = i1 + k if i1 + k < i2 else None
                j = j1 + k if j1 + k < j2 else None
                if i is not None and j is not None:
                    if mnemonic(o_ins[i]) == mnemonic(b_ins[j]):
                        rows.append(("~", i, j))
                    else:
                        rows.append(("-", i, None))
                        rows.append(("+", None, j))
                elif i is not None:
                    rows.append(("-", i, None))
                else:
                    rows.append(("+", None, j))
    return rows


def hunk_windows(rows, context):
    """Merge change neighborhoods into (start, end) row windows."""
    changes = [k for k, r in enumerate(rows) if r[0] != " "]
    if not changes:
        return []
    windows = []
    start = max(0, changes[0] - context)
    end = min(len(rows), changes[0] + context + 1)
    for k in changes[1:]:
        lo, hi = max(0, k - context), min(len(rows), k + context + 1)
        if lo <= end:
            end = max(end, hi)
        else:
            windows.append((start, end))
            start, end = lo, hi
    windows.append((start, end))
    return windows


def off_range(ins, idxs):
    """(first_off, last_off, count) for a set of instruction indices."""
    offs = [ins[i][0] for i in idxs if i is not None]
    if not offs:
        return None
    return (offs[0], offs[-1], len(offs))


def fmt_off(off):
    return "?" if off is None or off < 0 else "0x%x" % off


def render_row(tag, o, b, show_bytes, color):
    """Render one aligned row (a '~' row yields two printed lines)."""
    lines = []
    if tag == " ":
        o_off = fmt_off(o[0])
        b_off = fmt_off(b[0])
        off = o_off if o_off == b_off else "%s/%s" % (o_off, b_off)
        body = "%s  %s" % (off, o[2])
        if show_bytes:
            body += "   [%s | %s]" % (o[1], b[1]) if o[1] != b[1] \
                else "   [%s]" % o[1]
        lines.append(_c("  " + body, "ctx", color))
    elif tag == "-":
        body = "- %s  %s" % (fmt_off(o[0]), o[2])
        if show_bytes:
            body += "   [%s]" % o[1]
        lines.append(_c(body, "del", color))
    elif tag == "+":
        body = "+ %s  %s" % (fmt_off(b[0]), b[2])
        if show_bytes:
            body += "   [%s]" % b[1]
        lines.append(_c(body, "add", color))
    elif tag == "~":
        ob = "- %s  %s" % (fmt_off(o[0]), o[2])
        bb = "> %s  %s" % (fmt_off(b[0]), b[2])
        if show_bytes:
            ob += "   [%s]" % o[1]
            bb += "   [%s]" % b[1]
        lines.append(_c("~ " + ob[2:], "mod", color))
        lines.append(_c(bb, "mod", color))
    return lines


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("symbol",
                    help="mangled (_Z9GetMenuIDv) or unmangled (GetMenuID) symbol")
    ap.add_argument("build", nargs="?",
                    default=os.path.join(ROOT, "bazel-bin/src/libTTapp.so"))
    ap.add_argument("orig", nargs="?",
                    default=os.path.join(ROOT, "res/libTTapp.so"))
    ap.add_argument("--show-bytes", action="store_true",
                    help="also show the raw opcode bytes on each row "
                         "(default: asm text only)")
    ap.add_argument("--with-bytes", action="store_true",
                    help="strict mode: include raw bytes in the comparison "
                         "(implies --show-bytes)")
    ap.add_argument("--no-color", action="store_true",
                    help="disable ANSI colors")
    ap.add_argument("--context", "-C", type=int, default=3,
                    help="context lines around changes (default: 3)")
    args = ap.parse_args()

    for path in (args.build, args.orig):
        if not os.path.isfile(path):
            sys.stderr.write("error: file not found: %s\n" % path)
            return 2

    try:
        b_elf = Elf32(args.build)
        o_elf = Elf32(args.orig)
    except (OSError, ValueError) as e:
        sys.stderr.write("error: %s\n" % e)
        return 2
    b_elf.path, o_elf.path = args.build, args.orig

    symbol = resolve_symbol(args.symbol, (b_elf, o_elf))

    b_ins = disassemble(b_elf, b_elf.symbols(), symbol)
    o_ins = disassemble(o_elf, o_elf.symbols(), symbol)

    show_bytes = args.show_bytes or args.with_bytes
    rows = align(o_ins, b_ins, with_bytes=args.with_bytes)
    color = sys.stdout.isatty() and not args.no_color

    print("symbol: %s" % symbol)
    print("--- %s (original)" % args.orig)
    print("+++ %s (build)" % args.build)

    windows = hunk_windows(rows, args.context)
    if not windows:
        print("identical (%d instructions)" % len(o_ins))
        if show_bytes:
            return 0
        bdiff = sum(1 for a, b in zip(o_ins, b_ins) if a[1] != b[1])
        if bdiff and len(o_ins) == len(b_ins):
            print("(raw opcode bytes differ in %d line(s); "
                  "use --show-bytes/--with-bytes to see them)" % bdiff)
        return 0

    n_del = n_add = n_mod = 0
    prev_end = 0
    for start, end in windows:
        if start > prev_end:
            skipped = sum(1 for r in rows[prev_end:start] if r[0] == " ")
            if skipped:
                print(_c("  ⋯ %d unchanged instructions ⋯" % skipped,
                         "ctx", color))
        o_idx = [r[1] for r in rows[start:end]]
        b_idx = [r[2] for r in rows[start:end]]
        o_r, b_r = off_range(o_ins, o_idx), off_range(b_ins, b_idx)

        def fmt_range(r):
            if r is None:
                return "∅"
            first, last, n = r
            return "%s..%s (%d)" % (fmt_off(first), fmt_off(last), n)

        print(_c("@@ orig %s  build %s @@"
                 % (fmt_range(o_r), fmt_range(b_r)), "hunk", color))
        for tag, i, j in rows[start:end]:
            o = o_ins[i] if i is not None else None
            b = b_ins[j] if j is not None else None
            for line in render_row(tag, o, b, show_bytes, color):
                print(line)
            if tag == "-":
                n_del += 1
            elif tag == "+":
                n_add += 1
            elif tag == "~":
                n_mod += 1
        prev_end = end

    print()
    print("%d modified, %d orig-only, %d build-only "
          "(%d orig vs %d build instructions total)"
          % (n_mod, n_del, n_add, len(o_ins), len(b_ins)))
    return 1


if __name__ == "__main__":
    sys.exit(main())
