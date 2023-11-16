#!/usr/bin/env python

# Copyright 2015 Wieger Wesselink.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import sys
import concurrent.futures

# Makes sure that the script can find the modules when ran directly.
sys.path.append(os.path.join(os.path.dirname(__file__),'../../'))

from tests.utility.run_process import RunProcess

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))

class ToolTest:
    def __init__(self, toolname: str, options: list[str]):
        self.toolname = toolname
        self.options = options

available_tests = [
    ToolTest("mcrl22lps", ["-a", "-b", "-c", "-e", "-f", "-g", "-lregular", "-lregular2", "-lstack", "-m", "-n", "--no-constelm", "-o", "-rjitty", "-rjittyp", "--timings", "-w", "-z"])
]


def run_test(toolpath, arguments):
    return RunProcess(toolpath, arguments, 100, 3)

def main(tests):

    # Parse the input arguments.
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument('-t', '--toolpath', dest='toolpath', help='The path where the mCRL2 tools are installed', required=True)
    cmdline_parser.add_argument('-o', '--output', dest='output', metavar='o', action='store', help='Run the tests in the given directory, where the outputs are produced')
    cmdline_parser.add_argument("-m", "--max_workers", action="store", default=1, type=int)

    args = cmdline_parser.parse_args()

    if args.output:
        output = args.output
    else:
        output = "."

    mcrl2_file = os.path.join(MCRL2_ROOT, "examples/academic/abp/abp.mcrl2")

    name, _ = os.path.splitext(mcrl2_file)
    lps_file = os.path.join(output, f"{name}.lps")
    lts_file = os.path.join(output, f"{name}.lts")

    # Generate the input files for all tests beforehand
    run_test(os.path.join(args.toolpath, "mcrl22lps"), ["-D", mcrl2_file, lps_file])
    run_test(os.path.join(args.toolpath, "lps2lts"), [lps_file, lts_file])

    return

    with concurrent.futures.ThreadPoolExecutor(
        max_workers=args.max_workers
    ) as executor:
        
        # Start the test cases
        futures = {}
        for test in tests:
            for argument in test.options:
                print(f"Executing {test.toolname} {argument} {mcrl2_file}")
                futures[executor.submit(
                    run_test, os.path.join(args.toolpath, test.toolname), argument, mcrl2_file
                )] = (f"{test.toolname} {argument}")

        # Wait for the results and keep track of failed tests.
        failed = []
        for future in concurrent.futures.as_completed(futures):
            name = futures[future]

            try:
                future.result()
                print(f"{name} {(100 - len(name))*'.'} ok")
            except Exception as e:
                print(f"{name} {(100 - len(name))*'.'} failed \n  {e}")
                failed.append(name)

        print("Failed tests...")
        for name in failed:
            print(f"{name} failed")

    

if __name__ == '__main__':
    main(available_tests)