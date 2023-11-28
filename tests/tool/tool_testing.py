#!/usr/bin/env python

# Copyright 2015 Wieger Wesselink.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import sys
import concurrent.futures

# Makes sure that the script can find the modules when ran directly.
sys.path.append(os.path.join(os.path.dirname(__file__), "../../"))

from tests.utility.run_process import RunProcess

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))


class LpsFile:
    None


class ToolTest:
    def __init__(self, toolname: str, input: str | LpsFile, options: list[str]):
        self.toolname = toolname
        self.input = input
        self.options = options


available_tests = [
    ToolTest(
        "mcrl22lps",
        os.path.join(MCRL2_ROOT, "examples/academic/abp/abp.mcrl2"),
        [
            "-a",
            "-b",
            "-c",
            "-e",
            "-f",
            "-g",
            "-lregular",
            "-lregular2",
            "-lstack",
            "-m",
            "-n",
            "--no-constelm",
            "-o",
            "-rjitty",
            "-rjittyp",
            "--timings",
            "-w",
            "-z",
        ],
    ),
    ToolTest(
        "lps2lts",
        LpsFile(),
        [
            "-ctau",
            "-D",
            "--error-trace",
            "-rjitty",
            "-rjittyp",
            "-rjittyc",
            "-sd",
            "-sb",
            "-sh --todo-max=100",
            "--verbose --suppress",
            "-u",
        ],
    ),
    ToolTest(
        "lpsconstelm",
        LpsFile(),
        ["-c", "-f", "-s", "-t", "-rjitty", "-rjittyp", "-rjittyc"],
    ),
    ToolTest("lpsrewr", LpsFile(), ["-rjitty", "-rjittyp", "-rjittyc"]),
    ToolTest("lpsinfo", LpsFile(), []),
    ToolTest("lpspp", LpsFile(), []),
    ToolTest("lpsparelm", LpsFile(), []),
    ToolTest("lpssumelm", LpsFile(), ["-c"]),
    ToolTest(
        "lpsactionrename",
        LpsFile(),
        ["-m", "-o", "-t", "-rjitty", "-rjittyp", "-rjittyc"],
    ),
    ToolTest("lpsuntime", LpsFile(), ["-i", "-e"]),
    # TODO: "-zcvc"
    ToolTest(
        "lpsinvelm",
        LpsFile(),
        ["-c", "-e", "-l", "-n", "-pinvelm_test", "-rjitty", "-t10", "-rjittyc"],
    ),
]


def run_test(toolpath, arguments):
    return RunProcess(toolpath, arguments, 1000, 1)


def main(tests):
    # Parse the input arguments.
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument(
        "-t",
        "--toolpath",
        dest="toolpath",
        help="The path where the mCRL2 tools are installed",
        required=True,
    )
    cmdline_parser.add_argument(
        "-o",
        "--output",
        dest="output",
        metavar="o",
        action="store",
        help="Run the tests in the given directory, where the outputs are produced",
    )
    cmdline_parser.add_argument(
        "-m", "--max_workers", action="store", default=1, type=int
    )

    args = cmdline_parser.parse_args()

    if args.output:
        output = os.path.join(os.getcwd(), args.output)
    else:
        output = "."

    mcrl2_file = os.path.join(MCRL2_ROOT, "examples/academic/abp/abp.mcrl2")

    print(f"Write intermediate files into {output}")
    name, _ = os.path.splitext(os.path.basename(mcrl2_file))
    lps_file = os.path.join(output, f"{name}.lps")
    lts_file = os.path.join(output, f"{name}.lts")

    # Create input files for lpsconfcheck and lpsinvelm
    true_file = os.path.join(output, "true.inv")
    false_file = os.path.join(output, "false.inv")

    with open(true_file, encoding="utf-8") as f:
        f.write("true")
    with open(false_file, encoding="utf-8") as f:
        f.write("false")


    # Generate the input files for all tests beforehand
    run_test(os.path.join(args.toolpath, "mcrl22lps"), ["-D", mcrl2_file, lps_file])
    run_test(os.path.join(args.toolpath, "lps2lts"), [lps_file, lts_file])

    try:
        with concurrent.futures.ThreadPoolExecutor(
            max_workers=args.max_workers
        ) as executor:
            # Start the test cases
            futures = {}
            for test in tests:
                for argument in test.options + ["-v"]:
                    if argument == "-rjittyc":
                        continue

                    if isinstance(test.input, str):
                        arg_input = test.input
                    else:
                        arg_input = lps_file

                    toolpath = os.path.join(args.toolpath, test.toolname)
                    if True:
                        print(f"Executing {test.toolname} {argument} {arg_input}")
                        futures[
                            executor.submit(
                                run_test, toolpath, argument.split(" ") + [arg_input]
                            )
                        ] = f"{test.toolname} {argument}"
                    #else:
                    #    print(f"Skipped tool {test.toolname} since it cannot be found")

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
    except KeyboardInterrupt:
        print("Killed")


if __name__ == "__main__":
    main(available_tests)
