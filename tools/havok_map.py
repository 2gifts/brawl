#!/usr/bin/env python3
"""Map Brawl's Havok functions to the two external reference decomps.

One row per function in the havok lib and in sora/ph (Sora's wrappers):

  brawl_tu, symbol, address, size, inline_or_outline,
  halo_match, halo_confidence, skyrim_match, skyrim_confidence, notes

Brawl side: every function symbol inside the .text ranges of those units
(config/RSBE01_02/splits.txt + symbols.txt), disassembled straight from
main.dol for its fingerprint: float constants (lfs/lfd), string references,
integer immediates, struct offsets touched, callees in order, virtual-call
slots and its own vtable slot (vtables found from the DOL's data).

Skyrim (Community-Behaviors, MSVC x64, Havok 2010, real Havok names): matched
by Havok name after folding hkp/hka to hk, confirmed by argument count and by
vtable slot (the slot offset between Havok 4.0 and 2010 is calibrated per
class from two or more name matches). Unnamed functions in a vtable are tried
by slot alone (low confidence).

Halo 2 (halo2-decompiled, MSVC 7.1 x86, Havok ~2004, behaviour names, source
only, no binary): matched by fingerprint, per pair:
  float    float constants (Brawl lfs/lfd from the DOL; Halo literals)
  str      string literals (fuzzy: "TtCapsTriangle" ~ "TtCapsuleTriangle")
  int      integer immediates (two or more shared)
  name/cls method / class identifier tokens (Halo's behaviour names)
  vtab     same slot of two aligned virtual tables (Brawl vtable vs the Halo
           class's declared virtuals, order-preserving on agreeing names)
  topic    the Halo file's header comment names what the Brawl class is
  mutual   mutual best (size, callees, ...) inside a string / topic family
  graph    callers / callees already paired (iterated rounds)
  calls, size, vslot, vcall  supporting only (size ratio is calibrated)
A pair needs at least two independent signals; the confidence is the combined
weighted score. Struct offsets are extracted on the Brawl side but not scored:
the Halo repo has no binary, and its source rarely spells offsets.

Only our own derived data is written. The reference repos are read from a
directory outside this tree (default: ../donors/{halo2,community-behaviors}
next to the checkout, or --halo / --skyrim / $HAVOK_HALO_DIR /
$HAVOK_SKYRIM_DIR); nothing from them is copied.

usage: python tools/havok_map.py [-o havok/havok_map.csv] [--quiet]
"""

import argparse
import csv
import math
import os
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from havok_reflect import Dol, s16  # noqa: E402

DEFAULT_DOL = ROOT / "orig" / "RSBE01_02" / "sys" / "main.dol"
CONFIG = ROOT / "config" / "RSBE01_02"
DEFAULT_OUT = ROOT / "havok" / "havok_map.csv"

# float constants that carry no information (and CodeWarrior's int<->float magic)
DULL_FLOATS = {0.0, 1.0}
CW_MAGIC = {4503601774854144.0, 4503599627370496.0}


# ==========================================================================
# CodeWarrior demangling (just enough: class, method, parameter count)
# ==========================================================================

SPECIAL = {"__ct": "<ctor>", "__dt": "<dtor>", "__as": "operator=", "__eq": "operator==",
           "__ne": "operator!=", "__pl": "operator+", "__mi": "operator-", "__ml": "operator*",
           "__dv": "operator/", "__apl": "operator+=", "__ami": "operator-=",
           "__amu": "operator*=", "__adv": "operator/=", "__vc": "operator[]",
           "__nw": "operator new", "__dl": "operator delete", "__nwa": "operator new[]",
           "__dla": "operator delete[]", "__cl": "operator()"}


class Demangled:
    def __init__(self, cls: Optional[str], method: str, nparams: Optional[int], const: bool):
        self.cls, self.method, self.nparams, self.const = cls, method, nparams, const

    def pretty(self) -> str:
        m = self.method
        if m == "<ctor>":
            m = (self.cls or "").split("::")[-1]
        elif m == "<dtor>":
            m = "~" + (self.cls or "").split("::")[-1]
        return f"{self.cls}::{m}" if self.cls else m


def _parse_name(s: str, i: int) -> Tuple[Optional[str], int]:
    m = re.match(r"\d+", s[i:])
    if not m:
        return None, i
    n = int(m.group())
    j = i + len(m.group())
    return s[j:j + n], j + n


def _parse_qual(s: str, i: int) -> Tuple[Optional[str], int]:
    if i < len(s) and s[i] == "Q" and i + 1 < len(s) and s[i + 1].isdigit():
        n = int(s[i + 1])
        i += 2
        parts = []
        for _ in range(n):
            p, i = _parse_name(s, i)
            if p is None:
                return None, i
            parts.append(p)
        return "::".join(parts), i
    return _parse_name(s, i)


def _parse_type(s: str, i: int) -> int:
    """Skip one mangled type at s[i]; return the index after it (or -1)."""
    while i < len(s) and s[i] in "CVUS":
        i += 1
    if i >= len(s):
        return -1
    c = s[i]
    if c in "PR":
        return _parse_type(s, i + 1)
    if c in "vbcsilxfdwer":
        return i + 1
    if c.isdigit() or c == "Q":
        _, j = _parse_qual(s, i)
        return j
    if c == "A":
        m = re.match(r"A\d+_", s[i:])
        return _parse_type(s, i + len(m.group())) if m else -1
    if c == "M":
        _, j = _parse_qual(s, i + 1)
        return _parse_type(s, j)
    if c == "F":
        i += 1
        while i < len(s) and s[i] != "_":
            i = _parse_type(s, i)
            if i < 0:
                return -1
        return _parse_type(s, i + 1)
    return -1


def _count_params(s: str, i: int) -> Optional[int]:
    if s[i:] in ("v", ""):
        return 0
    n = 0
    while i < len(s):
        if s[i] == "_":  # return type of a template function follows
            break
        i = _parse_type(s, i)
        if i < 0:
            return None
        n += 1
    return n


def demangle(sym: str) -> Optional[Demangled]:
    if sym.startswith(("fn_", "lbl_", "@")) or "__" not in sym[1:]:
        return None
    if sym.startswith("__"):
        k = sym.find("__", 2)
        if k < 0:
            return None
        base, rest = sym[:k], sym[k + 2:]
        base = SPECIAL.get(base, base)
    else:
        m = None
        depth = 0
        for k in range(1, len(sym) - 2):
            ch = sym[k]
            if ch == "<":
                depth += 1
            elif ch == ">":
                depth -= 1
            elif depth == 0 and sym.startswith("__", k) and (sym[k + 2].isdigit() or sym[k + 2] in "QF"):
                m = k
                break
        if m is None:
            return None
        base, rest = sym[:m], sym[m + 2:]
    cls = None
    i = 0
    if rest and (rest[0].isdigit() or rest[0] == "Q"):
        cls, i = _parse_qual(rest, 0)
    const = False
    if i < len(rest) and rest[i] == "C":
        const = True
        i += 1
    if i >= len(rest) or rest[i] != "F":
        return Demangled(cls, base, None, const)  # data or unusual
    return Demangled(cls, base, _count_params(rest, i + 1), const)


# ==========================================================================
# Brawl: units, symbols, disassembly
# ==========================================================================

def havok_units(configure: Path) -> Dict[str, str]:
    """unit path -> status, for the havok lib (between the HAVOK markers) and sora/ph."""
    out: Dict[str, str] = {}
    inside = False
    pat = re.compile(r'Object\((\w+),\s*"([^"]+)"')
    for line in configure.read_text(encoding="utf-8").splitlines():
        if "# HAVOK-BEGIN" in line:
            inside = True
        elif "# HAVOK-END" in line:
            inside = False
        m = pat.search(line)
        if m and (inside or m.group(2).startswith("sora/ph/")):
            out[m.group(2)] = m.group(1)
    return out


def text_ranges(splits: Path) -> Dict[str, List[Tuple[int, int]]]:
    out: Dict[str, List[Tuple[int, int]]] = defaultdict(list)
    unit = None
    for line in splits.read_text(encoding="utf-8").splitlines():
        if line and not line[0].isspace() and line.endswith(":"):
            unit = line[:-1]
        elif unit and line.strip().startswith(".text"):
            m = re.search(r"start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
            if m:
                out[unit].append((int(m.group(1), 16), int(m.group(2), 16)))
    return out


class Sym:
    __slots__ = ("name", "addr", "size", "scope", "section")

    def __init__(self, name, addr, size, scope, section):
        self.name, self.addr, self.size, self.scope, self.section = name, addr, size, scope, section


def load_syms(path: Path) -> List[Sym]:
    pat = re.compile(r"^(\S+) = (\.?[\w.]+):0x([0-9A-Fa-f]+); // (.*)$")
    out = []
    for line in path.read_text(encoding="utf-8").splitlines():
        m = pat.match(line)
        if not m:
            continue
        attrs = m.group(4)
        sm = re.search(r"size:0x([0-9A-Fa-f]+)", attrs)
        sc = re.search(r"scope:(\w+)", attrs)
        ty = re.search(r"type:(\w+)", attrs)
        out.append(Sym(m.group(1), int(m.group(3), 16), int(sm.group(1), 16) if sm else 0,
                       sc.group(1) if sc else "global", m.group(2) + ":" + (ty.group(1) if ty else "")))
    return out


def sda_bases(dol: Dol, syms: List[Sym]) -> Tuple[int, int]:
    """r2 / r13 from __init_registers (lis rN,hi ; ori/addi rN,rN,lo)."""
    f = next(s for s in syms if s.name == "__init_registers")
    regs = {}
    for pc in range(f.addr, f.addr + f.size, 4):
        ins = dol.u32(pc)
        op, rd, ra, imm = ins >> 26, (ins >> 21) & 31, (ins >> 16) & 31, ins & 0xFFFF
        if op == 15 and ra == 0:
            regs[rd] = imm << 16
        elif op == 24 and ra in (2, 13) and rd == ra and ra in regs:
            regs[ra] |= imm
        elif op == 14 and rd in (2, 13) and ra == rd and rd in regs:
            regs[rd] = (regs[rd] + s16(imm)) & 0xFFFFFFFF
    return regs[2], regs[13]


class Text:
    def __init__(self, dol: Dol):
        # the first 7 DOL header slots are the code sections
        hdr = struct.unpack_from(">18I18I18I", dol.data, 0)
        code = [(hdr[18 + i], hdr[18 + i] + hdr[36 + i]) for i in range(7) if hdr[36 + i]]
        self.lo = min(a for a, _ in code)
        self.hi = max(b for _, b in code)
        self.data_lo = min(hdr[18 + i] for i in range(7, 18) if hdr[36 + i])

    def is_text(self, v: int) -> bool:
        return self.lo <= v < self.hi and v % 4 == 0


def read_cstring(dol: Dol, addr: int) -> Optional[str]:
    out = bytearray()
    for k in range(200):
        b = dol.read(addr + k, 1)
        if b is None:
            return None
        if b == b"\0":
            break
        if not (32 <= b[0] < 127 or b[0] in (9, 10)):
            return None
        out += b
    else:
        return None
    return out.decode() if len(out) >= 4 else None


class BFunc:
    def __init__(self, unit, sym: Sym):
        self.unit, self.sym = unit, sym
        self.name, self.addr, self.size = sym.name, sym.addr, sym.size
        self.dm = demangle(sym.name)
        self.floats: Set[float] = set()
        self.strings: Set[str] = set()
        self.ints: Set[int] = set()
        self.offsets: Set[int] = set()
        self.callees: List[int] = []
        self.vcalls: Set[int] = set()
        self.data_refs: Set[int] = set()
        self.vslots: List[Tuple[int, int]] = []  # (vtable, slot)
        self.vclass: Optional[str] = None


def f32(v: float) -> float:
    try:
        return struct.unpack(">f", struct.pack(">f", v))[0]
    except OverflowError:
        return v


def disasm(fn: BFunc, dol: Dol, r2: int, r13: int, text: Text):
    hi: Dict[int, int] = {}
    prev_r12_disp = None
    end = fn.addr + fn.size
    for pc in range(fn.addr, end, 4):
        ins = dol.u32(pc)
        op, rd, ra, imm = ins >> 26, (ins >> 21) & 31, (ins >> 16) & 31, ins & 0xFFFF
        d = s16(imm)

        def absaddr():
            if ra == 2:
                return (r2 + d) & 0xFFFFFFFF
            if ra == 13:
                return (r13 + d) & 0xFFFFFFFF
            if ra in hi:
                return (hi[ra] + d) & 0xFFFFFFFF
            return None

        if op == 15:  # lis / addis
            if ra == 0:
                hi[rd] = (d << 16) & 0xFFFFFFFF
            else:
                hi.pop(rd, None)
            continue
        if op == 14:  # addi / li
            a = absaddr() if ra else None
            if ra == 0:
                if abs(d) >= 2:
                    fn.ints.add(d)
            elif a is not None:
                fn.data_refs.add(a)
                s = read_cstring(dol, a)
                if s:
                    fn.strings.add(s)
            hi.pop(rd, None)
            continue
        if op in (10, 11):  # cmplwi / cmpwi
            v = imm if op == 10 else d
            if abs(v) >= 2:
                fn.ints.add(v)
            continue
        if op == 24 and ra in hi:  # ori into a lis-built value
            hi.pop(ra, None)
            continue
        if op == 18:  # b / bl
            li = ins & 0x03FFFFFC
            if li & 0x02000000:
                li -= 0x04000000
            tgt = li if ins & 2 else pc + li
            if ins & 1 or not (fn.addr <= tgt < end):
                fn.callees.append(tgt)
            for r in range(0, 13):
                if ins & 1:
                    hi.pop(r, None)
            continue
        if op == 19 and ((ins >> 1) & 0x3FF) == 528 and prev_r12_disp is not None:  # bcctr
            if prev_r12_disp >= 8 and prev_r12_disp % 4 == 0:
                fn.vcalls.add((prev_r12_disp - 8) // 4)
            prev_r12_disp = None
            continue
        if 32 <= op <= 55:  # D-form loads / stores
            a = absaddr()
            if a is not None:
                fn.data_refs.add(a)
                if op in (48, 49):  # lfs
                    b = dol.read(a, 4)
                    if b:
                        fn.floats.add(abs(struct.unpack(">f", b)[0]))
                elif op in (50, 51):  # lfd
                    b = dol.read(a, 8)
                    if b:
                        v = abs(struct.unpack(">d", b)[0])
                        if v not in CW_MAGIC:
                            fn.floats.add(f32(v) if v < 3.4e38 else v)
            elif ra != 1 and d >= 0:
                fn.offsets.add(d)
            if op == 32 and rd == 12 and ra == 12:
                prev_r12_disp = d
            if op in (32, 33, 34, 35, 40, 41, 42, 43, 46, 48, 49, 50, 51) and op < 48:
                hi.pop(rd, None)
            continue
        if op == 31:
            xo = (ins >> 1) & 0x3FF
            if xo == 444:  # or / mr
                if rd in hi and rd == ((ins >> 11) & 31):
                    hi[ra] = hi[rd]
                else:
                    hi.pop(ra, None)
            elif xo != 467:  # not mtspr
                hi.pop(rd, None)
            continue
        if op in (20, 21, 23, 24, 25, 26, 27, 28, 29):
            hi.pop(ra, None)
        else:
            hi.pop(rd, None)
    fn.floats -= DULL_FLOATS


def find_vtables(dol: Dol, text: Text, funcs: List[BFunc], syms: List[Sym]) -> Dict[int, List[int]]:
    """vtable address -> function pointers (CodeWarrior layout: two header words, then slots)."""
    cands: Set[int] = set()
    for f in funcs:
        cands |= f.data_refs
    cands |= {s.addr for s in syms if s.name.startswith("__vt__")}
    data_lo = text.hi
    out: Dict[int, List[int]] = {}
    for a in sorted(cands):
        if a < data_lo or a % 4:
            continue
        try:
            w0, w1 = dol.u32(a), dol.u32(a + 4)
        except ValueError:
            continue
        if w1 != 0 or (w0 != 0 and text.is_text(w0)):
            continue
        slots = []
        p = a + 8
        while True:
            try:
                w = dol.u32(p)
            except ValueError:
                break
            if not text.is_text(w):
                break
            slots.append(w)
            p += 4
        if len(slots) >= 1:
            out[a] = slots
    return out


def class_tokens(name: str) -> List[str]:
    name = re.sub(r"<.*>", "", name)
    toks = re.findall(r"[A-Z]+(?![a-z])|[A-Z]?[a-z]+|\d+", name)
    return [norm_tok(t) for t in toks if t.lower() not in ("hk", "hkp", "hka")]


def norm_tok(t: str) -> str:
    t = t.lower()
    if len(t) > 3 and t.endswith("s") and not t.endswith("ss"):
        t = t[:-1]
    return t


# ==========================================================================
# Skyrim (Community-Behaviors): classes, methods, slots
# ==========================================================================

class SkMethod:
    def __init__(self, cls, name, nparams, virtual, slot, has_body, path):
        self.cls, self.name, self.nparams, self.virtual = cls, name, nparams, virtual
        self.slot, self.has_body, self.path = slot, has_body, path


def fold_hk(name: str) -> str:
    return re.sub(r"\bhk[pab](?=[A-Z])", "hk", name)


def strip_comments(src: str) -> str:
    src = re.sub(r"/\*.*?\*/", " ", src, flags=re.S)
    return src


def count_args(params: str) -> int:
    p = params.strip()
    if not p or p == "void":
        return 0
    depth = 0
    n = 1
    for ch in p:
        if ch in "<([":
            depth += 1
        elif ch in ">)]":
            depth -= 1
        elif ch == "," and depth == 0:
            n += 1
    return n


def match_brace(src: str, i: int) -> int:
    depth = 0
    for j in range(i, len(src)):
        if src[j] == "{":
            depth += 1
        elif src[j] == "}":
            depth -= 1
            if depth == 0:
                return j
    return len(src)


def parse_skyrim(root: Path) -> Dict[Tuple[str, str], List[SkMethod]]:
    """(class with hkp/hka folded to hk, method) -> its declarations / definitions, each with
    parameter count, virtual-ness, the "slot N" its header notes, and whether a body exists."""
    methods: Dict[Tuple[str, str], List[SkMethod]] = defaultdict(list)
    files = [p for p in root.rglob("*") if p.suffix in (".h", ".hpp", ".cpp", ".inl")]
    class_re = re.compile(r"\b(?:class|struct)\s+(?:HK_\w+\s+)?(hk\w+)\s*(?:final\s*)?(?::\s*(?:public|protected|private)?\s*([\w:<>]+)[^{;]*)?\{")
    # a definition: [return type] hkX::name(params) [const] [: inits] {  at the start of a line,
    # with balanced parameter parentheses (a call inside "if (...) {" is not one)
    def_re = re.compile(r"(?:^|\n)[ \t]*(?:[\w:<>,\*&]+[ \t\*&]+)*?\b(hk\w+)::(~?\w+)\s*\(((?:[^;{}()]|\([^()]*\))*)\)\s*(?:const\s*)?(?::[^{;]*)?\{")
    for p in files:
        try:
            raw = p.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        src = strip_comments(raw)
        rel = p.relative_to(root).as_posix()
        for m in class_re.finditer(src):
            cls = fold_hk(m.group(1))
            body_start = m.end() - 1
            body_end = match_brace(src, body_start)
            body = src[body_start + 1:body_end]
            # walk top-level statements only (skip nested class bodies and inline bodies)
            i = 0
            depth = 0
            buf_start = 0
            stmts = []
            while i < len(body):
                ch = body[i]
                if ch == "{":
                    if depth == 0:
                        stmts.append((buf_start, i, True))
                    depth += 1
                elif ch == "}":
                    depth -= 1
                    if depth == 0:
                        buf_start = i + 1
                elif ch == ";" and depth == 0:
                    stmts.append((buf_start, i, False))
                    buf_start = i + 1
                i += 1
            for a, b, has_body in stmts:
                text = body[a:b]
                # the "slot N" note sits in a // comment on the same line
                line_end = body.find("\n", b)
                trailer = body[b:line_end if line_end >= 0 else len(body)]
                slot_m = re.search(r"slot\s+(\d+)", trailer)
                text = re.sub(r"//[^\n]*", " ", text)
                text = re.sub(r"^\s*(public|private|protected)\s*:", " ", text.strip())
                text = text.strip()
                if not text or text.startswith(("using", "typedef", "friend", "enum", "class ", "struct ", "#")):
                    continue
                mm = re.search(r"(\bvirtual\s+)?(?:[\w:<>,\*&\s]*?)(~?\b[A-Za-z_]\w*|operator\s*\S+)\s*\(([^()]*(?:\([^)]*\)[^()]*)*)\)", text)
                if not mm:
                    continue
                name = mm.group(2)
                if name in ("HK_DECLARE_CLASS_ALLOCATOR", "HK_DECLARE_REFLECTION", "HK_FORCE_INLINE",
                            "HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR", "alignas", "static_assert"):
                    continue
                virt = bool(mm.group(1)) or "override" in text[mm.end():]
                if name == m.group(1) or name == "~" + m.group(1):
                    name = fold_hk(name)
                methods[(cls, name)].append(SkMethod(cls, name, count_args(mm.group(3)), virt,
                                                     int(slot_m.group(1)) if slot_m else None,
                                                     has_body, rel))
        if p.suffix == ".cpp":
            for m in def_re.finditer(src):
                cls, name = fold_hk(m.group(1)), m.group(2)
                if name.lstrip("~") == m.group(1):
                    name = fold_hk(name)
                na = count_args(m.group(3))
                lst = methods[(cls, name)]
                hit = [x for x in lst if x.nparams == na]
                if hit:
                    for x in hit:
                        x.has_body = True
                else:
                    lst.append(SkMethod(cls, name, na, False, None, True, rel))
    return methods


# ==========================================================================
# Halo 2: functions.csv + source
# ==========================================================================

class HFunc:
    def __init__(self, va, size, calls, source, status):
        self.va, self.size, self.calls, self.source, self.status = va, size, calls, source, status
        self.name = ""
        self.cls: Optional[str] = None
        self.nparams: Optional[int] = None
        self.floats: Set[float] = set()
        self.strings: Set[str] = set()
        self.ints: Set[int] = set()
        self.vcalls: Set[int] = set()
        self.tokens: Set[str] = set()
        self.ctok: Set[str] = set()  # class-name tokens
        self.mtok: Set[str] = set()  # method-name tokens
        self.vslot: Optional[int] = None
        self.virtual = False
        self.topic: Set[str] = set()  # words of the source file's header comment


HALO_DROP_TOKENS = {"lane", "a", "b", "c", "d", "s", "e", "function", "unknown", "retail", "t", "of",
                    "the", "to", "and", "by", "for", "with", "in", "on", "x", "y", "z", "slot", "method", "v"}

FLT_MACROS = {"FLT_MAX": 3.4028234663852886e38, "FLT_EPSILON": 1.1920928955078125e-07,
              "FLT_MIN": 1.1754943508222875e-38, "HK_REAL_EPSILON": 1.1920928955078125e-07}


TOPIC_STOP = {"havok", "third", "party", "physics", "library", "the", "and", "its", "with", "from", "for",
              "that", "this", "which", "into", "are", "each", "one", "two", "when", "what", "where", "how",
              "has", "have", "not", "but", "they", "them", "their", "then", "than", "own", "all", "any",
              "kept", "made", "used", "use", "uses", "both", "other", "same", "also", "only", "way", "way",
              "between", "against", "whether", "given", "more", "most", "set", "get", "per", "about"}
PLACEHOLDER_RE = re.compile(r"(?:class|function|unknown|s|c|struct|type|t|vtable|table)_[0-9a-f]{5,6}")


def halo_tokens(ident: str) -> Set[str]:
    out = set()
    ident = PLACEHOLDER_RE.sub(" ", ident)  # address-only placeholder names carry no meaning
    for t in re.split(r"[_:~\W]+", ident):
        if not t or re.fullmatch(r"[0-9a-f]{5,6}", t) or t.isdigit():
            continue
        for u in re.findall(r"[A-Z]+(?![a-z])|[A-Z]?[a-z]+", t) or [t]:
            u = norm_tok(u)
            if u not in HALO_DROP_TOKENS and len(u) > 1:
                out.add(u)
    return out


def parse_halo(root: Path) -> Dict[int, HFunc]:
    rows = list(csv.DictReader((root / "config" / "functions.csv").open(encoding="utf-8")))
    funcs: Dict[int, HFunc] = {}
    for r in rows:
        if r["owner"] != "third:havok":
            continue
        va = int(r["va"], 16)
        calls = [int(x, 16) for x in r["calls"].split()] if r["calls"] else []
        funcs[va] = HFunc(va, int(r["size"]), calls, r["source"], r["status"])
    # class -> (base, ordered virtual names) from every header/source
    classes: Dict[str, Tuple[Optional[str], List[str]]] = {}
    class_re = re.compile(r"\bclass\s+(\w+)\s*(?::\s*public\s+(\w+))?\s*\{")
    texts: Dict[str, str] = {}
    for p in list((root / "include").rglob("*.h")) + list((root / "src").rglob("*.cpp")) + list((root / "src").rglob("*.h")):
        try:
            texts[p.relative_to(root).as_posix()] = p.read_text(encoding="utf-8", errors="replace")
        except OSError:
            pass
    for t in texts.values():
        for m in class_re.finditer(t):
            body = t[m.end():match_brace(t, m.end() - 1)]
            virts = re.findall(r"\bvirtual\s+[^;(]*?(~?\w+)\s*\(", body)
            classes[m.group(1)] = (m.group(2), virts)

    def slot_of(cls: str, meth: str) -> Optional[int]:
        chain = []
        c, seen = cls, set()
        while c in classes and c not in seen:
            seen.add(c)
            chain.append(c)
            c = classes[c][0]
        table: List[str] = []
        for c in reversed(chain):
            for v in classes[c][1]:
                key = "~" if v.startswith("~") else v
                if key not in table:
                    table.append(key)
        key = "~" if meth.startswith("~") else meth
        return table.index(key) if key in table else None

    marker = re.compile(r"^//\s*@retail\s+0x([0-9a-fA-F]+)(.*)$", re.M)
    by_file: Dict[str, List[int]] = defaultdict(list)
    for f in funcs.values():
        if f.source:
            by_file[f.source].append(f.va)
    for src_path in by_file:
        t = texts.get(src_path)
        if t is None:
            continue
        ms = list(marker.finditer(t))
        head = []
        for line in t.splitlines():
            if not line.startswith("//"):
                break
            if "@flags" not in line:
                head.append(line[2:])
        topic = {norm_tok(w) for w in re.findall(r"[A-Za-z]{3,}", " ".join(head))} - TOPIC_STOP
        for f in (funcs[va] for va in by_file[src_path]):
            f.topic = topic
        for k, m in enumerate(ms):
            va = int(m.group(1), 16)
            f = funcs.get(va)
            if f is None:
                continue
            extra = m.group(2).strip()
            if extra:  # compiler-generated (destructor / deleting) noted on the marker line
                words = extra.split()
                f.name = " ".join(words)
                f.mtok = {"dt"}
                f.ctok = halo_tokens(words[-1]) if len(words) > 1 else set()
                f.tokens = f.mtok | f.ctok
                if len(words) > 1:
                    f.cls = words[-1]
                    f.vslot = slot_of(words[-1], "~")
                continue
            body = t[m.end():ms[k + 1].start() if k + 1 < len(ms) else len(t)]
            code = re.sub(r"//[^\n]*", "", body)
            code = re.sub(r"/\*.*?\*/", "", code, flags=re.S)
            sig = re.search(r"([\w:~]+)\s*\(([^()]*(?:\([^)]*\)[^()]*)*)\)\s*(?:const)?\s*(?::[^{]*)?\{", code)
            if sig:
                f.name = sig.group(1)
                f.nparams = count_args(sig.group(2))
                if "::" in f.name:
                    f.cls, meth = f.name.rsplit("::", 1)
                    f.ctok = halo_tokens(f.cls)
                    if meth == f.cls.split("::")[-1]:
                        f.mtok = {"ct"}
                    elif meth.startswith("~"):
                        f.mtok = {"dt"}
                    else:
                        f.mtok = halo_tokens(meth)
                    f.tokens = f.ctok | f.mtok
                    f.vslot = slot_of(f.cls, meth)
                    f.virtual = f.vslot is not None
                else:
                    f.mtok = halo_tokens(f.name)
                    f.tokens = set(f.mtok)
                code_body = code[sig.end():]
            else:
                code_body = code
            for fm in re.finditer(r"(?<![\w.])(\d+\.\d*|\.\d+|\d+)(?:[eE]([+-]?\d+))?f\b|(?<![\w.])(\d+\.\d*(?:[eE][+-]?\d+)?|\d+[eE][+-]?\d+)(?![\w.])", code_body):
                txt = fm.group(0).rstrip("fF")
                try:
                    v = abs(f32(float(txt)))
                except ValueError:
                    continue
                f.floats.add(v)
            for name, v in FLT_MACROS.items():
                if name in code_body:
                    f.floats.add(f32(v))
            f.floats -= DULL_FLOATS
            for sm in re.finditer(r'"((?:[^"\\\n]|\\.){4,})"', code_body):
                st = sm.group(1)
                if re.search(r"\.(h|cpp|inl)$", st):
                    continue  # an #include, not a string the code uses
                st = re.sub(r"\\(.)", lambda e: {"n": "\n", "t": "\t", "0": "\0"}.get(e.group(1), e.group(1)), st)
                f.strings.add(st)
            for im in re.finditer(r"(?<![\w.\[])(0x[0-9a-fA-F]+|\d+)(?![\w.\]])", code_body):
                try:
                    v = int(im.group(1), 0)
                except ValueError:
                    continue
                if 2 <= v < 0x10000:
                    f.ints.add(v)
            for vm in re.finditer(r"slot_(\d+)\s*\(", code_body):
                f.vcalls.add(int(vm.group(1)))

    # per class: the virtual table as the headers declare it, slot -> defining function
    defined: Dict[Tuple[str, str], int] = {}
    for f in funcs.values():
        if f.name.startswith("deleting "):
            defined[(f.name.split()[-1], "~")] = f.va  # MSVC's vtable holds the deleting dtor
        elif f.name.startswith("destructor "):
            defined.setdefault((f.name.split()[-1], "~"), f.va)
        elif f.cls and "::" in f.name:
            meth = f.name.rsplit("::", 1)[1]
            defined.setdefault((f.cls, "~" if meth.startswith("~") else meth), f.va)
    tables: Dict[str, List[Tuple[str, Optional[int]]]] = {}
    for cls, (base, virts) in classes.items():
        if not virts:
            continue
        chain, c, seen = [], cls, set()
        while c in classes and c not in seen:
            seen.add(c)
            chain.append(c)
            c = classes[c][0]
        keys: List[str] = []
        for c in reversed(chain):
            for v in classes[c][1]:
                key = "~" if v.startswith("~") else v
                if key not in keys:
                    keys.append(key)
        row = []
        for key in keys:
            va = next((defined[(c, key)] for c in chain if (c, key) in defined), None)
            row.append((key, va))
        tables[cls] = row
    HALO_TABLES.clear()
    HALO_TABLES.update(tables)
    return funcs


HALO_TABLES: Dict[str, List[Tuple[str, Optional[int]]]] = {}


# ==========================================================================
# Matching
# ==========================================================================

def idf_weights(sets_a, sets_b):
    df = Counter()
    for s in list(sets_a) + list(sets_b):
        df.update(s)
    return {k: 1.0 / math.log2(2 + v) for k, v in df.items()}


def wjacc(a: Set, b: Set, w: Dict) -> Optional[float]:
    if not a and not b:
        return None
    if not a or not b:
        return 0.0
    inter = sum(w.get(x, 1.0) for x in a & b)
    union = sum(w.get(x, 1.0) for x in a | b)
    return inter / union if union else 0.0


def tok_match(a: str, b: str) -> bool:
    """equal, or one an abbreviation (prefix, >= 3 letters) of the other: caps/capsule, tri/triangle"""
    if a == b:
        return True
    if len(a) >= 3 and len(b) >= 3 and a[0] == b[0]:
        return a.startswith(b) or b.startswith(a)
    return False


def fuzzy_jacc(a: Set[str], b: Set[str], w: Dict) -> Optional[float]:
    if not a and not b:
        return None
    if not a or not b:
        return 0.0
    hit_a = {x for x in a if any(tok_match(x, y) for y in b)}
    hit_b = {y for y in b if any(tok_match(x, y) for x in a)}
    inter = (sum(w.get(x, 1.0) for x in hit_a) + sum(w.get(y, 1.0) for y in hit_b)) / 2
    union = inter + sum(w.get(x, 1.0) for x in a - hit_a) + sum(w.get(y, 1.0) for y in b - hit_b)
    return inter / union if union else 0.0


def string_tokens(st: str) -> Set[str]:
    st = re.sub(r"^[TLS]t(?=[A-Za-z])", "", st)  # monitor-stream timer prefixes (Tt/Lt/St)
    return {norm_tok(t) for t in re.findall(r"[A-Z]+(?![a-z])|[A-Z]?[a-z]+|\d+", st) if len(t) > 1 or t.isdigit()}


def string_sim(a: Set[str], b: Set[str]) -> Optional[float]:
    """best fuzzy match per string, averaged over the smaller set"""
    if not a and not b:
        return None
    if not a or not b:
        return 0.0
    small, big = (a, b) if len(a) <= len(b) else (b, a)
    tot = 0.0
    for x in small:
        tx = string_tokens(x)
        best = 0.0
        for y in big:
            if x == y:
                best = 1.0
                break
            v = fuzzy_jacc(tx, string_tokens(y), {})
            best = max(best, v or 0.0)
        tot += best
    return tot / max(len(small), 1) * (len(small) / len(big)) ** 0.25


WEIGHTS = {"float": 3.0, "str": 3.0, "int": 1.5, "name": 2.5, "cls": 1.5, "vtab": 2.0, "mutual": 1.5, "topic": 1.0, "vslot": 1.0, "vcall": 0.5,
           "calls": 1.0, "size": 1.0, "graph": 3.0}
# signals that count as independent evidence (vtable slot / virtual-call slots and
# callee count / size only support a pair, they never make one on their own)
STRONG = ("float", "str", "int", "name", "cls", "graph")


def strong_signals(sig: Dict[str, float]) -> List[str]:
    out = [k for k in STRONG if sig.get(k, 0) >= 0.5]
    if sig.get("int_shared", 0) < 2 and "int" in out:
        out.remove("int")
    if sig.get("topic", 0) >= 0.75:
        out.append("topic")  # the Halo file says it is about what the Brawl class is named for
    if sig.get("mutual"):
        out.append("mutual")  # mutual best inside a string / topic family
    if sig.get("vtab"):
        out.append("vtab")  # same slot of two aligned virtual tables
    if "cls" in out and "name" not in out and "vtab" not in out:
        out.remove("cls")  # a look-alike class alone says nothing about which method
    if sig.get("str", 0) >= 0.8 and sig.get("str_rare") and sig.get("size", 0) >= 0.5:
        out.append("str_unique")  # a string only these two functions use
    if sig.get("graph_sup", 0) >= 2:
        out.append("graph2")  # two anchor relations count twice
    elif "graph" in out and sig.get("calls", 0) >= 0.75 and sig.get("size", 0) >= 0.6:
        out.append("shape")  # one anchor relation, plus the same callee count and a fitting size
    return out


def pair_score(b: BFunc, h: HFunc, ctx, sup: int) -> Tuple[float, Dict[str, float]]:
    sig: Dict[str, float] = {}
    v = wjacc(b.floats, h.floats, ctx["wf"])
    if v is not None:
        sig["float"] = v
    v = string_sim(b.strings, h.strings)
    if v is not None:
        sig["str"] = v
        bdf, hdf = ctx["bdf"], ctx["hdf"]
        for x in b.strings:
            if bdf.get(x, 9) > 3:
                continue
            for y in h.strings:
                if hdf.get(y, 9) <= 2 and (x == y or (fuzzy_jacc(string_tokens(x), string_tokens(y), {}) or 0) >= 0.8):
                    sig["str_rare"] = 1
                    break
    if b.ints and h.ints:
        sig["int"] = wjacc(b.ints, h.ints, ctx["wi"])
        sig["int_shared"] = len(b.ints & h.ints)
    bc, bm = ctx["bctok"][b.addr], ctx["bmtok"][b.addr]
    if bc and h.ctok:
        sig["cls"] = fuzzy_jacc(bc, h.ctok, ctx["wt"])
    if bm and h.mtok and not (bm <= {"ct", "dt"} and h.mtok <= {"ct", "dt"}):
        sig["name"] = fuzzy_jacc(bm, h.mtok, ctx["wt"])
    elif bm and h.mtok and bm == h.mtok and sig.get("cls", 0) >= 0.5:
        sig["name"] = 1.0  # both ctors / both dtors of look-alike classes
    if (bm == {"ct"} and h.mtok == {"dt"}) or (bm == {"dt"} and h.mtok == {"ct"}):
        sig["veto"] = 1.0  # a constructor is never a destructor
    if h.vcalls and b.vcalls:
        sig["vcall"] = len(h.vcalls & b.vcalls) / len(h.vcalls | b.vcalls)
    bs = ctx["bslot"].get(b.addr)
    if bs is not None and h.vslot is not None and h.cls:
        # only comparable when the two classes look alike
        if sig.get("cls", 0) >= 0.3:
            sig["vslot"] = 1.0 if bs == h.vslot else (0.4 if abs(bs - h.vslot) == 1 else 0.0)
    bct = {x for x in bc if ctx["wtop"].get(x, 1.0) > 0.25} if bc else set()
    if len(bct) >= 2 and h.topic:
        hit = [x for x in bct if any(tok_match(x, y) for y in h.topic)]
        sig["topic"] = sum(ctx["wtop"].get(x, 1.0) for x in hit) / sum(ctx["wtop"].get(x, 1.0) for x in bct)
    if (b.addr, h.va) in ctx["vtab"]:
        sig["vtab"] = 1.0
    nb, nh = len(ctx["bcall"][b.addr]), len(ctx["hcall"][h.va])
    if nb or nh:
        sig["calls"] = 1.0 - abs(nb - nh) / max(nb, nh)
    r = b.size / max(h.size, 1)
    sig["size"] = max(0.0, 1.0 - abs(math.log(r / ctx["ratio"])) / 0.8)
    if sup:
        nbr = max(1, min(len(ctx["bcall"][b.addr]) + len(ctx["bcallers"][b.addr]),
                         len(ctx["hcall"][h.va]) + len(ctx["hcallers"][h.va])))
        sig["graph"] = min(1.0, sup / min(nbr, 3))
        sig["graph_sup"] = sup
    num = sum(WEIGHTS[k] * sig[k] for k in WEIGHTS if k in sig)
    tot = sum(WEIGHTS[k] for k in WEIGHTS if k in sig)
    score = num / tot if tot else 0.0
    if len(strong_signals(sig)) < 2:
        score *= 0.5
    if sig["size"] == 0.0:
        score *= 0.6
    if sig.get("veto"):
        score = 0.0
    return score, sig


def _lis(pairs: List[Tuple[int, int]]) -> List[Tuple[int, int]]:
    """longest chain of (i, j) increasing in both"""
    pairs = sorted(set(pairs))
    best: List[List[Tuple[int, int]]] = []
    for p in pairs:
        cand = [p]
        for chain in best:
            if chain[-1][0] < p[0] and chain[-1][1] < p[1] and len(chain) + 1 > len(cand):
                cand = chain + [p]
        best.append(cand)
    return max(best, key=len) if best else []


def align_vtables(bvt, bmap, halo, ctx, log) -> Dict[Tuple[int, int], str]:
    """Pair whole virtual tables. A Brawl vtable and a Halo class whose names look alike
    are aligned on the slots whose method names agree (an order-preserving chain of at
    least two); the slots between two agreeing ones are paired when the gap has the same
    length on both sides, or differs by one or two slots (Havok 4.0 and the 2004 version
    added/dropped virtuals) and one end of the gap fits the sizes clearly better.
    Returns (brawl fn, halo fn) -> 'BrawlClass~HaloClass'."""
    out: Dict[Tuple[int, int], str] = {}
    tables = {c: row for c, row in HALO_TABLES.items() if any(va in halo for _, va in row)}
    htok = {c: halo_tokens(c) for c in tables}
    ratio = ctx.get("ratio", 1.4)

    def fit(ba, ha):
        if ba not in bmap or ha not in halo:
            return 0.0
        r = bmap[ba].size / max(halo[ha].size, 1)
        return max(0.0, 1.0 - abs(math.log(r / ratio)) / 0.8)

    for vt, (cls, slots) in bvt.items():
        if not cls:
            continue
        bc = set(class_tokens(cls.split("::")[-1]))
        if not bc:
            continue
        best = None
        for hc, row in tables.items():
            if not htok[hc]:
                continue
            sim = fuzzy_jacc(bc, htok[hc], ctx["wt"]) or 0.0
            if sim < 0.5:
                continue
            anchors = []
            for i, ba in enumerate(slots):
                bm = ctx["bmtok"].get(ba)
                if not bm:
                    continue
                for j, (key, va) in enumerate(row):
                    hm = {"dt"} if key == "~" else halo_tokens(key)
                    if hm and (fuzzy_jacc(bm, hm, ctx["wt"]) or 0) >= 0.5:
                        anchors.append((i, j))
            chain = _lis(anchors)
            if len(chain) >= 2 and (best is None or (len(chain), sim) > (best[0], best[1])):
                best = (len(chain), sim, hc, chain)
        if best is None:
            continue
        _, sim, hc, chain = best
        row = tables[hc]
        tag = f"{cls}~{hc}"
        pts = [(-1, -1)] + chain + [(len(slots), len(row))]
        for (i, j) in chain:
            if row[j][1] in halo and slots[i] in bmap:
                out.setdefault((slots[i], row[j][1]), tag)
        for (i1, j1), (i2, j2) in zip(pts, pts[1:]):
            gb = list(range(i1 + 1, i2))
            gh = list(range(j1 + 1, j2))
            if not gb or not gh:
                continue
            if len(gb) == len(gh):
                options = [list(zip(gb, gh))]
            elif abs(len(gb) - len(gh)) <= 2:
                n = min(len(gb), len(gh))
                options = [list(zip(gb[:n], gh[:n])), list(zip(gb[-n:], gh[-n:]))]
            else:
                continue
            scored = [(sum(fit(slots[i], row[j][1]) for i, j in o) / len(o), o) for o in options]
            scored.sort(key=lambda t: -t[0])
            if len(scored) == 2 and scored[0][0] - scored[1][0] < 0.15:
                continue  # ambiguous which end lines up
            if scored[0][0] < 0.4:
                continue
            for i, j in scored[0][1]:
                if row[j][1] in halo and slots[i] in bmap:
                    out.setdefault((slots[i], row[j][1]), tag)
    log(f"  halo vtables: {len(out)} slot pairs from aligned classes")
    return out


def match_halo(bfuncs: List[BFunc], halo: Dict[int, HFunc], ctx, log):
    hlist = list(halo.values())
    # restrict call graphs to the Havok sets on each side
    baddr = {f.addr for f in bfuncs}
    ctx["bcall"] = {f.addr: [c for c in dict.fromkeys(f.callees) if c in baddr] for f in bfuncs}
    ctx["hcall"] = {h.va: [c for c in dict.fromkeys(h.calls) if c in halo] for h in hlist}
    bcallers, hcallers = defaultdict(set), defaultdict(set)
    for a, cs in ctx["bcall"].items():
        for c in cs:
            bcallers[c].add(a)
    for a, cs in ctx["hcall"].items():
        for c in cs:
            hcallers[c].add(a)
    ctx["wf"] = idf_weights([f.floats for f in bfuncs], [h.floats for h in hlist])
    ctx["ws"] = idf_weights([f.strings for f in bfuncs], [h.strings for h in hlist])
    ctx["bdf"], ctx["hdf"] = Counter(), Counter()
    for f in bfuncs:
        ctx["bdf"].update(f.strings)
    for h in hlist:
        ctx["hdf"].update(h.strings)
    ctx["wi"] = idf_weights([f.ints for f in bfuncs], [h.ints for h in hlist])
    ctx["wt"] = idf_weights([ctx["btok"][f.addr] for f in bfuncs], [h.tokens for h in hlist])
    # topic words: how many Halo files mention a word (class words like "agent" say little)
    files = {}
    for h in hlist:
        files[h.source] = h.topic
    tdf = Counter()
    for t in files.values():
        tdf.update(t)
    ctx["wtop"] = {k: 1.0 / math.log2(2 + v) for k, v in tdf.items()}

    # candidate pairs through an inverted index of the rarer features
    inv = defaultdict(set)
    for h in hlist:
        for x in h.floats:
            inv[("f", x)].add(h.va)
        for x in h.strings:
            for t in string_tokens(x):
                inv[("s", t[:3])].add(h.va)
        for x in h.tokens:
            if ctx["wt"].get(x, 1) > 0.2:
                inv[("t", x[:3])].add(h.va)
        for x in h.ints:
            if ctx["wi"].get(x, 1) > 0.25:
                inv[("i", x)].add(h.va)
        for x in h.topic:
            if ctx["wtop"].get(x, 1) > 0.25:
                inv[("p", x[:3])].add(h.va)
    cands: Dict[int, Set[int]] = defaultdict(set)
    for b in bfuncs:
        keys = [("f", x) for x in b.floats] + \
               [("s", t[:3]) for x in b.strings for t in string_tokens(x)] + \
               [("t", x[:3]) for x in ctx["btok"][b.addr]] + [("i", x) for x in b.ints]
        for k in keys:
            if len(inv.get(k, ())) <= 60:
                cands[b.addr] |= inv.get(k, set())
        # topic: Halo files whose header covers every informative word of the Brawl class
        bct = [x for x in ctx["bctok"][b.addr] if ctx["wtop"].get(x, 1.0) > 0.25]
        if len(bct) >= 2:
            sets = [inv.get(("p", x[:3]), set()) for x in bct]
            common = set.intersection(*sets) if sets else set()
            if len(common) <= 120:
                cands[b.addr] |= common
    bmap = {f.addr: f for f in bfuncs}
    ctx["bcallers"], ctx["hcallers"] = bcallers, hcallers
    ctx["vtab"] = align_vtables(ctx["bvt"], bmap, halo, ctx, log)
    for (ba, ha) in ctx["vtab"]:
        cands[ba].add(ha)

    accepted: Dict[int, Tuple[int, float, Dict]] = {}
    ctx["ratio"] = 1.4
    for rnd in range(10):
        graph_cands = defaultdict(set)
        for b, (h, sc, _) in accepted.items():
            bn = ctx["bcall"][b] + list(bcallers[b])
            hn = ctx["hcall"][h] + list(hcallers[h])
            if len(bn) <= 16 and len(hn) <= 16:
                for x in bn:
                    for y in hn:
                        graph_cands[x].add(y)
        scored = []
        for ba in set(cands) | set(graph_cands):
            b = bmap.get(ba)
            if b is None or b.size < 8:
                continue
            for ha in cands[ba] | graph_cands[ba]:
                h = halo[ha]
                sup = 0
                if rnd:
                    nh_out = ctx["hcall"][ha]
                    for x in ctx["bcall"][ba]:
                        if x in accepted and accepted[x][0] in nh_out:
                            sup += 1
                    for x in bcallers[ba]:
                        if x in accepted and accepted[x][0] in hcallers[ha]:
                            sup += 1
                sc, sig = pair_score(b, h, ctx, sup)
                scored.append((sc, ba, ha, sig))
        # a string family (the same monitor timer / message on both sides) names a group of
        # functions, not one; inside a family a pair counts when it is the mutual best by the
        # other signals with a clear margin on both sides
        fam = [(ba, ha, sig) for sc, ba, ha, sig in scored
               if sig.get("str", 0) >= 0.8 or sig.get("topic", 0) >= 0.75]
        sec = {}
        for ba, ha, sig in fam:
            parts = [(1.0, sig["size"])] + [(w, sig[k]) for k, w in (("calls", 1.0), ("name", 2.0), ("int", 1.0), ("float", 1.0), ("graph", 2.0)) if k in sig]
            sec[(ba, ha)] = sum(w * v for w, v in parts) / sum(w for w, _ in parts)
        by_b, by_h = defaultdict(list), defaultdict(list)
        for (ba, ha), v in sec.items():
            by_b[ba].append((v, ha))
            by_h[ha].append((v, ba))
        mutual = set()
        for (ba, ha), v in sec.items():
            if v < 0.5:
                continue
            lb = sorted(by_b[ba], reverse=True)
            lh = sorted(by_h[ha], reverse=True)
            if lb[0][1] != ha or lh[0][1] != ba:
                continue
            mb = v - (lb[1][0] if len(lb) > 1 else 0.0)
            mh = v - (lh[1][0] if len(lh) > 1 else 0.0)
            if mb >= 0.1 and mh >= 0.1:
                mutual.add((ba, ha))
        if os.environ.get("HAVOK_MAP_DEBUG"):
            topic_pairs = [(ba, ha) for ba, ha, sig in fam if sig.get("topic", 0) >= 0.75]
            log(f"    family pairs {len(fam)} (topic {len(topic_pairs)}, brawl fns {len({b for b, _ in topic_pairs})}), mutual {len(mutual)}")
        rescored = []
        for sc, ba, ha, sig in scored:
            if (ba, ha) in mutual:
                sig = dict(sig, mutual=1.0)
                num = sum(WEIGHTS[k] * sig[k] for k in WEIGHTS if k in sig)
                tot = sum(WEIGHTS[k] for k in WEIGHTS if k in sig)
                sc = num / tot
                if len(strong_signals(sig)) < 2:
                    sc *= 0.5
                if sig["size"] == 0.0:
                    sc *= 0.6
                if sig.get("veto"):
                    sc = 0.0
            if sc >= 0.30:
                rescored.append((sc, ba, ha, sig))
        scored = rescored
        # pairs accepted last round go first while they still qualify, so rounds only grow
        scored.sort(key=lambda t: (not (t[1] in accepted and accepted[t[1]][0] == t[2]), -t[0]))
        new: Dict[int, Tuple[int, float, Dict]] = {}
        used = set()
        for sc, ba, ha, sig in scored:
            if ba in new or ha in used:
                continue
            if sc < 0.45 or len(strong_signals(sig)) < 2:
                continue
            new[ba] = (ha, sc, sig)
            used.add(ha)
        # recalibrate the expected PPC/x86 size ratio from the confident pairs
        rs = sorted(bmap[b].size / max(halo[h].size, 1) for b, (h, sc, _) in new.items()
                    if sc >= 0.6 and bmap[b].size >= 64)
        if len(rs) >= 15:
            ctx["ratio"] = rs[len(rs) // 2]
        log(f"  halo round {rnd}: {len(new)} pairs, size ratio {ctx['ratio']:.2f}")
        if new.keys() == accepted.keys() and all(new[k][0] == accepted[k][0] for k in new):
            accepted = new
            break
        accepted = new
    return accepted


def match_skyrim(bfuncs, methods, log):
    """name match (hkp/hka folded) + arg count + vtable slot; slot-only for unnamed."""
    out: Dict[int, Tuple[str, float, str]] = {}
    by_cls = defaultdict(list)
    for (c, n), lst in methods.items():
        by_cls[c].extend(lst)

    # pass 1: name matches; collect the slot offset (Skyrim slot - Brawl slot) per Brawl class
    # and per hierarchy root. Brawl's slots 0/1 are the destructor and hkReferencedObject's
    # one extra virtual; Havok 2010 has two there, and each later layer can add or drop some,
    # so an offset is trusted per class only.
    cls_d = defaultdict(Counter)
    prelim = {}
    for b in bfuncs:
        if not b.dm or not b.dm.cls:
            continue
        cls = b.dm.cls.split("::")[-1] if "::" in b.dm.cls and not b.dm.cls.startswith("hk") else b.dm.cls
        cls = cls.split("<")[0]
        meth = b.dm.method
        if meth == "<ctor>":
            meth = cls
        elif meth == "<dtor>":
            meth = "~" + cls
        lst = methods.get((cls, meth))
        if not lst and "::" in (b.dm.cls or ""):
            last = b.dm.cls.split("::")[-1]
            lst = methods.get((last, meth if not meth.startswith(("~", cls)) else meth.replace(cls, last)))
        if not lst:
            continue
        prelim[b.addr] = (cls, meth, lst)
        bs = b.own_slot
        if bs is None or bs < 2 or b.vclass != b.dm.cls:
            continue
        for m in lst:
            if m.slot is not None and m.virtual:
                cls_d[cls][m.slot - bs] += 1

    def delta_for(cls, need=2):
        # per-root offsets proved unreliable (layers add virtuals), so only a class's own
        # named methods, two or more agreeing, set its offset
        c = cls_d.get(cls)
        if c and c.most_common(1)[0][1] >= need:
            return c.most_common(1)[0][0], "class"
        return None, None

    for b in bfuncs:
        if b.addr not in prelim:
            continue
        cls, meth, lst = prelim[b.addr]
        n = b.dm.nparams
        exact = [m for m in lst if n is not None and m.nparams == n]
        pick = (exact or lst)[0]
        conf = 0.55
        notes = []
        if exact:
            conf += 0.2
        else:
            notes.append(f"argc {n} vs {pick.nparams}")
        bs = b.own_slot
        if pick.slot is not None and pick.virtual and bs is not None and bs >= 2:
            d, how = delta_for(cls)
            if d is not None:
                if pick.slot - bs == d:
                    conf += 0.15
                    notes.append(f"slot {bs}~{pick.slot}")
                else:
                    conf -= 0.15
                    notes.append(f"slot {bs} vs {pick.slot} (offset {d:+d} by {how})")
        if pick.has_body:
            conf += 0.1
        else:
            notes.append("declared only")
            conf -= 0.25
        out[b.addr] = (f"{pick.cls}::{pick.name} @{pick.path}", round(max(0.05, min(conf, 1.0)), 2), "; ".join(notes))
    # pass 2: unnamed functions in a vtable -> the same slot of the Skyrim class, only where
    # the class offset rests on two or more named methods of that very class
    for b in bfuncs:
        if b.addr in out or b.dm is not None or b.vclass is None or b.own_slot is None or b.own_slot < 2:
            continue
        cls = b.vclass
        d, how = delta_for(cls, need=2)
        if d is None:
            continue
        want = b.own_slot + d
        hits = [m for m in by_cls.get(cls, []) if m.slot == want and m.virtual]
        if len(hits) == 1:
            m = hits[0]
            conf = 0.35 + (0.1 if m.has_body else -0.1)
            out[b.addr] = (f"{m.cls}::{m.name} @{m.path}", round(conf, 2),
                           f"vtable slot only ({cls} slot {b.own_slot}~{want})")
    log(f"  skyrim: {len(out)} matches, slot offsets for {len(cls_d)} classes")
    return out


# ==========================================================================
# main
# ==========================================================================

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--dol", type=Path, default=DEFAULT_DOL)
    ap.add_argument("--halo", type=Path, default=Path(os.environ.get("HAVOK_HALO_DIR", ROOT.parent / "donors" / "halo2")))
    ap.add_argument("--skyrim", type=Path, default=Path(os.environ.get("HAVOK_SKYRIM_DIR", ROOT.parent / "donors" / "community-behaviors")))
    ap.add_argument("-o", "--out", type=Path, default=DEFAULT_OUT)
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()
    log = (lambda *a: None) if args.quiet else (lambda *a: print(*a, file=sys.stderr))

    dol = Dol(args.dol)
    syms = load_syms(CONFIG / "symbols.txt")
    units = havok_units(ROOT / "configure.py")
    ranges = text_ranges(CONFIG / "splits.txt")
    r2, r13 = sda_bases(dol, syms)
    text = Text(dol)
    textsyms = sorted((s for s in syms if s.section.startswith(".text") and s.section.endswith("function")),
                      key=lambda s: s.addr)
    by_addr = {s.addr: s for s in syms}

    bfuncs: List[BFunc] = []
    for unit in units:
        for lo, hi in ranges.get(unit, []):
            for s in textsyms:
                if lo <= s.addr < hi and s.size:
                    bfuncs.append(BFunc(unit, s))
    for f in bfuncs:
        disasm(f, dol, r2, r13, text)
    log(f"brawl: {len(bfuncs)} functions in {len(units)} units (r2={r2:#x} r13={r13:#x})")

    # vtables: own slot + class per function
    vts = find_vtables(dol, text, bfuncs, syms)
    faddr = {f.addr: f for f in bfuncs}
    vt_class: Dict[int, str] = {}
    for a, slots in vts.items():
        named = by_addr.get(a)
        cls = None
        if named and named.name.startswith("__vt__"):
            dm = demangle("x__" + named.name[6:] + "Fv")
            cls = dm.cls if dm else None
        if cls is None:
            cnt = Counter()
            for k, w in enumerate(slots):
                s = by_addr.get(w)
                dm = demangle(s.name) if s else None
                if dm and dm.cls:
                    cnt[dm.cls] += 3 if dm.method == "<dtor>" else 1
            cls = cnt.most_common(1)[0][0] if cnt else None
        vt_class[a] = cls
        for k, w in enumerate(slots):
            if w in faddr:
                faddr[w].vslots.append((a, k))
    for f in bfuncs:
        f.own_slot = None
        own = [(a, k) for a, k in f.vslots if vt_class.get(a) and f.dm and f.dm.cls == vt_class[a]]
        pick = own[0] if own else (min(f.vslots, key=lambda t: t[1]) if f.vslots else None)
        if pick:
            f.own_slot = pick[1]
            f.vclass = vt_class.get(pick[0])
    log(f"brawl: {len(vts)} vtables, {sum(1 for f in bfuncs if f.own_slot is not None)} functions in a vtable")

    # tokens for the Brawl side: class part and method part
    bctok, bmtok = {}, {}
    for f in bfuncs:
        c, mt = set(), set()
        cls = f.dm.cls if f.dm and f.dm.cls else f.vclass
        if cls:
            c = set(class_tokens(cls.split("::")[-1]))
        if f.name.startswith("dtor_"):
            mt = {"dt"}
        if f.dm:
            m = f.dm.method
            if m == "<ctor>":
                mt = {"ct"}
            elif m == "<dtor>":
                mt = {"dt"}
            elif not m.startswith("operator"):
                mt = set(class_tokens(m))
        bctok[f.addr] = {x for x in c if x}
        bmtok[f.addr] = {x for x in mt if x}
    btok = {a: bctok[a] | bmtok[a] for a in bctok}

    is_sora = lambda f: f.unit.startswith("sora/")
    hk_funcs = [f for f in bfuncs if not is_sora(f)]

    halo_res: Dict[int, Tuple[int, float, Dict]] = {}
    halo: Dict[int, HFunc] = {}
    if (args.halo / "config" / "functions.csv").exists():
        halo = parse_halo(args.halo)
        log(f"halo: {len(halo)} third:havok functions")
        bvt = {a: (vt_class.get(a), sl) for a, sl in vts.items()}
        ctx = {"bvt": bvt, "btok": btok, "bctok": bctok, "bmtok": bmtok, "bslot": {f.addr: f.own_slot for f in bfuncs if f.own_slot is not None}}
        halo_res = match_halo(hk_funcs, halo, ctx, log)
    else:
        log(f"halo: not found at {args.halo}, skipped")

    sky_res: Dict[int, Tuple[str, float, str]] = {}
    if (args.skyrim / "havok").is_dir():
        methods = parse_skyrim(args.skyrim / "havok")
        log(f"skyrim: {len(methods)} (class, method) names")
        sky_res = match_skyrim(hk_funcs, methods, log)
    else:
        log(f"skyrim: not found at {args.skyrim}, skipped")

    # ---------------------------------------------------------------- write
    args.out.parent.mkdir(parents=True, exist_ok=True)
    rows = []
    for f in sorted(bfuncs, key=lambda f: f.addr):
        notes = []
        status = units[f.unit]
        if status != "NonMatching":
            notes.append(f"unit {status}")
        if is_sora(f):
            notes.append("Sora wrapper (no donor expected)")
        if f.dm:
            notes.append(f.dm.pretty() + (f"/{f.dm.nparams}" if f.dm.nparams is not None else ""))
        elif f.vclass is not None:
            notes.append(f"in vtable of {f.vclass} slot {f.own_slot}")
        hm = hc = ""
        if f.addr in halo_res:
            ha, sc, sig = halo_res[f.addr]
            h = halo[ha]
            hm = f"0x{ha:x} {h.name or '?'} @{h.source}"
            hc = f"{sc:.2f}"
            strong = strong_signals(sig)
            notes.append("halo:" + "+".join(strong) + (f" [{h.status}]" if h.status != "matched" else ""))
        sm = scf = ""
        if f.addr in sky_res:
            sm, c, n = sky_res[f.addr]
            scf = f"{c:.2f}"
            if n:
                notes.append("skyrim: " + n)
        rows.append([f.unit, f.name, f"0x{f.addr:08X}", f"0x{f.size:X}",
                     inline_or_outline(f), hm, hc, sm, scf, "; ".join(notes)])
    with args.out.open("w", newline="", encoding="utf-8") as fh:
        w = csv.writer(fh, lineterminator="\n")
        w.writerow(["brawl_tu", "symbol", "address", "size", "inline_or_outline", "halo_match",
                    "halo_confidence", "skyrim_match", "skyrim_confidence", "notes"])
        w.writerows(rows)
    log(f"wrote {args.out} ({len(rows)} rows)")
    summary(rows)


def _squash(name: str) -> str:
    return re.sub(r"[^a-z0-9]", "", re.sub(r"^hk[pa]?(?=[A-Z])", "", name.split("<")[0]).lower())


def inline_or_outline(f: BFunc) -> str:
    """The linked DOL keeps one copy of each weak function, so this is a heuristic:
    'inline' = weak, a template instance, or a method of a class other than the one
    the unit is named for (a header inline emitted out of line here); 'outline' = a
    method of the unit's own class or a free function; 'unknown' = no name, no vtable."""
    if f.sym.scope == "weak":
        return "inline"
    cls = f.dm.cls if f.dm and f.dm.cls else f.vclass
    if f.dm and f.dm.cls is None:
        return "inline" if "<" in f.name.split("__")[0] else "outline"
    if not cls:
        return "unknown"
    if "<" in cls:
        return "inline"
    stem = Path(f.unit).stem
    if stem.endswith("Class") and stem != "hkClass":
        stem = stem[:-5]  # reflection units: hkFooClass.cpp
    stem = _squash(stem)
    own = _squash(cls.split("::")[0])
    if own and (own in stem or stem in own):
        return "outline"
    # an unnamed function only known through a (possibly derived) class's vtable: no verdict
    return "inline" if f.dm else "unknown"


def summary(rows):
    per = defaultdict(Counter)
    for r in rows:
        tu, size = r[0], int(r[3], 16)
        h, s = bool(r[5]), bool(r[7])
        k = "both" if h and s else "halo" if h else "skyrim" if s else "neither"
        for key in (tu, "TOTAL") + (("TOTAL havok lib",) if tu.startswith("havok/") else ()):
            per[key]["n"] += 1
            per[key]["b"] += size
            per[key][k + "_n"] += 1
            per[key][k + "_b"] += size
            if h:
                per[key]["H_n"] += 1
                per[key]["H_b"] += size
            if s:
                per[key]["S_n"] += 1
                per[key]["S_b"] += size
    cols = ["H", "S", "both", "neither"]
    hdr = f"{'unit':44} {'fns':>5} {'bytes':>7} | " + " | ".join(f"{c:>15}" for c in ["halo", "skyrim", "both", "neither"])
    print(hdr)
    print("-" * len(hdr))
    keys = sorted(k for k in per if not k.startswith("TOTAL"))
    for k in keys + ["TOTAL havok lib", "TOTAL"]:
        c = per[k]
        cells = " | ".join(f"{c[x + '_n']:>4} {c[x + '_b']:>7} B" for x in cols)
        print(f"{k[:44]:44} {c['n']:>5} {c['b']:>7} | {cells}")
    t = per["TOTAL havok lib"]
    print()
    print("havok lib (sora/ph excluded):")
    for x, label in zip(cols, ["Halo match", "Skyrim match", "both", "neither"]):
        print(f"{label:13}: {t[x + '_n']:5} fns ({100 * t[x + '_n'] / t['n']:5.1f}%)  "
              f"{t[x + '_b']:7} B ({100 * t[x + '_b'] / t['b']:5.1f}%)")


if __name__ == "__main__":
    main()
