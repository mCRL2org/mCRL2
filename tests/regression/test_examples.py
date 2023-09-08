#!/usr/bin/env python3

import argparse
import os
import subprocess
import concurrent.futures

def main():
    """Finds all the .mcrl2 files in the example directory and tries to parse them with mcrl22lps."""

    # Parse some configuration options
    parser = argparse.ArgumentParser(
        prog="test_examples.py",
        description="Prepares the examples specifications for testing",
        epilog="",
    )

    parser.add_argument("-m", "--max_workers", action="store", default=1, type=int)
    parser.add_argument(
        "-t", "--mcrl2-binpath", action="store", type=str, required=True
    )

    args = parser.parse_args()

    os.environ["PATH"] += os.pathsep + args.mcrl2_binpath

    # Ensure that tmp directory exists since the mCRL2 tools cannot make it
    tmp_directory = 'tmp/'
    try:
        os.mkdir(tmp_directory)
    except OSError:
        print(f"{tmp_directory} already exists")

    def linearise_process(path, index):
        print(f"[{index}] Linearising {path}")
        lps_file = os.path.join(tmp_directory, f"temp{index}.lps")
        lts_file = os.path.join(tmp_directory, f"temp{index}.lts")

        subprocess.check_call(["mcrl22lps", path, lps_file], timeout=60)

        subprocess.check_call(["lps2lts", lps_file, lts_file], timeout=60)
        return path
    
    # Start the linearisation process
    with concurrent.futures.ThreadPoolExecutor(
        max_workers=args.max_workers
    ) as executor:
        # Collect all .mcrl2 files
        mcrl2_files = []
        for root, _, files in os.walk(
            os.path.join(os.path.realpath(os.path.dirname(__file__)), "../../examples/")
        ):
            for file in files:
                if file.endswith(".mcrl2"):
                    mcrl2_files.append(os.path.join(root, file))

        # Submit the jobs
        futures = []
        for index, path in enumerate(mcrl2_files):
            futures.append(executor.submit(linearise_process, path, index))

        # Wait for jobs to finish and show a progress bar.
        finished = 0
        for future in concurrent.futures.as_completed(futures):
            if future.exception():
                None
            if future.cancelled():
                None
            else:
                path = future.result()
                print(f"[{finished}/{len(mcrl2_files)}] Finished linearising {path}")
                finished += 1


if __name__ == "__main__":
    main()
