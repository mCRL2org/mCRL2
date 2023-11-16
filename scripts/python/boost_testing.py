#!/usr/bin/env python

"""
A script that can take a number of executables as input and runs all Boost.Test
tests in parallel up to given amount. Can be (much) faster for multiple tests
defined in a single executable.
"""

import os
import argparse
import concurrent.futures
import time
import subprocess
import re

target_regex = re.compile(r"(.*)\*$")


def discover_tests(executable):
    """
    Runs the executable and uses --list_content to figure out the contained tests, if it fails
    to discover tests it is ignored.
    """
    try:
        result = subprocess.run(
            [executable, "--list_content"],
            check=True,
            capture_output=True,
            text=True,
        )
        targets = []
        for line in result.stderr.split("\n"):
            match = target_regex.match(line)
            if match:
                targets.append(match.group(1))

        return targets
    except:
        print(f"skipped {os.path.basename(executable)}")
        return None


def run_test(executable, target):
    """
    Runs the specific target test defined in the given executable.
    """
    start = time.time()
    result = subprocess.run(
        [executable, "--run_test=", target], check=False, capture_output=True, text=True
    )
    return (result.returncode == 0, time.time() - start)


def main():
    """Entrypoint"""
    # Parse some configuration options
    parser = argparse.ArgumentParser(
        prog="run.py",
        description="Prepares the variability parity games and runs the solving experiments",
        epilog="",
    )

    parser.add_argument("-m", "--max_workers", action="store", default=1, type=int)
    parser.add_argument("test_executables", nargs="+")

    args = parser.parse_args()

    with concurrent.futures.ThreadPoolExecutor(
        max_workers=args.max_workers
    ) as executor:
        futures = {}

        print(f"Discovering {len(args.test_executables)} test executable(s)...")
        mapping = {}
        for executable in args.test_executables:
            futures[executor.submit(discover_tests, executable)] = executable

        mapping = {}
        for future in concurrent.futures.as_completed(futures):
            targets = future.result()

            if targets:
                executable = futures[future]
                mapping[executable] = targets

        futures = {}
        total = sum(map(len, mapping.values()))
        print(f"Executing {total} test(s)...")
        for executable, targets in mapping.items():
            for target in targets:
                print(f"Running {os.path.basename(executable)}:{target}...")
                futures[executor.submit(run_test, executable, target)] = (
                    executable,
                    target,
                )

        # Keep track failed test to show at the end.
        failed = []
        finished = 0
        for future in concurrent.futures.as_completed(futures):
            success, result = future.result()
            executable, target = futures[future]

            finished += 1
            txt = f"{finished}/{total} {os.path.basename(executable)}:{target}"
            print(
                f"{txt} {(100 - len(txt))*'.'} {'ok' if success else 'failed'} ({result:.2f}s)"
            )

            if not success:
                failed.append((executable, target))

        print("Failed tests...")
        for executable, target in failed:
            print(f"{os.path.basename(executable)}:{target} failed")


if __name__ == "__main__":
    main()
