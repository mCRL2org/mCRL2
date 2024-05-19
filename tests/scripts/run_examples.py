#!/usr/bin/env python3

import argparse
import os
import subprocess
import sys
import concurrent.futures


def main():
    """Finds all the .mcrl2 files in the example directory and tries to parse them with mcrl22lps."""

    # Parse some configuration options
    parser = argparse.ArgumentParser(
        prog="run_examples.py",
        description="Prepares the examples specifications for testing",
        epilog="",
    )

    parser.add_argument("-m", "--max_workers", action="store", default=1, type=int)
    parser.add_argument("-r", "--jittyc", action="store_true")
    parser.add_argument(
        "-t", "--toolpath", action="store", type=str, required=True
    )

    args = parser.parse_args()

    os.environ["PATH"] += os.pathsep + args.toolpath

    def run_example(path, index):
        print(f"[{index}] Running {path}")

        try:
            result = subprocess.run([sys.executable, path] + ["-rjittyc"] if args.jittyc else [], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=60, cwd=os.path.dirname(path), check=True)
        except subprocess.TimeoutExpired:
            # Ignore timeouts, they are not errors.
            return ""

        return result.stdout

    # Start the linearisation process
    with concurrent.futures.ThreadPoolExecutor(
        max_workers=args.max_workers
    ) as executor:
        # Collect all .mcrl2 files
        run_scripts = []
        for root, _, files in os.walk(
            os.path.join(os.path.realpath(os.path.dirname(__file__)), "../../examples/")
        ):
            for file in files:
                if file == "run.py":
                    run_scripts.append(os.path.join(root, file))

        # Submit the jobs
        futures = {}
        for index, path in enumerate(run_scripts):
            futures[(executor.submit(run_example, path, index))] = path

        # Wait for jobs to finish and show a progress bar.
        finished = 0
        failed = []
        for future in concurrent.futures.as_completed(futures):
            path = futures[future]

            if future.exception():
                print(
                    f"[{finished}/{len(run_scripts)}] Example {path} failed with {future.exception()}"
                )
                failed.append(path)
            if future.cancelled():
                print(f"[{finished}/{len(run_scripts)}] Example {path} was cancelled")
            else:
                print(f"[{finished}/{len(run_scripts)}] Finished example {path}")

            finished += 1

        print("Failed tests...")
        for name in failed:
            print(f"{name} failed")
            sys.exit(-1)

if __name__ == "__main__":
    main()
