#!/usr/bin/env python3

"""Report .text ranges that splits.txt does not claim yet.

Every translation unit has to be carved out of dtk's auto_* blobs before it can
be decompiled, and the hard part is proving where one unit ends. This looks for
holes between the .text ranges already in splits.txt, then reports which data
each hole references. Data that no other block touches pins the boundary down:
a hole whose constants belong to nobody else is a translation unit.

Run it after a build, so build/<version>/asm exists. Without --module it reads
the main DOL; --module names a REL (config/<version>/rels/<name>, assembly in
build/<version>/<name>/asm) and can be repeated, and --module all walks every
REL:

    python tools/split_gaps.py
    python tools/split_gaps.py --min-size 1024 --exclusive-only
    python tools/split_gaps.py --module ft_poke --module ft_marth
    python tools/split_gaps.py --module all --exclusive-only --min-size 512
"""

from argparse import ArgumentParser
from collections import defaultdict
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple
import os
import re

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.abspath(os.path.join(script_dir, ".."))

DATA_SECTIONS = ("sdata2", "sdata", "data", "rodata", "sbss", "bss")

Range = Tuple[int, int, str]
Owned = Tuple[str, int, int, bool, str]

SPLIT_UNIT = re.compile(r"^(\S+\.(?:cpp|c)):\s*$")
SPLIT_SECTION = re.compile(r"^\s+\.(\w+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)")
SYMBOL = re.compile(r"^(\S+) = \.(\w+):0x([0-9A-Fa-f]+);(.*)")
SIZE = re.compile(r"size:0x([0-9A-Fa-f]+)")
SDA_REF = re.compile(r"\b([A-Za-z_]\w*)@(?:sda21|ha|l)\b")
# dtk names a blob after its index and address, or after the function it starts
# with when that function sits alone (auto_fn_<module>_<offset>_text in a REL,
# auto_fn_<address>_text in the DOL).
AUTO_UNIT = re.compile(r"^auto_(?:\d+|fn(?:_\d+)?)_([0-9A-Fa-f]+)_text$")
# A blob can carry several sections (extab, .text, ...); each opens with this
# header, and only the .text one places the blob.
AUTO_RANGE = re.compile(r"^# 0x([0-9A-Fa-f]+)\.\.0x([0-9A-Fa-f]+)\b.*\n\.text\b", re.M)


def read_splits(path: Path) -> Dict[str, List[Range]]:
    sections: Dict[str, List[Range]] = defaultdict(list)
    unit: Optional[str] = None
    for line in path.read_text().splitlines():
        match = SPLIT_UNIT.match(line)
        if match:
            unit = match.group(1)
            continue
        match = SPLIT_SECTION.match(line)
        if match and unit:
            sections[match.group(1)].append(
                (int(match.group(2), 16), int(match.group(3), 16), unit)
            )
    for ranges in sections.values():
        ranges.sort()
    return sections


def read_symbols(path: Path) -> Tuple[Dict[str, Tuple[str, int]], List[Tuple[int, str, int]]]:
    by_name: Dict[str, Tuple[str, int]] = {}
    functions: List[Tuple[int, str, int]] = []
    for line in path.read_text().splitlines():
        match = SYMBOL.match(line)
        if not match:
            continue
        name, section, address = match.group(1), match.group(2), int(match.group(3), 16)
        by_name[name] = (section, address)
        if section == "text":
            size = SIZE.search(match.group(4))
            functions.append((address, name, int(size.group(1), 16) if size else 0))
    functions.sort()
    return by_name, functions


def read_asm_refs(
    asm_dir: Path, known: Set[str], text: List[Range]
) -> Tuple[Dict[str, Set[str]], Dict[str, Tuple[int, int]]]:
    """Symbol references per assembly file, skipping stale output.

    dtk leaves the .s of a unit behind when it disappears from splits.txt, and a
    leftover file would look like a second user of every constant it mentions,
    hiding the boundary. Only files that still correspond to a unit or to an
    auto_* block are counted. An auto_* block counts only while its range lies in
    a hole of the current .text split; when leftovers from an older layout
    overlap inside one hole, the newest file wins.
    """
    refs: Dict[str, Set[str]] = {}
    autos: List[Tuple[float, str, int, int, str]] = []
    for path in asm_dir.rglob("*.s"):
        if path.stem in known:
            refs[path.stem] = set(SDA_REF.findall(path.read_text()))
            continue
        if not AUTO_UNIT.match(path.stem):
            continue
        body = path.read_text()
        match = AUTO_RANGE.search(body)
        if not match:
            continue
        lo, hi = int(match.group(1), 16), int(match.group(2), 16)
        if any(start < hi and lo < end for start, end, _ in text):
            continue
        autos.append((path.stat().st_mtime, path.stem, lo, hi, body))

    spans: Dict[str, Tuple[int, int]] = {}
    for _, stem, lo, hi, body in sorted(autos, reverse=True):
        if any(start < hi and lo < end for start, end in spans.values()):
            continue
        spans[stem] = (lo, hi)
        refs[stem] = set(SDA_REF.findall(body))
    return refs, spans


def holes(ranges: List[Range]) -> List[Tuple[int, int, str, str]]:
    return [
        (ranges[i][1], ranges[i + 1][0], ranges[i][2], ranges[i + 1][2])
        for i in range(len(ranges) - 1)
        if ranges[i + 1][0] > ranges[i][1]
    ]


def owned_data(
    used: Set[str],
    elsewhere: Set[str],
    by_name: Dict[str, Tuple[str, int]],
    sections: Dict[str, List[Range]],
) -> List[Owned]:
    """Data the hole references, split into exclusive and shared.

    A hole can hold data from several units, so exclusivity is decided per
    symbol rather than per hole -- otherwise a single shared constant hides
    every boundary in the range.

    Data inside a range splits.txt already gives to a unit counts too when that
    unit never touches it and the hole is its only user: a unit split out before
    its neighbour often took the neighbour's data with it. Those entries carry
    the unit that holds them.
    """
    owned: List[Owned] = []
    for section in DATA_SECTIONS:
        ranges = sections.get(section, [])
        unclaimed = holes(ranges)
        mine = [
            name
            for name in used
            if by_name.get(name, ("", 0))[0] == section
            and any(lo <= by_name[name][1] < hi for lo, hi, _, _ in unclaimed)
        ]
        for names, exclusive in ((set(mine) - elsewhere, True), (set(mine) & elsewhere, False)):
            if not names:
                continue
            addresses = [by_name[name][1] for name in names]
            owned.append((section, min(addresses), max(addresses) + 4, exclusive, ""))

        claimed: Dict[str, List[int]] = defaultdict(list)
        for name in used - elsewhere:
            if by_name.get(name, ("", 0))[0] != section:
                continue
            address = by_name[name][1]
            for lo, hi, unit in ranges:
                if lo <= address < hi:
                    claimed[unit].append(address)
                    break
        for unit, addresses in sorted(claimed.items()):
            owned.append((section, min(addresses), max(addresses) + 4, True, unit))
    return owned


def report(
    config: Path, asm_dir: Path, args, title: Optional[str] = None
) -> int:
    sections = read_splits(config / "splits.txt")
    by_name, functions = read_symbols(config / "symbols.txt")
    known = {Path(unit).stem for ranges in sections.values() for _, _, unit in ranges}
    refs, spans = read_asm_refs(asm_dir, known, sections.get("text", []))

    text = sections.get("text", [])
    gaps = holes(text)
    # Blocks before the first or after the last named range are holes too.
    if spans:
        first = min(lo for lo, _ in spans.values())
        last = max(hi for _, hi in spans.values())
        if text and first < text[0][0]:
            gaps.insert(0, (first, text[0][0], "(start)", text[0][2]))
        if text and last > text[-1][1]:
            gaps.append((text[-1][1], last, text[-1][2], "(end)"))
        if not text:
            gaps = [(first, last, "(start)", "(end)")]

    reported = 0
    for lo, hi, before, after in gaps:
        owners = {stem for stem, (start, _) in spans.items() if lo <= start < hi}
        if not owners:
            continue
        size = hi - lo
        if size < args.min_size or (args.max_size and size > args.max_size):
            continue

        used: Set[str] = set().union(*(refs[owner] for owner in owners))
        elsewhere: Set[str] = set().union(
            *(names for stem, names in refs.items() if stem not in owners)
        )
        owned = owned_data(used, elsewhere, by_name, sections)
        if args.exclusive_only and not any(entry[3] for entry in owned):
            continue

        if title and reported == 0:
            print(f"== {title}")
        count = sum(1 for address, _, _ in functions if lo <= address < hi)
        print(f"0x{lo:08X}-0x{hi:08X}  {size:>7} bytes  {count:>4} functions")
        print(f"    between {before} and {after}")
        for stem in sorted(owners, key=lambda name: spans[name][0]):
            print(f"    {stem}.s")
        for section, start, end, exclusive, holder in owned:
            kind = "exclusive" if exclusive else "shared"
            if holder:
                kind += f", inside {holder}"
            print(f"    .{section:<7} 0x{start:08X}-0x{end:08X} {kind}")
        reported += 1
    return reported


def main() -> None:
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("--version", default="RSBE01_02", help="version to inspect")
    parser.add_argument(
        "--module",
        action="append",
        default=[],
        help="REL to inspect (repeatable; 'all' for every REL, 'main' for the DOL)",
    )
    parser.add_argument("--min-size", type=int, default=0, help="skip holes smaller than this")
    parser.add_argument("--max-size", type=int, default=0, help="skip holes larger than this")
    parser.add_argument(
        "--exclusive-only",
        action="store_true",
        help="only report holes that own data outright",
    )
    args = parser.parse_args()

    config = Path(root_dir, "config", args.version)
    build = Path(root_dir, "build", args.version)
    rels = sorted(path.name for path in (config / "rels").iterdir() if path.is_dir())

    modules: List[str] = []
    for name in args.module or ["main"]:
        if name == "all":
            modules.extend(rels)
        elif name == "main" or name in rels:
            modules.append(name)
        else:
            parser.error(f"unknown module {name}; expected main, all or one of config/{args.version}/rels")

    total = 0
    for name in modules:
        if name == "main":
            module_config, asm_dir = config, build / "asm"
        else:
            module_config, asm_dir = config / "rels" / name, build / name / "asm"
        if not asm_dir.is_dir():
            if len(modules) == 1:
                parser.error(f"{asm_dir} not found; build first")
            print(f"== {name}: {asm_dir} not found, skipped")
            continue
        title = name if len(modules) > 1 else None
        found = report(module_config, asm_dir, args, title)
        if title and found:
            print()
        total += found

    print(f"\n{total} unclaimed .text range(s)")


if __name__ == "__main__":
    main()
