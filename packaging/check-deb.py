#!/usr/bin/env python3
"""check-deb.py DEB : neuronte ships the library and not the packed weights."""

from __future__ import annotations

import os
import subprocess
import sys
import tempfile
from pathlib import Path


def fail(message: str) -> None:
    print(f"check-deb: {message}", file=sys.stderr)
    raise SystemExit(1)


def main() -> None:
    if len(sys.argv) != 2:
        fail("usage: check-deb.py DEB")
    deb = Path(sys.argv[1])
    fields = subprocess.run(
        ["dpkg-deb", "-f", str(deb), "Package", "Version", "Architecture"],
        check=True,
        capture_output=True,
        text=True,
    ).stdout
    for needle in ("Package: neuronte\n", "Version: 2.1.0+packaging1\n"):
        if needle not in fields:
            fail(f"unexpected fields:\n{fields}")
    arch = ""
    for line in fields.splitlines():
        if line.startswith("Architecture: "):
            arch = line.split(": ", 1)[1]
    if arch not in {"amd64", "arm64"}:
        fail(f"unexpected architecture:\n{fields}")
    with tempfile.TemporaryDirectory(prefix="neuronte-deb.") as raw:
        root = Path(raw)
        subprocess.run(["dpkg-deb", "-x", str(deb), str(root)], check=True)
        libs = list(root.glob("usr/lib/*/libneuronte.so.1"))
        probe = root / "usr/bin/neuronte-probe"
        header = root / "usr/include/neuronte.h"
        if len(libs) != 1 or not probe.is_file() or not header.is_file():
            fail("library, probe, or header missing")
        for path in root.rglob("*"):
            if path.suffix == ".nrt" or path.name.endswith(".nrt"):
                fail(f"packed weight packaged: {path.name}")
            if path.is_file() and not path.is_symlink() and path.stat().st_size > 1_000_000:
                fail(f"file over 1MB: {path.name} ({path.stat().st_size})")
        env = os.environ.copy()
        env["LD_LIBRARY_PATH"] = str(libs[0].parent)
        ran = subprocess.run(
            [str(probe)],
            check=False,
            capture_output=True,
            text=True,
            env=env,
        )
        if ran.returncode != 0 or ran.stdout.strip() != "neuronte loaded":
            fail(f"probe failed ({ran.returncode}): {ran.stdout!r} {ran.stderr!r}")
    print("check-deb: passed")


if __name__ == "__main__":
    main()
