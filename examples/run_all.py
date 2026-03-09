#!/usr/bin/env python3
"""Utility to execute every ``run.py`` in the examples tree.

The script discovers all ``run.py`` files under the directory where it
itself lives, executes them one by one (or in parallel if ``-j`` is
specified) and measures the wall‑clock time taken by each invocation.
A short report is printed to stdout when all examples have finished.

Usage::

    cd examples
    ./run_all.py            # serial, single job
    ./run_all.py -j 4       # run up to four examples concurrently

The exit code of ``run_all.py`` is zero when every example returned
successfully; if any example failed the exit code is non‑zero.
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


class Result(NamedTuple):
    path: Path
    duration: float
    returncode: int
    exc: Exception | None


def find_examples(root: Path) -> Sequence[Path]:
    """Return a sorted list of run.py files under *root* (recursively).
    """
    pattern = str(root / "**" / "run.py")
    files = [Path(p) for p in glob.glob(pattern, recursive=True)]
    files.sort()
    return files


def run_one(script: Path) -> Result:
    """Invoke a single ``run.py`` and time it.

    The script is executed via the Python interpreter because some
    examples may not be marked executable.  The working directory is set
    to the directory containing the script, which mirrors the behaviour
    of most of the example wrappers themselves.
    """
    cwd = script.parent
    start = time.monotonic()
    try:
        completed = subprocess.run(
            [sys.executable, str(script)], cwd=cwd, check=False
        )
        rc = completed.returncode
        exc = None
    except Exception as e:  # pragma: no cover - run-time safety
        rc = 1
        exc = e
    end = time.monotonic()
    return Result(path=script, duration=end - start, returncode=rc, exc=exc)


def report(results: Sequence[Result]) -> None:
    """Print a table summarising the outcome of each invocation."""
    print()
    print("Example run report")
    print("==================")
    fmt = "{:<60} {:>9}   {:>7}"
    print(fmt.format("script", "seconds", "result"))
    print(fmt.format("-" * 60, "-" * 7, "-" * 6))
    failed = 0
    for r in results:
        status = "OK" if r.returncode == 0 else f"FAIL({r.returncode})"
        if r.exc:
            status = "EXCEPTION"
        if r.returncode != 0 or r.exc:
            failed += 1
        print(fmt.format(str(r.path), f"{r.duration:.2f}", status))
    print()
    if failed:
        print(f"{failed} example(s) failed")
    else:
        print("all examples succeeded")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Run every ``examples/**/run.py`` and measure timings"
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=1,
        help="number of parallel runs (default: 1)",
    )
    args = parser.parse_args()

    me = Path(__file__).resolve().parent
    examples = find_examples(me)
    if not examples:
        print("no examples found", file=sys.stderr)
        return 1

    results: list[Result] = []

    if args.jobs <= 1:
        for script in examples:
            print(f"Running {script}...", flush=True)
            r = run_one(script)
            results.append(r)
    else:
        # use ThreadPoolExecutor because the work is entirely subprocess
        # invocations; threads are sufficient and cheaper than processes.
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
            futures = {ex.submit(run_one, s): s for s in examples}
            for fut in concurrent.futures.as_completed(futures):
                r = fut.result()
                results.append(r)
                print(f"Finished {r.path} -> returncode {r.returncode}")

    # sort results by path for consistent reporting
    results.sort(key=lambda r: str(r.path))
    report(results)

    # exit non-zero if any run failed
    return 0 if all(r.returncode == 0 and r.exc is None for r in results) else 1


if __name__ == "__main__":
    sys.exit(main())
