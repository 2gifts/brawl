#!/usr/bin/env python3
"""Read Havok's reflection (hkClass / hkClassMember / hkClassEnum) out of main.dol.

Brawl links Havok 4.0's reflection tables into main.dol. Each hkXxxClass object is
built at startup by a static initializer (__sinit_*) that calls
hkClass::hkClass(name, parent, size, interfaces, numInterfaces, enums, numEnums,
members, numMembers, defaults); the member and enum tables it points at are
constant data. This tool replays those initializers, decodes the tables and uses
them three ways:

  dump  [CLASS...]       print the reflected layout (size, parent, member offsets)
  seed  CLASS...         print a C++ declaration for a class (a starting header)
  check [-o DIR]         write tests/havok_layout/<header>.cpp: for every reflected
                         class with a header definition, sizeof + offsetof + member
                         sizes as compile-time asserts, one file per header (each
                         header must compile on its own). configure.py compiles
                         them with cflags_havok, so a wrong layout breaks the build.

Only the executable is read (no Havok SDK material). Sizes and alignments per
member type come from the DOL's own hkClassMember::TYPE_PROPERTIES table.
Target: 32-bit big-endian PowerPC, CodeWarrior layout.
"""

import argparse
import re
import struct
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_DOL = ROOT / "orig" / "RSBE01_02" / "sys" / "main.dol"
DEFAULT_SYMBOLS = ROOT / "config" / "RSBE01_02" / "symbols.txt"
CTOR = "__ct__7hkClassFPCcPC7hkClassiPPC7hkClassiPC11hkClassEnumiPC13hkClassMemberiPCv"

# hkClassMember::Type, in table order (names match include/havok/hkClass.h).
TYPE_NAMES = [
    "VOID", "BOOL", "CHAR", "INT8", "UINT8", "INT16", "UINT16", "INT32", "UINT32",
    "INT64", "UINT64", "REAL", "VECTOR4", "QUATERNION", "MATRIX3", "ROTATION",
    "QSTRANSFORM", "MATRIX4", "TRANSFORM", "ZERO", "POINTER", "FUNCTIONPOINTER",
    "ARRAY", "INPLACEARRAY", "ENUM", "STRUCT", "SIMPLEARRAY", "HOMOGENEOUSARRAY",
    "VARIANT", "CSTRING", "ULONG", "FLAGS",
]
T = {n: i for i, n in enumerate(TYPE_NAMES)}

# C++ spelling of the basic member types (Havok's own names, as in TYPE_PROPERTIES).
CXX_BASIC = {
    T["BOOL"]: "hkBool", T["CHAR"]: "char", T["INT8"]: "s8", T["UINT8"]: "u8",
    T["INT16"]: "s16", T["UINT16"]: "u16", T["INT32"]: "s32", T["UINT32"]: "u32",
    T["INT64"]: "s64", T["UINT64"]: "u64", T["REAL"]: "hkReal",
    T["VECTOR4"]: "hkVector4", T["QUATERNION"]: "hkQuaternion",
    T["MATRIX3"]: "hkMatrix3", T["ROTATION"]: "hkRotation",
    T["QSTRANSFORM"]: "hkQsTransform", T["MATRIX4"]: "hkMatrix4",
    T["TRANSFORM"]: "hkTransform", T["CSTRING"]: "char*", T["VARIANT"]: "hkVariant",
    T["FUNCTIONPOINTER"]: "void*", T["ULONG"]: "u32",
}


# --------------------------------------------------------------------------
# DOL + symbols
# --------------------------------------------------------------------------

class Dol:
    def __init__(self, path: Path):
        self.data = path.read_bytes()
        hdr = struct.unpack_from(">18I18I18I", self.data, 0)
        offs, addrs, sizes = hdr[0:18], hdr[18:36], hdr[36:54]
        self.sections = [(a, o, s) for o, a, s in zip(offs, addrs, sizes) if s]

    def read(self, addr: int, size: int) -> Optional[bytes]:
        for a, o, s in self.sections:
            if a <= addr and addr + size <= a + s:
                return self.data[o + addr - a:o + addr - a + size]
        return None

    def u32(self, addr: int) -> int:
        b = self.read(addr, 4)
        if b is None:
            raise ValueError(f"address {addr:#x} not in DOL")
        return struct.unpack(">I", b)[0]

    def cstr(self, addr: int) -> str:
        out = bytearray()
        while True:
            b = self.read(addr + len(out), 1)
            if b is None or b == b"\0":
                return out.decode("ascii", "replace")
            out += b


def load_symbols(path: Path) -> Tuple[Dict[str, Tuple[int, int]], Dict[int, str]]:
    by_name: Dict[str, Tuple[int, int]] = {}
    by_addr: Dict[int, str] = {}
    pat = re.compile(r"^(\S+) = \.?[\w.]+:0x([0-9A-Fa-f]+);")
    size_pat = re.compile(r"size:0x([0-9A-Fa-f]+)")
    for line in path.read_text(encoding="utf-8").splitlines():
        m = pat.match(line)
        if not m:
            continue
        name, addr = m.group(1), int(m.group(2), 16)
        sm = size_pat.search(line)
        size = int(sm.group(1), 16) if sm else 0
        by_name[name] = (addr, size)
        by_addr.setdefault(addr, name)
    return by_name, by_addr


# --------------------------------------------------------------------------
# Static initializer replay (just enough PowerPC for argument set-up)
# --------------------------------------------------------------------------

def s16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def replay_ctor_calls(dol: Dol, start: int, size: int, ctor: int) -> List[List[Optional[int]]]:
    """Return the argument lists (this, 7 register args, 3 stack args) of every
    hkClass ctor call made by the function at start."""
    regs: List[Optional[int]] = [None] * 32
    stack: Dict[int, Optional[int]] = {}
    calls = []
    for pc in range(start, start + size, 4):
        ins = dol.u32(pc)
        op = ins >> 26
        rd, ra = (ins >> 21) & 31, (ins >> 16) & 31
        imm = ins & 0xFFFF
        if op == 15:  # addis / lis
            base = 0 if ra == 0 else regs[ra]
            regs[rd] = None if base is None else (base + (s16(imm) << 16)) & 0xFFFFFFFF
        elif op == 14:  # addi / li
            base = 0 if ra == 0 else regs[ra]
            regs[rd] = None if base is None else (base + s16(imm)) & 0xFFFFFFFF
        elif op == 24:  # ori: rA = rS | uimm
            regs[ra] = None if regs[rd] is None else regs[rd] | imm
        elif op == 36:  # stw rS, d(rA)
            if ra == 1:
                stack[s16(imm)] = regs[rd]
        elif op == 37:  # stwu
            pass
        elif op == 32:  # lwz
            base = regs[ra] if ra else 0
            val = None
            if base is not None and ra != 1:
                b = dol.read((base + s16(imm)) & 0xFFFFFFFF, 4)
                val = struct.unpack(">I", b)[0] if b else None
            regs[rd] = val
        elif op == 31 and ((ins >> 1) & 0x3FF) == 444:  # or / mr
            rb = (ins >> 11) & 31
            regs[ra] = regs[rd] if rd == rb else None
        elif op == 18:  # b / bl
            li = ins & 0x03FFFFFC
            if li & 0x02000000:
                li -= 0x04000000
            target = li if ins & 2 else pc + li
            if ins & 1:
                if target == ctor:
                    calls.append(regs[3:11] + [stack.get(8), stack.get(12), stack.get(16)])
                for r in [0] + list(range(3, 13)):
                    regs[r] = None
        elif op in (19,):  # mflr/mtlr/blr etc: no GPR result we track (mflr is op 31)
            pass
        else:
            regs[rd] = None  # conservative: assume the D field was written
    return calls


# --------------------------------------------------------------------------
# Reflection model
# --------------------------------------------------------------------------

class Member:
    def __init__(self, name, cls, enum, type_, subtype, carray, flags, offset):
        self.name, self.cls, self.enum = name, cls, enum
        self.type, self.subtype, self.carray = type_, subtype, carray
        self.flags, self.offset = flags, offset


class Enum:
    def __init__(self, addr, name, items):
        self.addr, self.name, self.items = addr, name, items


class Klass:
    def __init__(self, addr, name, parent, size, num_ifaces, enums, members, defaults, sym):
        self.addr, self.name, self.parent, self.size = addr, name, parent, size
        self.num_ifaces, self.enums, self.members = num_ifaces, enums, members
        self.defaults, self.sym = defaults, sym


class Reflection:
    def __init__(self, dol_path: Path = DEFAULT_DOL, sym_path: Path = DEFAULT_SYMBOLS):
        self.dol = Dol(dol_path)
        self.syms, self.sym_at = load_symbols(sym_path)
        tp_addr, _ = self.syms["TYPE_PROPERTIES__13hkClassMember"]
        self.type_size: Dict[int, int] = {}
        self.type_align: Dict[int, int] = {}
        for i in range(len(TYPE_NAMES)):
            raw = self.dol.read(tp_addr + i * 12, 12)
            if raw is None:
                break
            t, _name, size, align = struct.unpack(">iIhh", raw)
            if i == T["FUNCTIONPOINTER"] or t == i:
                self.type_size[i], self.type_align[i] = size, align
        self.enums_at: Dict[int, Enum] = {}
        self.classes: Dict[str, Klass] = {}
        self.class_at: Dict[int, Klass] = {}
        self._load()

    def _enum(self, addr: int) -> Enum:
        if addr not in self.enums_at:
            name_p, items_p, n = struct.unpack(">IIi", self.dol.read(addr, 12))
            items = []
            for i in range(n):
                v, np = struct.unpack(">iI", self.dol.read(items_p + i * 8, 8))
                items.append((v, self.dol.cstr(np)))
            self.enums_at[addr] = Enum(addr, self.dol.cstr(name_p), items)
        return self.enums_at[addr]

    def _load(self):
        ctor, _ = self.syms[CTOR]
        raw_calls = []
        for name, (addr, size) in self.syms.items():
            if name.startswith("__sinit_") and size:
                raw_calls += replay_ctor_calls(self.dol, addr, size, ctor)
        pending = []
        for args in raw_calls:
            this, name_p, parent, size, _ifaces, n_if, enums_p, n_enums, mem_p, n_mem, defaults = args
            if None in (this, name_p, size, n_mem, n_enums):
                raise RuntimeError(f"could not replay hkClass ctor args: {args}")
            name = self.dol.cstr(name_p)
            enums = [self._enum(enums_p + i * 12) for i in range(n_enums or 0)]
            sym = self.sym_at.get(this, "")
            k = Klass(this, name, parent or 0, size, n_if or 0, enums, [], defaults or 0, sym)
            pending.append((k, mem_p, n_mem))
            # Key by symbol: hkClassVersion1Class also calls itself "hkClass" (an older
            # layout of hkClass kept for versioning), so names are not unique.
            key = sym[:-5] if sym.endswith("Class") else name
            self.classes[key] = k
            self.class_at[this] = k
        for k, mem_p, n_mem in pending:
            for i in range(n_mem):
                raw = self.dol.read(mem_p + i * 0x14, 0x14)
                name_p, cls_p, enum_p, t, st, carr, flags, off = struct.unpack(">IIIBBhHH", raw)
                cls = self.class_at.get(cls_p) if cls_p else None
                if cls_p and cls is None:
                    raise RuntimeError(f"{k.name}: member class {cls_p:#x} not reflected")
                enum = self._enum(enum_p) if enum_p else None
                k.members.append(Member(self.dol.cstr(name_p), cls, enum, t, st, carr, flags, off))
        for k in self.classes.values():
            k.parent = self.class_at.get(k.parent) if k.parent else None

    # ---- layout helpers (mirror hkClassMember::getSizeInBytes / getAlignment) ----

    def member_size(self, m: Member) -> int:
        t = m.subtype if m.type == T["ZERO"] else m.type
        n = m.carray or 1
        if t == T["ENUM"]:
            return n * m.flags // 8
        if t == T["STRUCT"]:
            return n * m.cls.size
        return n * self.type_size.get(t, -1)

    def member_align(self, m: Member) -> int:
        t = m.subtype if m.type == T["ZERO"] else m.type
        if t == T["ENUM"]:
            return m.flags >> 3
        if t == T["STRUCT"]:
            return max([1] + [self.member_align(x) for x in self.all_members(m.cls)])
        return self.type_align.get(t, 1)

    def all_members(self, k: Klass) -> List[Member]:
        chain = []
        while k:
            chain.append(k)
            k = k.parent
        out: List[Member] = []
        for c in reversed(chain):
            out += c.members
        return out

    # ---- C++ spelling ----

    def cxx_type(self, m: Member) -> str:
        t, st = m.type, m.subtype
        if t == T["ZERO"]:
            t, st = st, 0
        if t == T["STRUCT"]:
            return m.cls.name
        if t == T["POINTER"]:
            if st == T["STRUCT"]:
                return f"{m.cls.name}*"
            if st == T["CHAR"]:
                return "char*"
            return f"{CXX_BASIC.get(st, 'void')}*"
        if t == T["ARRAY"]:
            return f"hkArray<{self._elem(m)}>"
        if t == T["ENUM"]:
            return {1: "s8", 2: "s16", 4: "s32"}.get(m.flags // 8, "s32")
        return CXX_BASIC.get(t, "void*")

    def _elem(self, m: Member) -> str:
        st = m.subtype
        if st == T["STRUCT"]:
            return m.cls.name
        if st == T["POINTER"]:
            return f"{m.cls.name}*" if m.cls else "void*"
        return CXX_BASIC.get(st, "void*")

    def cxx_fields(self, m: Member) -> List[Tuple[str, str, int, int]]:
        """(type, field, offset, size) of the C++ fields one reflected member covers."""
        base = "m_" + m.name
        cap = m.name[:1].upper() + m.name[1:]
        if m.type == T["SIMPLEARRAY"]:
            return [(f"{self._elem(m)}*", base, m.offset, 4), ("int", f"m_num{cap}", m.offset + 4, 4)]
        if m.type == T["HOMOGENEOUSARRAY"]:
            return [("const hkClass*", f"{base}Class", m.offset, 4),
                    ("void*", base, m.offset + 4, 4),
                    ("int", f"m_num{cap}", m.offset + 8, 4)]
        return [(self.cxx_type(m), base, m.offset, self.member_size(m))]


# --------------------------------------------------------------------------
# Header discovery
# --------------------------------------------------------------------------

DEF_RE = re.compile(r"^\s*(?:struct|class)\s+(?:__attribute__\(\(.*?\)\)\s+)?(hk\w+)\b(?![^\n]*;)[^\n]*$", re.M)


def find_definitions(names) -> Dict[str, List[Path]]:
    """Header (and source) files that define a given class, by a simple scan."""
    found: Dict[str, List[Path]] = {n: [] for n in names}
    for top in ("include", "src"):
        for p in sorted((ROOT / top).rglob("*")):
            if p.suffix not in (".h", ".hpp", ".cpp") or p.name.endswith("Class.cpp"):
                continue
            text = p.read_text(encoding="utf-8", errors="replace")
            for m in DEF_RE.finditer(text):
                if m.group(1) in found:
                    found[m.group(1)].append(p.relative_to(ROOT))
    return found


# --------------------------------------------------------------------------
# Commands
# --------------------------------------------------------------------------

def cmd_dump(r: Reflection, names: List[str]):
    for key, k in sorted(r.classes.items()):
        if names and key not in names:
            continue
        par = k.parent.name if k.parent else "-"
        alias = f" (reflects as {k.name})" if key != k.name else ""
        print(f"{key}{alias}  size={k.size:#x} parent={par} ifaces={k.num_ifaces}"
              f" defaults={'yes' if k.defaults else 'no'} @{k.addr:#x}")
        for e in k.enums:
            print(f"  enum {e.name}: " + ", ".join(f"{n}={v}" for v, n in e.items))
        for m in k.members:
            t = TYPE_NAMES[m.type] + ("/" + TYPE_NAMES[m.subtype] if m.subtype else "")
            extra = f" [{m.carray}]" if m.carray else ""
            if m.cls:
                extra += f" class={m.cls.name}"
            if m.enum:
                extra += f" enum={m.enum.name}"
            if m.flags:
                extra += f" flags={m.flags:#x}"
            print(f"  {m.offset:#06x} {r.member_size(m):#06x} {m.name:<32} {t}{extra}")


def align_up(v: int, a: int) -> int:
    return (v + a - 1) // a * a


def class_align(r: Reflection, k: Klass) -> int:
    """Natural CodeWarrior alignment of a reflected class: its widest member (4 with a vtable)."""
    a = max([1] + [max(1, r.member_align(m)) for m in r.all_members(k)])
    root = k
    while root.parent:
        root = root.parent
    if root.size >= 4 and (not root.members or root.members[0].offset >= 4):
        a = max(a, 4)  # leading gap in the root class: the vtable pointer
    return a


def cmd_seed(r: Reflection, names: List[str]):
    """Print a starting declaration. Members are laid out naturally; a gap the natural layout does not
    produce becomes an unk field, and a size only reachable with 16-byte alignment gets the attribute."""
    for name in names:
        k = r.classes[name]
        par = f" : {k.parent.name}" if k.parent else ""
        nat_align = class_align(r, k)
        body = []
        for e in k.enums:
            body.append(f"    enum {e.name} {{")
            body += [f"        {n} = {v}," for v, n in e.items]
            body.append("    };")
        pos = k.parent.size if k.parent else 0
        if not k.parent and k.members and k.members[0].offset >= 4:
            body.append("    virtual ~%s();" % k.name)
            pos = 4
        for m in k.members:
            for ty, field, off, size in r.cxx_fields(m):
                align = 4 if m.type in (T["SIMPLEARRAY"], T["HOMOGENEOUSARRAY"]) else max(1, r.member_align(m))
                if align_up(pos, align) != off:
                    body.append(f"    u8 unk{pos:X}[{off - pos:#x}]; // {pos:#04x} not in the reflection")
                note = f" ({m.enum.name})" if m.enum else ""
                if m.type == T["ZERO"]:
                    note += " not serialized"
                arr = f"[{m.carray}]" if m.carray and m.type != T["SIMPLEARRAY"] else ""
                body.append(f"    {ty} {field}{arr}; // {off:#04x}{note}")
                pos = off + size
        attr = ""
        if align_up(pos, nat_align) != k.size:
            if align_up(pos, 16) == k.size:
                attr = " __attribute__((aligned(16)))"  # MWCC ignores it before the name
            else:
                body.append(f"    u8 unk{pos:X}[{k.size - pos:#x}]; // {pos:#04x} not in the reflection")
        print(f"// {k.name} ({k.size:#x} bytes). Layout and names from main.dol's {k.sym or k.name + 'Class'}.")
        print(f"struct {k.name}{par} {{")
        if body:
            print("\n".join(body))
        print(f"}}{attr};\n")


CHECK_HEADER = """// Generated by tools/havok_reflect.py check from main.dol's Havok reflection; do not edit.
// Compile-time layout check of <{header}>: never linked. configure.py compiles every file in
// this directory with cflags_havok, so a size or member offset that disagrees with the
// executable fails the build. Regenerate after changing which headers define these classes.

#include <{header}>

#define HK_LAYOUT(NAME, COND) typedef char NAME[(COND) ? 1 : -1]
#define HK_OFFSET(CLS, M) ((unsigned long)&((CLS*)0)->M)
#define HK_MSIZE(CLS, M) sizeof(((CLS*)0)->M)
"""

# Math types are basic member types in the reflection (no hkClass of their own); their
# size comes from hkClassMember::TYPE_PROPERTIES. Checked in the header that defines them.
MATH_TYPES = ["VECTOR4", "QUATERNION", "MATRIX3", "ROTATION", "QSTRANSFORM", "MATRIX4", "TRANSFORM"]


def check_lines(r: Reflection, name: str) -> List[str]:
    k = r.classes[name]
    out = [f"// {name}: {k.size:#x} bytes" + (f", parent {k.parent.name}" if k.parent else ""),
           f"HK_LAYOUT(size_{name}, sizeof({name}) == {k.size:#x});"]
    for m in k.members:
        for _ty, field, off, size in r.cxx_fields(m):
            tag = f"{name}_{field}"
            out.append(f"HK_LAYOUT(off_{tag}, HK_OFFSET({name}, {field}) == {off:#x});")
            if size > 0:
                out.append(f"HK_LAYOUT(size_{tag}, HK_MSIZE({name}, {field}) == {size:#x});")
    return out


def cmd_check(r: Reflection, out_dir: Optional[Path], skip: List[str]):
    math = {CXX_BASIC[T[t]]: (r.type_size[T[t]], r.type_align[T[t]]) for t in MATH_TYPES}
    defs = find_definitions(list(r.classes) + list(math))
    by_header: Dict[str, List[str]] = {}
    covered, missing, dup = [], [], []
    for name in sorted(list(r.classes) + list(math)):
        if name in r.classes and r.classes[name].name != name:
            continue  # versioning layouts (hkClassVersion1) have no C++ type of their own
        paths = [p for p in defs[name] if p.parts[0] == "include"]
        if len(defs[name]) > 1:
            dup.append((name, defs[name]))
        if not paths or name in skip:
            missing.append(name)
            continue
        by_header.setdefault(paths[0].relative_to("include").as_posix(), []).append(name)
        covered.append(name)
    files: Dict[str, str] = {}
    n_asserts = 0
    for header, names in sorted(by_header.items()):
        lines = [CHECK_HEADER.format(header=header)]
        for name in names:
            if name in math:
                size, align = math[name]
                body = [f"// {name}: basic member type, {size:#x} bytes, {align}-byte aligned",
                        f"HK_LAYOUT(size_{name}, sizeof({name}) == {size:#x});",
                        f"HK_LAYOUT(align_{name}, __alignof__({name}) == {align});"]
            else:
                body = check_lines(r, name)
            n_asserts += sum(1 for x in body if x.startswith("HK_LAYOUT"))
            lines += body + [""]
        files[Path(header).stem + ".cpp"] = "\n".join(lines)
    if out_dir:
        out_dir.mkdir(parents=True, exist_ok=True)
        for old in out_dir.glob("*.cpp"):
            if old.name not in files:
                old.unlink()
        for fname, text in files.items():
            p = out_dir / fname
            if not p.exists() or p.read_text(encoding="utf-8") != text:
                p.write_text(text, encoding="utf-8", newline="\n")
    else:
        for fname, text in files.items():
            sys.stdout.write(f"// ==== {fname}\n{text}\n")
    n_classes = len([c for c in covered if c not in math])
    print(f"havok_reflect: {len(r.classes)} reflected classes; {n_classes} classes + "
          f"{len(covered) - n_classes} math types checked in {len(files)} files "
          f"({n_asserts} asserts); {len(missing)} without a header", file=sys.stderr)
    if missing:
        print("  no header: " + " ".join(missing), file=sys.stderr)
    for name, paths in dup:
        print(f"  more than one definition: {name}: " + ", ".join(map(str, paths)), file=sys.stderr)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--dol", type=Path, default=DEFAULT_DOL)
    ap.add_argument("--symbols", type=Path, default=DEFAULT_SYMBOLS)
    sub = ap.add_subparsers(dest="cmd", required=True)
    d = sub.add_parser("dump")
    d.add_argument("names", nargs="*")
    s = sub.add_parser("seed")
    s.add_argument("names", nargs="+")
    c = sub.add_parser("check")
    c.add_argument("-o", "--output-dir", type=Path, default=ROOT / "tests" / "havok_layout")
    c.add_argument("--stdout", action="store_true", help="print instead of writing the files")
    c.add_argument("--skip", nargs="*", default=[], help="classes to leave out of the check")
    a = ap.parse_args()
    r = Reflection(a.dol, a.symbols)
    if a.cmd == "dump":
        cmd_dump(r, a.names)
    elif a.cmd == "seed":
        cmd_seed(r, a.names)
    else:
        cmd_check(r, None if a.stdout else a.output_dir, a.skip)


if __name__ == "__main__":
    main()
