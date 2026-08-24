#!/usr/bin/env python3
"""Dense, AI-ready per-symbol diff from ``objdiff-cli diff``.

Runs ``objdiff-cli diff -p . <symbol> -o -`` (megabytes of noisy JSON: sections,
label-only symbols, full opcode/operand detail), keeps only the instructions
where the two sides diverge, and prints them aligned with context hints.

There are exactly two sides, in objdiff's fixed order (target on the left, base
on the right, exactly as shown in the objdiff GUI):

  left  == target == the ORIGINAL - where we want to end up, the goal. In this
                     project these are the per-function 'build/split' objects.
  right == base   == the CURRENT - our own src/ as it compiles right now (the
                     full 'build/CMakeFiles' build).

Direction is verified from the object files, not guessed: left bytes match
'build/split' (target), right bytes match 'build/CMakeFiles' (base); and src/
is the current code that base_path builds. Hint attribution follows:

  the '-' and '~' lines come from the TARGET (the original/goal).
  the '+' and '>' lines come from the BASE  (our current code).

So, to make your current source match the original: '-' (original-only
instructions you are missing) must be ADDED to your code, and '+' (extra
instructions your current build emits) must be REMOVED. Note the diff glyphs
are counterintuitive: a '-' does NOT mean "delete from your code".

Usage:  python scripts/objdiff-cli.py SYMBOL  [-p DIR] [-C N] [--full] [--no-color]
"""
import argparse
import base64
import json
import re
import struct
import subprocess
import sys

#: Canonical legend, shared verbatim by the module docstring and the --help text.
_LEGEND = """\
Two sides (objdiff's fixed order; target on the left, base on the right - same
as the objdiff GUI; direction verified from the object files):

  left  = target = the ORIGINAL, where we want to end up (the goal). In this
          project: the per-function 'build/split' objects.
  right = base   = the CURRENT, our src/ as it compiles right now. In this
          project: the full 'build/CMakeFiles' build.

The marker letters are counterintuitive but fixed by the side, so read first
WHICH SIDE a line is from, then act:

  ' '    unchanged - identical on both sides. Printed only as context around
         changes; runs longer than -C collapse to ".. N identical instructions ..".
  '-'    TARGET-only (left) = in the ORIGINAL, missing from your current code.
         => ADD it to your C; you are missing this behaviour. Do NOT delete it.
  '+'    BASE-only (right) = in your CURRENT build, absent from the original.
         => REMOVE/fix it in your C; you emit extra code the original lacks.
  '~'    same opcode, operands differ. The '~' line is the TARGET(original);
         the '>' line below it is the BASE(current). Make your '>' match '~'.
  '>'    the BASE(current) version of the paired '~' instruction.
  '..'   a collapsed run of N unchanged instructions (keeps output dense).

Rule of thumb: '-' and '~' present what the ORIGINAL (goal) does and, if not
already, must be produced by your C. '+' and '>' present your CURRENT build's
behaviour and should be changed to disappear / match the original. To match:
ADD every '-' you lack, REMOVE every '+', and make every '>' line equal its '~'.

Context hints: many lines carry a trailing '#' comment with decompiling signal:
string / float constants ('MaulOnTheRun', 1.0), named '.LC' rodata refs, and
resolved jump/call targets ('-> <instruction or callee>'), so control flow is
readable without tracking branch addresses by hand. Referenced data is also
summarized in a "referenced data (base)" list. Use --full to print the entire
base listing with no context collapsing.
"""

__doc__ += "\n\n" + _LEGEND

#: A symbol counts as fully matched (nothing worth showing) at/above this.
FULL_MATCH = 99.999

_COLOR = {"ctx": "90", "del": "31", "add": "32", "mod": "33"}


# --------------------------------------------------------------------------- #
# helpers
# --------------------------------------------------------------------------- #
def _hex(addr):
    if isinstance(addr, str):
        try:
            addr = int(addr, 0)
        except ValueError:
            return None
    return addr


def _c(text, kind, on):
    if not on or not text:
        return text
    return "\x1b[%sm%s\x1b[0m" % (_COLOR[kind], text)


def first_formatted(element):
    inner = element.get("instruction")
    return inner.get("formatted") if isinstance(inner, dict) else None


def fmt_pct(v):
    return "—" if v is None else ("%.3f" % v).rstrip("0").rstrip(".") + "%"


def pct_of(sym):
    if not sym:
        return None
    p = sym.get("match_percent")
    if p is None:
        return None if not sym.get("kind") else 0.0
    try:
        return float(p)
    except (TypeError, ValueError):
        return None


def demangled(symbol):
    name = symbol.get("demangled_name")
    if name:
        return name
    raw = symbol.get("name", "")
    try:
        return subprocess.run(["c++filt"], input=raw, text=True,
                              capture_output=True).stdout.strip() or raw
    except Exception:
        return raw


# --------------------------------------------------------------------------- #
# referenced-data resolution (rodata -> strings / floats)
# --------------------------------------------------------------------------- #
# Mnemonics that consume an FP constant from a .rodata.cst* location.
_FLOAT_OPS = (
    "movss", "movsd", "movsldup", "addss", "subss", "mulss", "divss",
    "sqrtss", "minss", "maxss", "comiss", "ucomiss", "andps", "orps",
    "xorps", "cvtss2si", "cvtsi2ss", "cvtss2sd", "fld", "fstp", "fadd",
    "fmul", "fsub", "fdiv", "rcpss", "rsqrtss",
)
# Mnemonics that load an address (string / data pointer).
_PTR_OPS = ("lea", "mov", "xchg", "add", "movzx", "movsx")


def _sect_bytes(sec):
    """Reconstruct a section's bytes from its data_diff chunk list."""
    out = bytearray()
    for ch in sec.get("data_diff") or []:
        if ch.get("data"):
            out += base64.b64decode(ch["data"])
        else:
            out += b"\x00" * int(ch.get("size", 0))
    return bytes(out)


class RoDataResolver:
    """Resolves ``.LC`` rodata references in the *base* object to values.

    objdiff assigns ``.rodata.cst4`` / ``.rodata.str1.1`` refs the label name
    ``.LC<N>``; the label's ``address`` is a direct offset into the owning
    section's bytes: floats live at low offsets in ``.rodata.cst4``, strings in
    ``.rodata.str1.1``. We decode both and pick by the referencing mnemonic.
    """

    def __init__(self, right):
        blob = {s["name"]: _sect_bytes(s) for s in right["sections"]}
        self.str_bytes = blob.get(".rodata.str1.1", b"")
        self.cst4 = blob.get(".rodata.cst4", b"")
        self.cst8 = blob.get(".rodata.cst8", b"")
        self.cst16 = blob.get(".rodata.cst16", b"")

    # -- decoders ---------------------------------------------------------- #
    def _string_at(self, off):
        b = self.str_bytes
        if not (0 <= off < len(b)):
            return None
        end = b.find(b"\x00", off)
        s = b[off:end if end >= 0 else len(b)]
        if s and all(32 <= c < 127 for c in s):
            return s.decode()
        return None

    def _float_at(self, off):
        for table, n in ((self.cst4, 4), (self.cst8, 8)):
            if 0 <= off <= len(table) - n:
                raw = table[off:off + n]
                return struct.unpack("<f", raw[:4])[0], raw.hex()
        return None

    # -- main entry --------------------------------------------------------- #
    def resolve(self, addr, disasm):
        """Return a trailing comment (or None) documenting ``addr``'s value."""
        if addr is None:
            return None
        off = int(addr)
        mnemonic = disasm.split()[0]
        is_float = any(m in mnemonic for m in _FLOAT_OPS)

        if is_float:
            got = self._float_at(off)
            if got:
                val, hx = got
                return f"{val!r}  (0x{hx})"

        s = self._string_at(off)
        if s is not None:
            return s.__repr__()

        got = self._float_at(off)
        if got:
            val, hx = got
            return f"{val!r}  (0x{hx})"
        return None


_LC_RE = re.compile(r"\.LC(\d+)")


# --------------------------------------------------------------------------- #
# rendering
# --------------------------------------------------------------------------- #
def render_function(lsym, rsym, color, context, full=False, resolver=None,
                    rightsyms=None, sym_map=None):
    """One symbol's position-aligning instruction diff.

    The two sides' ``instructions`` arrays are positionally aligned by objdiff
    and carry the same ``diff_kind`` at each index. Unchanged runs longer than
    ``context`` collapse to a single ``⋯`` marker. With ``full`` every
    instruction is printed and no runs collapse. Lines get trailing '#' hints:
    base-side ``.LC`` references resolve to their string/float values, and
    jump/call targets resolve to the instruction or callee they point at.
    """
    il = lsym.get("instructions", [])
    ir = rsym.get("instructions", [])

    def _ins_map(elems):
        m = {}
        for e in elems:
            inner = e.get("instruction")
            if isinstance(inner, dict) and inner.get("address") is not None:
                a = _hex(inner["address"])
                if a is not None:
                    m[a] = inner.get("formatted") or ""
        return m

    left_ins = _ins_map(il)
    right_ins = _ins_map(ir)

    rows = []  # (kind, target_text, base_text, addr, base_elem)
    n = max(len(il), len(ir))
    for i in range(n):
        kind = (il[i].get("diff_kind", "") if i < len(il) else "") or ""
        lt = first_formatted(il[i]) if i < len(il) else None
        rt = first_formatted(ir[i]) if i < len(ir) else None
        be = ir[i] if i < len(ir) else {}
        addr = None
        for s in (il[i] if i < len(il) else {}, be):
            inner = s.get("instruction")
            if isinstance(inner, dict) and inner.get("address") is not None:
                addr = inner.get("address")
                break
        if kind == "DIFF_DELETE":
            rows.append(("del", lt, None, addr, be))
        elif kind == "DIFF_INSERT":
            rows.append(("add", None, rt, addr, be))
        elif kind in ("DIFF_REPLACE", "DIFF_OP_MISMATCH", "DIFF_ARG_MISMATCH"):
            rows.append(("mod", lt, rt, addr, be))
        else:
            rows.append(("ctx", lt or rt, None, addr, None))

    rows = [(k, lt, rt, _hex(a), be) for (k, lt, rt, a, be) in rows]
    addr_w = max((len("%x" % (r[3] or 0)) for r in rows), default=0)

    def branch_hint(text, ins_map, sym_map):
        """Resolve a jump/call target to the instruction or callee it points at."""
        if not text:
            return None
        head = text.split()[0]
        if not (head.startswith("j") or head in
                ("call", "loop", "loope", "loopne", "loopz", "loopnz",
                 "jcxz", "jecxz")):
            return None
        mo = re.search(r"\b0x[0-9a-fA-F]+\b(?:-\d+)?", text)
        if not mo:
            return None
        raw = mo.group(0)
        tgt = int(raw.split("-")[0], 16)
        if head == "call":
            if sym_map and tgt in sym_map:
                return "→ " + sym_map[tgt]
            d = ins_map.get(tgt)
            return ("→ " + d) if d else None
        # jumps: prefer in-function target instruction, else callee/global
        d = ins_map.get(tgt)
        if d:
            return "→ " + d
        if sym_map and tgt in sym_map:
            return "→ " + sym_map[tgt]
        return None

    def base_comment(be):
        """Source comment for one base-side element's resolved rodata ref."""
        if resolver is None or not be:
            return None
        inner = be.get("instruction")
        rel = (inner or {}).get("relocation") if isinstance(inner, dict) else None
        if not rel:
            return None
        ts = rel.get("target_symbol")
        if ts is None or rightsyms is None or ts >= len(rightsyms):
            return None
        sym = rightsyms[ts]
        addr = sym.get("address")
        if not _LC_RE.match(sym.get("name", "")) or addr is None:
            return None
        comment = resolver.resolve(addr, (inner or {}).get("formatted", "") or "")
        if comment is None:
            return None
        return comment

    def comment(text, is_base, data_cm):
        """Aggregate data + flow hints into one trailing '#' comment."""
        parts = []
        if data_cm:
            parts.append(data_cm)
        br = branch_hint(text,
                         right_ins if is_base else left_ins,
                         sym_map if is_base else None)
        if br:
            parts.append(br)
        return "  # " + " ; ".join(parts) if parts else None

    refs = {}
    lines = []
    changed = 0
    last_change = -99
    i = 0
    while i < n:
        kind, lt, rt, addr, be = rows[i]
        a = ("%*x" % (addr_w, addr)) if addr is not None else " " * addr_w
        if kind == "ctx":
            run = 1
            while i + run < n and rows[i + run][0] == "ctx":
                run += 1
            in_window = i <= last_change + context + 1
            run_in_window = (i + run - 1) <= last_change + context + 1
            if not full and run > 1 and not (in_window and run_in_window):
                lines.append(f"{_c(' ', 'ctx', color)} {a}  "
                             f"⋯ {run} identical instructions ⋯")
                i += run
                continue
            for j in range(i, i + run):
                if full or j <= last_change + context:
                    aa = ("%*x" % (addr_w, rows[j][3])) if rows[j][3] is not None \
                        else " " * addr_w
                    txt = rows[j][1]
                    c = comment(txt, True, None)
                    if c:
                        txt = f"{txt}{c}"
                    lines.append(f"{_c(' ', 'ctx', color)} {aa}  {txt}")
            i += run
            continue
        changed += 1
        cm = base_comment(be)
        if cm is not None and rt is not None:
            refs[_lc_label(be, rightsyms) or rt] = cm
        if kind == "mod":
            lt_c = comment(lt, False, None)
            lines.append(f"{_c('~', 'mod', color)} {a}  "
                         f"{(lt + lt_c) if lt_c else lt}")
            if rt is not None and rt != lt:
                rt_c = comment(rt, True, cm)
                body = f"{rt}{rt_c}" if rt_c else rt
                lines.append(f"{_c('>', 'mod', color)} {a}  {body}")
        else:
            mark = "-" if kind == "del" else "+"
            txt, isbase = (lt, False) if kind == "del" else (rt, True)
            c = comment(txt, isbase, cm if isbase else None)
            if c:
                txt = f"{txt}{c}"
            lines.append(f"{_c(mark, kind, color)} {a}  {txt}")
        last_change = i
        i += 1
    return lines, changed, refs


def _lc_label(be, rightsyms):
    if not be:
        return None
    inner = be.get("instruction")
    rel = (inner or {}).get("relocation") if isinstance(inner, dict) else None
    ts = (rel or {}).get("target_symbol")
    if ts is not None and rightsyms is not None and ts < len(rightsyms):
        return rightsyms[ts].get("name")
    return None


def base_sym_map(right):
    """Base object: function address -> demangled name (for callee hints)."""
    syms = right["symbols"]
    names = [s.get("name") for s in syms
             if s.get("kind") == "SYMBOL_FUNCTION" and s.get("name")]
    dec = {}
    try:
        res = subprocess.run(["c++filt"], input="\n".join(names) + "\n",
                             text=True, capture_output=True)
        if res.returncode == 0:
            for n, v in zip(names, res.stdout.splitlines()):
                dec[n] = v
    except Exception:
        pass
    m = {}
    for s in syms:
        if s.get("kind") == "SYMBOL_FUNCTION" and s.get("address") is not None:
            a = _hex(s["address"])
            if a is not None:
                m[a] = s.get("demangled_name") or dec.get(s.get("name")) \
                    or s.get("name")
    return m


def render_data(sym, color):
    """Hex preview of a data object's differing chunks."""
    dd = sym.get("data_diff") or []
    lines, changed = [], 0
    for di in dd:
        kind = di.get("kind", "")
        if not kind:
            continue
        b = base64.b64decode(di.get("data") or "") if di.get("data") else b""
        changed += 1
        sample = " ".join("%02x" % c for c in b[:8])
        sample += "" if len(b) <= 8 else " …"
        mark = "-" if kind == "DIFF_DELETE" else ("+" if kind == "DIFF_INSERT" else "~")
        ckind = "del" if mark == "-" else ("add" if mark == "+" else "mod")
        lines.append(f"{_c(mark, ckind, color)}   "
                     f"[{kind.replace('DIFF_', ''):8}] {len(b):>4}B  {sample}")
    return lines, changed


# --------------------------------------------------------------------------- #
# main
# --------------------------------------------------------------------------- #
def main():
    ap = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=__doc__.split("\n\n")[0],
        epilog=_LEGEND,
    )
    ap.add_argument("symbol", help="mangled symbol to diff, e.g. _Z13ResetPodStuffv")
    ap.add_argument("-p", "--project", default=".",
                    help="project dir passed to objdiff-cli (default: '.')")
    ap.add_argument("-C", "--context", type=int, default=4,
                    help="unchanged context lines around changes (default: 4)")
    ap.add_argument("--full", action="store_true",
                    help="print the entire base instruction listing uncollapsed")
    ap.add_argument("--no-color", action="store_true", help="disable ANSI colors")
    args = ap.parse_args()

    proc = subprocess.run(
        ["objdiff-cli", "diff", "-p", args.project, args.symbol, "-o", "-"],
        capture_output=True, text=True)
    if proc.returncode != 0:
        sys.stderr.write("objdiff-cli failed:\n" + proc.stderr + "\n")
        return 1
    data = json.loads(proc.stdout)

    color = sys.stdout.isatty() and not args.no_color
    left = data["left"]
    right = data["right"]
    lm = {s["name"]: s for s in left["symbols"]}
    rm = {s["name"]: s for s in right["symbols"]}

    sym = rm.get(args.symbol) or lm.get(args.symbol)
    if sym is None:
        sys.stderr.write(f"Symbol not found in diff: {args.symbol}\n")
        return 1
    lsym, rsym = lm.get(args.symbol), rm.get(args.symbol)
    p = pct_of(rsym or lsym)

    out = []

    def P(s=""):
        out.append(s)

    P(f"=== {demangled(sym)}   [{args.symbol}]")
    if p is not None:
        P(f"    size {sym.get('size', '—')} · match {fmt_pct(p)}")
    P()
    if p is not None and p >= FULL_MATCH:
        P("    fully matched — no diff to show.")
        print("\n".join(out))
        return 0

    ins_lines, n_ins, refs = render_function(
        lsym or {}, rsym or {}, color, args.context, full=args.full,
        resolver=RoDataResolver(right), rightsyms=right["symbols"],
        sym_map=base_sym_map(right))
    data_lines, n_data = render_data(rsym or lsym, color)

    if not n_ins and not n_data:
        if p is not None:
            P(f"    symbol matches at {fmt_pct(p)} but no code/data diff was emitted.")
        else:
            P("    no paired code or data diff for this symbol.")
    else:
        if n_ins:
            P(f"    {n_ins} differing instructions")
        if n_data:
            P(f"    {n_data} data chunks")
        if args.full:
            P("    (--full: entire base listing, no context collapsing)")
        P()
        for ln in ins_lines + data_lines:
            P(ln)

    if refs:
        P()
        P("    referenced data (base):")
        for label, val in sorted(refs.items()):
            P(f"      {label:<10} = {val}")

    print("\n".join(out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
