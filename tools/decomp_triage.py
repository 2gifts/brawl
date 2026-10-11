#!/usr/bin/env python3
"""Read-only Brawl triage: queue, frames, and compiler-note catalogue.

Inspired by KakarottoCake/decomp-triage (CC0), especially stack-frame-diff.py
and levers.py. Brawl uses MWCC GC/3.0a5.2: Sunshine's GC/1.2.5 observations
are not automatically established facts for this compiler.

Examples:
  python tools/decomp_triage.py queue --report ../validation/baseline/report.json
  python tools/decomp_triage.py frames --unit ft_wario --output ../evidence/frames.json
  python tools/decomp_triage.py levers --symptom stack --output ../evidence/levers.json

No command builds, edits source, changes claims, or promotes Matching units.
Object diagnostics are hypotheses; decomp_build.py remains the full gate.
Generated output must stay outside the checkout. Missing drafts are unavailable.
"""
from __future__ import annotations

import argparse
import ast
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
import struct
import sys


class TriageError(Exception):
    pass


def load_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


def configured_sources(root: Path) -> dict[str, str]:
    """Inspect Object calls without importing/executing configure.py."""
    tree = ast.parse((root / "configure.py").read_text(encoding="utf-8"))
    result = {}
    for node in ast.walk(tree):
        if not isinstance(node, ast.Call) or not isinstance(node.func, ast.Name):
            continue
        if node.func.id != "Object" or len(node.args) < 2:
            continue
        status, source = node.args[:2]
        if isinstance(status, ast.Name) and isinstance(source, ast.Constant) and isinstance(source.value, str):
            previous = result.get(source.value)
            if previous is not None and previous != status.id:
                raise TriageError(f"Ambiguous configured status: {source.value}")
            result[source.value] = status.id
    return result


def behavior_priority(source: str) -> int:
    if "/ft/" in source or source.startswith("mo_fighter/"):
        return 0
    if source.startswith(("mo_stage/", "mo_item/")) or "/it/" in source:
        return 1
    return 2


def active_claims(text: str) -> list[str]:
    return [line.split("|")[1].strip() for line in text.splitlines()
            if len(line.split("|")) >= 4
            and not re.match(r"(?:done|abandoned|released)\b", line.split("|")[3].strip(), re.I)]


def matching_claims(source: str, claims: list[str]) -> list[str]:
    # Free-text subsystem claims cannot be inferred reliably. The queue still
    # requires a human/agent to inspect the board; this catches explicit paths,
    # filenames and module scopes, not every natural-language claim.
    module = source.split("/")[1] if source.startswith("mo_") and "/" in source else ""
    tokens = [source, "src/" + source, Path(source).name]
    if module:
        tokens.append(module)
    return [claim for claim in claims if any(re.search(r"(?<![\w])" + re.escape(token) + r"(?![\w])", claim)
                                            for token in tokens)]


def work_queue(root: Path, report: dict | None, claims: list[str]) -> list[dict]:
    by_source = {}
    for unit in (report or {}).get("units", []):
        source = unit.get("metadata", {}).get("source_path", "").replace("\\", "/")
        if source.startswith("src/"):
            source = source[4:]
        if source:
            by_source.setdefault(source, []).append(unit)
    rows = []
    for source, status in configured_sources(root).items():
        if status != "NonMatching":
            continue
        units = by_source.get(source, [])
        functions = [f for unit in units for f in unit.get("functions", [])]
        compared = [f for f in functions if "fuzzy_match_percent" in f]
        exact = [f for f in compared if float(f["fuzzy_match_percent"]) == 100.0]
        outstanding = [f for f in compared if float(f["fuzzy_match_percent"]) != 100.0]
        rows.append({"source": "src/" + source, "priority": behavior_priority(source),
                     "source_exists": (root / "src" / source).is_file(),
                     "claims": matching_claims(source, claims),
                     "reported_functions": len(functions), "compared_functions": len(compared),
                     "exact_functions": len(exact), "remaining_functions": len(outstanding),
                     "remaining_bytes": sum(int(f["size"]) for f in outstanding),
                     "comparison": "available" if compared else "unavailable"})
    return sorted(rows, key=lambda row: (bool(row["claims"]), row["priority"],
                                        not row["source_exists"], row["comparison"] != "available",
                                        -row["remaining_bytes"], row["source"]))


def read_functions(path: Path) -> dict[str, dict]:
    """Read bounded ELF32 big-endian PPC functions and their RELA/REL entries.

    The parser structure follows decomp-triage's CC0 stack-frame-diff.py.
    Relocation signatures are compared too: identical raw instruction bytes
    alone must not be labelled an exact function match.
    """
    data = path.read_bytes()

    def unpack(fmt, offset):
        size = struct.calcsize(fmt)
        if offset < 0 or offset + size > len(data):
            raise TriageError(f"Truncated ELF: {path}")
        return struct.unpack_from(fmt, data, offset)

    def region(offset, size):
        if offset < 0 or size < 0 or offset + size > len(data):
            raise TriageError(f"Invalid ELF extent: {path}")
        return data[offset:offset + size]

    if len(data) < 52 or data[:6] != b"\x7fELF\x01\x02" or unpack(">H", 18)[0] != 20:
        raise TriageError(f"Expected ELF32 big-endian PowerPC: {path}")
    section_offset = unpack(">I", 32)[0]
    section_stride, section_count = unpack(">HH", 46)
    if section_stride < 40 or not section_count:
        raise TriageError(f"Unsupported ELF section table: {path}")
    sections = [unpack(">10I", section_offset + i * section_stride) for i in range(section_count)]
    tables = {}
    for index, sec in enumerate(sections):
        if sec[1] != 2:  # SHT_SYMTAB
            continue
        if sec[6] >= section_count or sec[9] < 16 or sec[5] % sec[9]:
            raise TriageError(f"Invalid ELF symbol table: {path}")
        strings = sections[sec[6]]
        string_data = region(strings[4], strings[5])
        symbols = []
        for off in range(sec[4], sec[4] + sec[5], sec[9]):
            name, value, size, info, _, owner = unpack(">IIIBBH", off)
            if name >= len(string_data) or b"\0" not in string_data[name:]:
                raise TriageError(f"Invalid ELF symbol name: {path}")
            label = string_data[name:string_data.index(b"\0", name)].decode("utf-8", "replace")
            symbols.append((label, value, size, info, owner))
        tables[index] = symbols
    relocations = {}
    for sec in sections:
        if sec[1] not in (4, 9):  # SHT_RELA / SHT_REL
            continue
        stride = 12 if sec[1] == 4 else 8
        if sec[6] not in tables or sec[7] >= section_count or sec[9] != stride or sec[5] % stride:
            raise TriageError(f"Invalid ELF relocation table: {path}")
        for off in range(sec[4], sec[4] + sec[5], stride):
            address, info = unpack(">II", off)
            symbol_index, kind = info >> 8, info & 0xff
            if symbol_index >= len(tables[sec[6]]):
                raise TriageError(f"Invalid ELF relocation symbol: {path}")
            symbol = tables[sec[6]][symbol_index]
            # Anonymous section symbols need their owner: a blank name alone
            # could confuse relocations into completely different sections.
            target = symbol[0] or f"section:{symbol[4]}"
            addend = unpack(">i", off + 8)[0] if stride == 12 else None
            relocations.setdefault(sec[7], []).append((address, kind, target, addend))
    functions = {}
    for symbols in tables.values():
        for label, value, size, info, owner in symbols:
            if not label or info & 15 != 2 or not size or owner == 0 or owner >= section_count:
                continue
            host = sections[owner]
            if not host[2] & 4:
                continue
            relative = value - host[3]
            if relative < 0 or relative + size > host[5] or size % 4:
                raise TriageError(f"Invalid function extent for {label}: {path}")
            if label in functions:
                raise TriageError(f"Ambiguous function symbol {label}: {path}")
            functions[label] = {"code": region(host[4] + relative, size),
                                "relocations": sorted((address - value, kind, target, addend)
                                                      for address, kind, target, addend in relocations.get(owner, [])
                                                      if value <= address < value + size)}
    return functions


def words(code: bytes) -> tuple[int, ...]:
    return struct.unpack(">" + "I" * (len(code) // 4), code)


def signed16(word: int) -> int:
    value = word & 0xffff
    return value - 65536 if value & 0x8000 else value


def frame_size(code: bytes) -> int:
    for word in words(code)[:6]:
        if word & 0xffff0000 == 0x94210000 and signed16(word) < 0:
            return -signed16(word)
        # Do not scan into another basic block or mistake a later instruction
        # for the function's prologue.
        if word >> 26 in (16, 18, 19):
            break
    return 0


def frame_only(target: bytes, base: bytes) -> bool:
    delta = frame_size(target) - frame_size(base)
    if not delta or len(target) != len(base):
        return False
    for a, b in zip(words(target), words(base)):
        if a == b:
            continue
        if a >> 16 != b >> 16:
            return False
        op, ra = a >> 26, (a >> 16) & 31
        difference = signed16(a) - signed16(b)
        if a & 0xffff0000 == 0x94210000:
            if difference != -delta:
                return False
        elif ra == 1 and (op == 14 or 32 <= op <= 55):
            if difference != delta:
                return False
        else:
            # Literal, object-field, branch, rotate and unrecognized changes
            # must never be excused merely because their delta equals a frame.
            return False
    return True


def frame_report(root: Path, filters: list[str]) -> dict:
    rows, unavailable = [], []
    config = load_json(root / "objdiff.json")
    for unit in config["units"]:
        if filters and not any(part in unit["name"] for part in filters):
            continue
        paths = [root / unit.get(key, "") for key in ("target_path", "base_path")]
        if not all(path.is_file() for path in paths):
            unavailable.append({"unit": unit["name"], "reason": "missing target or compiled draft"})
            continue
        target, base = map(read_functions, paths)
        for label, original in target.items():
            draft = base.get(label)
            row = {"unit": unit["name"], "symbol": label, "target_size": len(original["code"])}
            if draft is None:
                row["bucket"] = "unpaired"
            else:
                a, b = original["code"], draft["code"]
                reloc_equal = original["relocations"] == draft["relocations"]
                row.update(base_size=len(b), target_frame=frame_size(a), base_frame=frame_size(b),
                           delta=frame_size(a) - frame_size(b), relocation_signatures_equal=reloc_equal)
                if a == b:
                    row["bucket"] = "raw-identical" if reloc_equal else "relocations-differ"
                elif len(a) != len(b):
                    row["bucket"] = "length-differs"
                elif not reloc_equal:
                    row["bucket"] = "code-and-relocations-differ"
                elif frame_only(a, b):
                    row["bucket"] = "frame-only-hypothesis"
                else:
                    row["bucket"] = "frame-and-code" if row["delta"] else "structural"
            rows.append(row)
    return {"notice": "Diagnostic only: raw equality and ELF relocation labels are not the full exact/link gate.",
            "compiler": "MWCC GC/3.0a5.2", "buckets": dict(Counter(row["bucket"] for row in rows)),
            "unavailable": unavailable, "functions": rows}


def compiler_notes(root: Path, symptom: str) -> list[dict]:
    tips = (root / "TIPS.md").read_text(encoding="utf-8")
    section, entries = "", []
    for paragraph in re.split(r"\n(?=- |## )", tips):
        if paragraph.startswith("## "):
            section = paragraph.splitlines()[0][3:]
            continue
        if not paragraph.startswith("- ") or symptom.lower() not in paragraph.lower():
            continue
        confidence = re.search(r"\*\*(HIGH|MED(?:-HIGH)?|LOW)\*\*", paragraph)
        entries.append({"compiler": "MWCC GC/3.0a5.2", "section": section,
                        "confidence": "project-note-" + (confidence.group(1).lower() if confidence else "unspecified"),
                        "text": paragraph[2:].strip(), "source": "TIPS.md",
                        "source_sha256": hashlib.sha256(tips.encode()).hexdigest(),
                        "notice": "Project observation; applicability still needs target assembly and measurement."})
    return entries


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--project", type=Path, default=Path(__file__).resolve().parents[1])
    sub = parser.add_subparsers(dest="command", required=True)
    queue = sub.add_parser("queue", help="Rank NonMatching sources, with unavailable drafts explicit")
    queue.add_argument("--report", type=Path)
    queue.add_argument("--claims", type=Path, help="External board claims snapshot (no credentials)")
    queue.add_argument("--limit", type=int, default=30)
    frames = sub.add_parser("frames", help="Compare PPC frames without editing or compiling source")
    frames.add_argument("--unit", action="append", default=[])
    levers = sub.add_parser("levers", help="Search Brawl's compiler observations without upgrading confidence")
    levers.add_argument("--symptom", default="")
    for command_parser in (queue, frames, levers):
        command_parser.add_argument("--output", type=Path)
    args = parser.parse_args(argv)
    try:
        root = args.project.resolve()
        if args.output:
            output = args.output.resolve()
            if output == root or root in output.parents:
                raise TriageError("Generated output must be outside the checkout")
            if output.exists():
                raise TriageError("Output already exists; choose a fresh evidence path")
        if args.command == "queue":
            report = load_json(args.report) if args.report else None
            claims = active_claims(args.claims.read_text(encoding="utf-8")) if args.claims else []
            result = {"notice": "Inspect board claims before claiming; free-text claims may not map to source paths.",
                      "report": str(args.report) if args.report else None,
                      "units": work_queue(root, report, claims)[:max(0, args.limit)]}
        elif args.command == "frames":
            result = frame_report(root, args.unit)
        else:
            result = compiler_notes(root, args.symptom)
        text = json.dumps(result, indent=2) + "\n"
        if args.output:
            output.parent.mkdir(parents=True, exist_ok=True)
            # Exclusive creation also prevents an accidental overwrite race.
            with output.open("x", encoding="utf-8") as stream:
                stream.write(text)
            print(f"Wrote {output}")
        else:
            print(text, end="")
        return 0
    except (OSError, ValueError, KeyError, SyntaxError, struct.error, TriageError) as exc:
        print(f"Triage failed: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
