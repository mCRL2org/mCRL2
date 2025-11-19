#!/usr/bin/env python3

# Copyright 2024-2025 Maurice Laveaux
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
from enum import Enum
import os
from pathlib import Path
import sys
import shutil
import concurrent.futures

# Makes sure that the script can find the modules when ran directly.
sys.path.append(os.path.join(os.path.dirname(__file__), "../../"))
from tests.utility.run_process import RunProcess, TimeExceededError

class Variant(Enum):
    Script = 1
    MCRL2Project = 2
    MCRL2 = 3

def main():
    """Execute all the run.py scripts in the example directory."""

    # Parse some configuration options
    parser = argparse.ArgumentParser(
        prog="run_examples.py",
        description="Prepares the examples specifications for testing",
        epilog="",
    )

    parser.add_argument("-m", "--max_workers", action="store", default=1, type=int)
    parser.add_argument("-i", "--timeout", action="store", default=60, type=int)
    parser.add_argument("-r", "--jittyc", action="store_true")
    parser.add_argument("-t", "--toolpath", action="store", type=str, required=True)

    args = parser.parse_args()

    os.environ["PATH"] = args.toolpath + os.pathsep + os.environ["PATH"]

    mcrl22lps_path = shutil.which("mcrl22lps")
    lps2pbes_path = shutil.which("lps2pbes")
    
    def run_example(path, index):
        print(f"[{index}] Running {path}", flush=True)

        arguments = [path]
        if args.jittyc:
            arguments += ["-rjittyc"]

        # Change the working directory to the script path
        os.chdir(os.path.dirname(path))

        # Remove the .exe suffix on Windows systems since RunProcess adds it automatically.
        python = sys.executable
        if python.endswith("exe"):
            python = python[:-4]

        process = RunProcess(python, arguments, 2000, args.timeout)
        return process.stdout

    def run_mcrl2_file(mcrl22lps_path, path, index):
        print(f"[{index}] Checking mcrl2 {path}", flush=True)

        process = RunProcess(
            mcrl22lps_path,
            ["--check-only", path],
            2000,
            args.timeout,
        )
        return process.stdout

    def run_mcrl2proj_file(mcrl22lps_path, lps2pbes_path, path, index):
        print(f"[{index}] Checking mcrl2proj {path}", flush=True)

        mcrl2_path = os.path.splitext(path)[0] + "_spec.mcrl2"
        lps_path = str(Path(path).with_suffix(".lps"))

        process = RunProcess(
            mcrl22lps_path,
            [mcrl2_path, lps_path],
            2000,
            args.timeout,
        )

        output = process.stdout

        # Find properties folder and list property files
        properties_dir = Path(path).parent / "properties"
        if properties_dir.exists():
            for prop in os.listdir(properties_dir):
                process = RunProcess(
                    lps2pbes_path,
                    ["--check-only", f"--formula={os.path.join(properties_dir, prop)}", lps_path],
                    2000,
                    args.timeout,
                )
                output += process.stdout

        return process.stdout

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
                    run_scripts.append((Variant.Script, os.path.join(root, file)))
                elif file.endswith(".mcrl2"):
                    run_scripts.append((Variant.MCRL2, os.path.join(root, file)))
                elif file.endswith(".mcrl2proj"):
                    run_scripts.append((Variant.MCRL2Project, os.path.join(root, file)))

        # Submit the jobs
        futures = {}
        for index, (variant, path) in enumerate(run_scripts):
            if variant == Variant.Script:
                futures[(executor.submit(run_example, path, index))] = path
            elif variant == Variant.MCRL2:
                futures[(executor.submit(run_mcrl2_file, mcrl22lps_path, path, index))] = path
            elif variant == Variant.MCRL2Project:
                futures[(executor.submit(run_mcrl2proj_file, mcrl22lps_path, lps2pbes_path, path, index))] = path

        # Wait for jobs to finish and show a progress bar.
        finished = 0
        failed = []
        for future in concurrent.futures.as_completed(futures):
            path = futures[future]

            if future.exception():
                if isinstance(future.exception(), TimeExceededError):
                    # Ignore timeouts, they are not errors.
                    print(
                        f"[{finished}/{len(run_scripts)}] Example {path} timed out",
                        flush=True,
                    )
                else:
                    print(
                        f"[{finished}/{len(run_scripts)}] Example {path} failed with {future.exception()}",
                        flush=True,
                    )
                    failed.append(path)
            elif future.cancelled():
                print(
                    f"[{finished}/{len(run_scripts)}] Example {path} was cancelled",
                    flush=True,
                )
            else:
                print(
                    f"[{finished}/{len(run_scripts)}] Finished example {path}",
                    flush=True,
                )

            finished += 1

        print("Failed tests...")
        for name in failed:
            print(f"{name} failed")

        if failed:
            sys.exit(-1)


if __name__ == "__main__":
    main()
