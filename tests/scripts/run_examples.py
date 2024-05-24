#!/usr/bin/env python3

import argparse
import os
import subprocess
import sys
import concurrent.futures

# Makes sure that the script can find the modules when ran directly.
sys.path.append(os.path.join(os.path.dirname(__file__),'../../'))
from tests.utility.run_process import RunProcess, TimeExceededError

def main():
    """Finds all the .mcrl2 files in the example directory and tries to parse them with mcrl22lps."""

    # Parse some configuration options
    parser = argparse.ArgumentParser(
        prog="run_examples.py",
        description="Prepares the examples specifications for testing",
        epilog="",
    )

    parser.add_argument("-m", "--max_workers", action="store", default=1, type=int)
    parser.add_argument("-i", "--timeout", action="store", default=60, type=int)
    parser.add_argument("-r", "--jittyc", action="store_true")
    parser.add_argument(
        "-t", "--toolpath", action="store", type=str, required=True
    )

    args = parser.parse_args()

    os.environ["PATH"] += os.pathsep + args.toolpath

    def run_example(path, index):
        print(f"[{index}] Running {path}", flush=True)

        arguments = [path]
        if args.jittyc:
            arguments += ["-rjittyc"]
        
        # Change the working directory to the script path
        os.chdir(os.path.dirname(path))

        # Strip the extension since the Runprocess adds it again
        python,_ = os.path.splitext(sys.executable)

        process = RunProcess(python, arguments, 2000, args.timeout)
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
                if isinstance(future.exception(), TimeExceededError):
                    # Ignore timeouts, they are not errors.
                    print(
                        f"[{finished}/{len(run_scripts)}] Example {path} timed out", 
                        flush=True
                    )
                else:
                    print(
                        f"[{finished}/{len(run_scripts)}] Example {path} failed with {future.exception()}", 
                        flush=True
                    )
                    failed.append(path)
            if future.cancelled():
                print(f"[{finished}/{len(run_scripts)}] Example {path} was cancelled", flush=True)
            else:
                print(f"[{finished}/{len(run_scripts)}] Finished example {path}", flush=True)

            finished += 1

        print("Failed tests...")
        for name in failed:
            print(f"{name} failed")

        if failed:
            sys.exit(-1)

if __name__ == "__main__":
    main()
