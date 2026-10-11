"""Synthetic triage cases: no original game data or compiler needed."""
import contextlib
import io
import json
from pathlib import Path
import struct
import tempfile
import unittest

from tools import decomp_triage as triage


def code(*values):
    return struct.pack(">" + "I" * len(values), *values)


def elf_function(instructions, relocation_target=None):
    """One executable section, one function, optional PPC RELA import."""
    text = code(*instructions)
    strings = b"\0behavior\0" + (relocation_target.encode() + b"\0" if relocation_target else b"")
    symbols = b"\0" * 16 + struct.pack(">IIIBBH", 1, 0, len(text), 0x12, 0, 1)
    if relocation_target:
        symbols += struct.pack(">IIIBBH", 10, 0, 0, 0x10, 0, 0)
    payload = text + strings + symbols
    sections = [(0,) * 10,
                (0, 1, 4, 0, 52, len(text), 0, 0, 4, 0),
                (0, 3, 0, 0, 52 + len(text), len(strings), 0, 0, 1, 0),
                (0, 2, 0, 0, 52 + len(text) + len(strings), len(symbols), 2, 0, 4, 16)]
    if relocation_target:
        rel = struct.pack(">IIi", 0, (2 << 8) | 10, 0)
        sections.append((0, 4, 0, 0, 52 + len(payload), len(rel), 3, 1, 4, 12))
        payload += rel
    header = bytearray(52)
    header[:6] = b"\x7fELF\x01\x02"
    struct.pack_into(">HH", header, 16, 1, 20)
    struct.pack_into(">I", header, 32, 52 + len(payload))
    struct.pack_into(">HH", header, 46, 40, len(sections))
    return bytes(header) + payload + b"".join(struct.pack(">10I", *section) for section in sections)


class FrameTests(unittest.TestCase):
    def test_prologue_and_saved_stack_offsets(self):
        target = code(0x9421ffc0, 0x90010044, 0x38210040, 0x4e800020)
        base = code(0x9421ffd0, 0x90010034, 0x38210030, 0x4e800020)
        self.assertEqual(triage.frame_size(target), 64)
        self.assertTrue(triage.frame_only(target, base))

    def test_literal_delta_is_not_stack_evidence(self):
        target = code(0x9421ffc0, 0x38600020, 0x4e800020)
        base = code(0x9421ffd0, 0x38600010, 0x4e800020)
        self.assertFalse(triage.frame_only(target, base))

    def test_object_field_delta_is_not_stack_evidence(self):
        target = code(0x9421ffc0, 0x80640020, 0x4e800020)
        base = code(0x9421ffd0, 0x80640010, 0x4e800020)
        self.assertFalse(triage.frame_only(target, base))

    def test_unrecognized_opcode_never_excused(self):
        self.assertFalse(triage.frame_only(code(0x9421ffc0, 0x48000020), code(0x9421ffd0, 0x48000010)))

    def test_wrong_sign_is_not_frame_only(self):
        self.assertFalse(triage.frame_only(code(0x9421ffc0, 0x90010024), code(0x9421ffd0, 0x90010034)))

    def test_leaf_and_branch_before_stack_instruction(self):
        self.assertEqual(triage.frame_size(code(0x4e800020)), 0)
        self.assertEqual(triage.frame_size(code(0x48000020, 0x9421ffc0)), 0)

    def test_length_difference_not_frame_only(self):
        self.assertFalse(triage.frame_only(code(0x9421ffc0), code(0x9421ffd0, 0x4e800020)))


class ProjectTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name) / "project"
        self.root.mkdir()
        (self.root / "configure.py").write_text('''
raise RuntimeError("must not execute")
objects = [Object(Matching, "mo_fighter/ft_a/exact.cpp"),
           Object(NonMatching, "mo_fighter/ft_a/draft.cpp"),
           Object(NonMatching, "mo_stage/st_a/ground.cpp")]
''', encoding="utf-8")

    def write_object(self, name, instructions, relocation_target=None):
        path = self.root / name
        path.write_bytes(elf_function(instructions, relocation_target))
        return path

    def test_queue_never_executes_configure_or_lists_matching(self):
        rows = triage.work_queue(self.root, None, [])
        self.assertEqual(len(rows), 2)
        self.assertTrue(rows[0]["source"].startswith("src/mo_fighter/"))
        self.assertEqual(rows[0]["comparison"], "unavailable")
        self.assertEqual(rows[0]["exact_functions"], 0)

    def test_active_claims_and_module_exclusion_ranking(self):
        claims = triage.active_claims("sol | ft_a | host:tree | in progress\nold | st_a | tree | done PR1\n")
        rows = triage.work_queue(self.root, None, claims)
        self.assertEqual(len(claims), 1)
        self.assertTrue(rows[-1]["claims"])
        self.assertEqual(rows[0]["claims"], [])
        self.assertEqual(triage.matching_claims("mo_fighter/ft_ab/a.cpp", claims), [])

    def test_target_only_report_does_not_become_success(self):
        report = {"units": [{"metadata": {"source_path": "src/mo_fighter/ft_a/draft.cpp"},
                             "functions": [{"name": "behavior", "size": "4"}]}]}
        row = triage.work_queue(self.root, report, [])[0]
        self.assertEqual(row["reported_functions"], 1)
        self.assertEqual(row["compared_functions"], 0)
        self.assertEqual(row["comparison"], "unavailable")

    def test_elf_function_and_relocation(self):
        function = triage.read_functions(self.write_object("a.o", [0x48000001], "callee"))["behavior"]
        self.assertEqual(function["code"], code(0x48000001))
        self.assertEqual(function["relocations"], [(0, 10, "callee", 0)])

    def test_truncated_elf_fails_clearly(self):
        path = self.write_object("a.o", [0x4e800020])
        path.write_bytes(path.read_bytes()[:-1])
        with self.assertRaises(triage.TriageError):
            triage.read_functions(path)

    def test_wrong_machine_rejected(self):
        path = self.write_object("a.o", [0x4e800020])
        value = bytearray(path.read_bytes())
        struct.pack_into(">H", value, 18, 40)
        path.write_bytes(value)
        with self.assertRaises(triage.TriageError):
            triage.read_functions(path)

    def test_equal_bytes_different_callees_are_not_equal(self):
        self.write_object("target.o", [0x48000001], "first")
        self.write_object("base.o", [0x48000001], "second")
        (self.root / "objdiff.json").write_text(json.dumps({"units": [
            {"name": "ft_a/draft", "target_path": "target.o", "base_path": "base.o"},
            {"name": "ft_a/absent", "target_path": "target.o", "base_path": "absent.o"}]}))
        result = triage.frame_report(self.root, [])
        self.assertEqual(result["functions"][0]["bucket"], "relocations-differ")
        self.assertEqual(len(result["unavailable"]), 1)

    def test_output_inside_checkout_rejected(self):
        with contextlib.redirect_stderr(io.StringIO()):
            result = triage.main(["--project", str(self.root), "queue", "--output", str(self.root / "dump.json")])
        self.assertEqual(result, 1)
        self.assertFalse((self.root / "dump.json").exists())

    def test_existing_output_not_overwritten(self):
        output = Path(self.temp.name) / "queue.json"
        output.write_text("preserved")
        with contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(triage.main(["--project", str(self.root), "queue", "--output", str(output)]), 1)
        self.assertEqual(output.read_text(), "preserved")

    def test_compiler_notes_do_not_upgrade_confidence(self):
        (self.root / "TIPS.md").write_text("## Stack\n- Frame observation. **HIGH**\n", encoding="utf-8")
        note = triage.compiler_notes(self.root, "frame")[0]
        self.assertEqual(note["confidence"], "project-note-high")
        self.assertEqual(note["compiler"], "MWCC GC/3.0a5.2")


if __name__ == "__main__":
    unittest.main()
