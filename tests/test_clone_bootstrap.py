"""Exercise the actual clone guard, exclusive marker, and cleanup boundaries.

The SSH transport is replaced by a local forced-command handler in a temporary
directory. No server configuration, authorization keys, or game files are used.
"""
import ast
import base64
import importlib.util
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time
import unittest

MODULE = Path(__file__).resolve().parents[1] / "tools/remote/bootstrap_clone.py"
spec = importlib.util.spec_from_file_location("bootstrap_clone", MODULE)
bootstrap = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bootstrap)


@unittest.skipUnless(os.name == "posix", "guard uses POSIX flock and process groups")
class CloneGuardTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="clone-bootstrap-test-")
        self.root = Path(self.temp.name)
        for folder in ("notes", "bin", "tools/remote"):
            (self.root / folder).mkdir(parents=True)
        coordinator = bootstrap.COORDINATOR.replace(
            "Path('/srv/brawl-native')", f"Path({str(self.root)!r})")
        (self.root / "coordinator.py").write_text(coordinator)
        board = self.root / "tools/remote/board.sh"
        board.write_text('#!/bin/sh\nprintf "board-ok\\n"\n')
        board.chmod(0o755)
        client = bootstrap.CLIENT.replace(
            "/srv/brawl-native/notes/clone-coordination.json",
            str(self.root / "notes/clone-coordination.json"))
        (self.root / "client.py").write_text(client)
        (self.root / "notes/clone-coordination.json").write_text(json.dumps({
            "coordinator": "fixture", "user": "fixture",
            "known_hosts": str(self.root / "known"), "key": str(self.root / "key")}))
        ssh = self.root / "bin/ssh"
        ssh.write_text(
            "#!/usr/bin/env python3\nimport os,sys\n"
            'os.environ["SSH_ORIGINAL_COMMAND"]=sys.argv[-1]\n'
            f"os.execv(sys.executable,[sys.executable,{str(self.root / 'coordinator.py')!r}])\n")
        ssh.chmod(0o755)
        self.env = dict(os.environ)
        self.env["PATH"] = str(self.root / "bin") + os.pathsep + self.env["PATH"]
        self.env.pop("BRAWL_GLOBAL_BUILD_LOCK", None)
        self.children = []

    def tearDown(self):
        for child in self.children:
            if child.poll() is None:
                child.terminate()
                child.communicate(timeout=15)
        self.temp.cleanup()

    def guard(self, code):
        child = subprocess.Popen(
            [sys.executable, str(self.root / "client.py"), "guard",
             sys.executable, "-c", code], env=self.env,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        self.children.append(child)
        return child

    def wait_for(self, path):
        for _ in range(100):
            if path.exists():
                return
            time.sleep(0.02)
        self.fail(f"fixture did not create {path.name}")

    def test_serializes_builds_and_removes_runner_marker(self):
        log = self.root / "order"
        def command(tag, delay):
            return (f"from pathlib import Path;import time;p=Path({str(log)!r});"
                    f"p.open('a').write('{tag}-start\\n');time.sleep({delay});"
                    f"p.open('a').write('{tag}-end\\n')")
        first = self.guard(command("a", 0.3))
        marker = self.root / "notes/decomp-runner.lock"
        self.wait_for(log)
        with self.assertRaises(FileExistsError):
            fd = os.open(marker, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o600)
            os.close(fd)
        second = self.guard(command("b", 0.01))
        for child in (first, second):
            _, error = child.communicate(timeout=10)
            self.assertEqual(child.returncode, 0, error)
        self.assertEqual(log.read_text().splitlines(),
                         ["a-start", "a-end", "b-start", "b-end"])
        self.assertFalse(marker.exists())

    def test_cancellation_stops_child_and_releases_locks(self):
        pidfile = self.root / "child-pid"
        child = self.guard(
            f"from pathlib import Path;import os,time;"
            f"Path({str(pidfile)!r}).write_text(str(os.getpid()));time.sleep(60)")
        self.wait_for(pidfile)
        pid = int(pidfile.read_text())
        child.terminate()
        child.communicate(timeout=15)
        with self.assertRaises(ProcessLookupError):
            os.kill(pid, 0)
        self.assertFalse((self.root / "notes/decomp-runner.lock").exists())
        later = self.guard("print('lock-released')")
        output, error = later.communicate(timeout=10)
        self.assertEqual(later.returncode, 0, error)
        self.assertIn("lock-released", output)

    def test_forced_command_rejects_shell_execution(self):
        token = base64.urlsafe_b64encode(json.dumps(
            {"action": "shell", "args": ["id"]}).encode()).decode()
        env = dict(self.env)
        env["SSH_ORIGINAL_COMMAND"] = "brawl-clone-v1 " + token
        result = subprocess.run([sys.executable, str(self.root / "coordinator.py")],
                                env=env, capture_output=True, text=True)
        self.assertEqual(result.returncode, 2)
        self.assertIn("rejected", result.stderr)

    def test_cache_cleanup_rejects_outside_paths_and_symlinks(self):
        tree = ast.parse(bootstrap.SETUP)
        function = next(node for node in tree.body
                        if isinstance(node, ast.FunctionDef) and node.name == "safe")
        namespace = {"Path": Path, "root": self.root}
        exec(compile(ast.Module(body=[function], type_ignores=[]), "safe", "exec"), namespace)
        safe = namespace["safe"]
        self.assertEqual(safe(self.root / "build"), self.root / "build")
        for path in (self.root, self.root.parent / "outside"):
            with self.assertRaises(RuntimeError):
                safe(path)
        link = self.root / "symlink"
        link.symlink_to(self.root.parent / "outside", target_is_directory=True)
        with self.assertRaises(RuntimeError):
            safe(link)


if __name__ == "__main__":
    unittest.main()
