#!/usr/bin/env python3
"""Scan for ``.lps`` files and run ``lps2lts`` on each.

For every discovered LPS file the converter is executed, the wall‑clock
seconds taken is measured and the resulting LTS is inspected with
``ltsinfo`` to determine the number of reachable states.  A plain text
report is printed at the end.

Usage:

    cd examples
    ./lps_report.py          # serial execution
    ./lps_report.py -j 8     # up to eight concurrent conversions

The script removes temporary ``.lts`` files it creates.  The exit code is
zero when all conversions succeeded; if any failed the script returns a
non‑zero status.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import glob
import os
import subprocess
import sys
import time
from pathlib import Path
from typing import NamedTuple, Sequence


class LpsResult(NamedTuple):
    path: Path
    duration: float
    states: int | None
    returncode: int
    exc: Exception | None


def find_lps(root: Path) -> Sequence[Path]:
    pattern = str(root / "**" / "*.lps")
    files = [Path(p) for p in glob.glob(pattern, recursive=True)]
    files.sort()
    return files


def count_states(ltsfile: Path) -> int | None:
    """Return number of states reported by ``ltsinfo`` or ``None`` if
    parsing failed (but the tool succeeded).
    """
    try:
        completed = subprocess.run(
            ["ltsinfo", str(ltsfile)],
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            check=True,
        )
    except subprocess.CalledProcessError:
        return None
    for line in completed.stdout.splitlines():
        # typical output contains a line like "states: 1234"
        if "states" in line.lower():
            parts = line.split()
            for token in parts:
                if token.isdigit():
                    return int(token)
    return None


def run_conversion(lpsfile: Path) -> LpsResult:
    cwd = lpsfile.parent
    out = lpsfile.with_suffix(".lts")
    start = time.monotonic()
    try:
        # run converter; capture stderr to avoid polluting stdout
        conv = subprocess.run(
            ["/home/wieger/repos/mCRL2/local/bin/lps2lts", "-v", str(lpsfile), str(out)],
            cwd=cwd,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )
        rc = conv.returncode
        states = None
        if rc == 0:
            states = count_states(out)
        exc = None
    except Exception as e:  # pragma: no cover
        rc = 1
        exc = e
        states = None
    end = time.monotonic()
    # cleanup temporarily created file
    try:
        if out.exists():
            out.unlink()
    except Exception:  # ignore cleanup failures
        pass
    return LpsResult(path=lpsfile, duration=end - start, states=states, returncode=rc, exc=exc)


def report(results: Sequence[LpsResult]) -> None:
    print()
    print("LPS conversion report")
    print("====================")
    fmt = "{:<60} {:>9} {:>10}   {:>7}"
    print(fmt.format("lps file", "seconds", "states", "result"))
    print(fmt.format("-" * 60, "-" * 7, "-" * 10, "-" * 6))
    failed = 0
    for r in results:
        status = "OK" if r.returncode == 0 else f"FAIL({r.returncode})"
        if r.exc:
            status = "EXCEPTION"
        if r.returncode != 0 or r.exc:
            failed += 1
        states_str = str(r.states) if r.states is not None else "?"
        print(fmt.format(str(r.path), f"{r.duration:.2f}", states_str, status))
    print()
    if failed:
        print(f"{failed} conversion(s) failed")
    else:
        print("all conversions succeeded")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Convert every ``.lps`` under the examples tree to LTS."
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=1,
        help="number of parallel conversions (default: 1)",
    )
    args = parser.parse_args()

    me = Path(__file__).resolve().parent
    files = find_lps(me)
    if not files:
        print("no .lps files found", file=sys.stderr)
        return 1

    results: list[LpsResult] = []
    if args.jobs <= 1:
        for f in files:
            print(f"converting {f}...", flush=True)
            res = run_conversion(f)
            results.append(res)
    else:
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
            futures = {ex.submit(run_conversion, f): f for f in files}
            for fut in concurrent.futures.as_completed(futures):
                res = fut.result()
                results.append(res)
                print(f"finished {res.path} -> rc {res.returncode}")

    results.sort(key=lambda r: str(r.path))
    report(results)
    return 0 if all(r.returncode == 0 and r.exc is None for r in results) else 1


if __name__ == "__main__":
    sys.exit(main())
