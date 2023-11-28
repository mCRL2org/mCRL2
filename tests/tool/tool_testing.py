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
    pass


class LtsFile:
    pass


class ToolTest:
    def __init__(self, toolname: str, file_input: str, options: list[str]):
        self.toolname = toolname
        self.file_input = file_input
        self.options = options


available_tests = [
    ToolTest(
        "mcrl22lps",
        "[mcrl2file]",
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
        "[lpsfile]",
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
        "[lpsfile]",
        ["-c", "-f", "-s", "-t", "-rjitty", "-rjittyp", "-rjittyc"],
    ),
    ToolTest("lpsrewr", "[lpsfile]", ["-rjitty", "-rjittyp", "-rjittyc"]),
    ToolTest("lpsinfo", "[lpsfile]", []),
    ToolTest("lpspp", "[lpsfile]", []),
    ToolTest("lpsparelm", "[lpsfile]", []),
    ToolTest("lpssumelm", "[lpsfile]", ["-c"]),
    ToolTest(
        "lpsactionrename",
        "[lpsfile]",
        list(
            map(
                lambda x: "-f[rename] " + x,
                ["-m", "-o", "-t", "-rjitty", "-rjittyp", "-rjittyc"],
            )
        ),
    ),
    ToolTest("lpsuntime", "[lpsfile]", ["-i", "-e"]),
    # TODO: detect "-zcvc"
    ToolTest(
        "lpsinvelm",
        "[lpsfile]",
        list(
            map(
                lambda x: "-i[trueinv] " + x,
                [
                    "-c",
                    "-e",
                    "-l",
                    "-n",
                    "-pinvelm_test",
                    "-rjitty",
                    "-t10",
                    "-rjittyc",
                ],
            )
        ),
    ),
    ToolTest(
        "lpsconfcheck",
        "[lpsfile]",
        ["-rjitty", "-rjittyc"],
    ),
    ToolTest(
        "lpsbinary",
        "[lpsfile]",
        ["-rjitty", "-rjittyp", "-rjittyc"],
    ),
    ToolTest(
        "lpsparunfold",
        "[lpsfile]",
        list(
            map(
                lambda x: "-sNat " + x,
                ["-a", "-p", "-n10", "-rjitty", "-rjittyp", "-rjittyc"],
            )
        )
        + ["-i0"],
    ),
    ToolTest(
        "lpssuminst",
        "[lpsfile]",
        list(map(lambda x: "--finite " + x, ["-rjitty", "-rjittyp", "-rjittyc"])),
    ),
    ToolTest(
        "lpsconfcheck",
        "[lpsfile]",
        [
            "-a",
            "-c",
            "-g",
            "-i[trueinv]",
            "-i[falseinv]",
            "-p[dotfile] -i[trueinv]",
            "-m",
            "-n",
            "-o",
            "-rjitty",
            "-rjittyc",
            "-s10 -i[trueinv]",
            "-t10 -i[trueinv]",
        ],
    ),
    ToolTest(
        "ltsinfo",
        "[ltsfile]",
        ["-v"],
    ),
    ToolTest(
        "lts2lps",
        "[ltsfile]",
        ["-m [mcrl2file]"],
    ),
    # TODO: Add action hiding --tau
    ToolTest(
        "ltsconvert",
        "[ltsfile]",
        list(
            map(
                lambda x: "-oaut " + x,
                [
                    "-D",
                    "-ebisim",
                    "-ebranching-bisim",
                    "-edpbranching-bisim",
                    "-esim",
                    "-etau-star",
                    "-etrace",
                    "-eweak-trace",
                    "-n",
                    "--no-reach",
                ],
            )
        ),
    ),
    ToolTest(
        "ltscompare",
        "[ltsfile] [ltsfile]",
        [
            "-c -ebranching-bisim",
            "-c -edpbranching-bisim",
            "-c -esim",
            "-c -etrace",
            "-c -eweak-trace",
            "-psim",
            "-pweak-trace",
        ],
    ),
    ToolTest("lps2pbes", "[lpsfile]", ["-f[mcffile]", "-t -f[mcffile]"]),
    ToolTest("lts2pbes", "[ltsfile]", ["-f[mcffile]"]),
    ToolTest("pbes2bool", "[pbesfile]", ["-v", "-f"]),
    ToolTest("pbespp", "[pbesfile]", ["-finternal", "-fdefault"]),
    ToolTest("pbesconstelm", "[pbesfile]", [ "-c", "-e", "-psimplify", "-pquantifier-all", "-pquantifier-finite", "-ppfnf", "-rjitty", "-rjittyp", "-rjittyc"]),
    ToolTest("pbesparelm", "[pbesfile]", ["-v"]),
    ToolTest("pbesrewr", "[pbesfile]", ["-psimplify", "-pquantifier-all", "-pquantifier-finite", "-ppfnf", "-rjitty", "-rjittyp", "-rjittyc"]),
    ToolTest("txt2lps", "[lpstxtfile]", ["-v"]),
    ToolTest("txt2lps", "[pbestxtfile]", ["-v"]),
    ToolTest("pbes2bes", "[pbesfile]", [ "-opbes", "-otext", "-obes", "-opgsolver", "-rjitty", "-rjittyp", "-rjittyc", "-s0", "-s1", "-s2", "-s3", "-u", "-zb", "-zd", "--erase=none", "--erase=some", "--erase=all"]),
    ToolTest("besinfo", "[pbesfile]", ["-v", "-f"]),
]


def run_test(toolpath, arguments):
    return RunProcess(toolpath, arguments, 1000, 2)


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

    with open(true_file, "w", encoding="utf-8") as f:
        f.write("true")
    with open(false_file, "w", encoding="utf-8") as f:
        f.write("false")

    # Create input files for lpsactionrename
    rename_file = os.path.join(output, "abp.rename")
    with open(rename_file, "w", encoding="utf-8") as f:
        f.write("act renamed;\n var x:D;\n rename r1(x) => renamed;")

    # Create a generic nodeadlock formula
    mcf_file = os.path.join(output, "nodeadlock.mcf")
    with open(mcf_file, "w", encoding="utf-8") as f:
        f.write("[true*]<true>true")
    pbes_file = os.path.join(output, "abp.nodeadlock.pbes")

    # Generate the input files for all tests beforehand
    run_test(os.path.join(args.toolpath, "mcrl22lps"), ["-D", mcrl2_file, lps_file])
    run_test(os.path.join(args.toolpath, "lps2lts"), [lps_file, lts_file])
    run_test(
        os.path.join(args.toolpath, "lps2pbes"), [lps_file, "-f", mcf_file, pbes_file]
    )

    try:
        with concurrent.futures.ThreadPoolExecutor(
            max_workers=args.max_workers
        ) as executor:
            # Start the test cases
            futures = {}
            for test in tests:
                for argument in test.options:
                    if "-rjittyc" in argument:
                        continue

                    # Replace placeholders by actual filenames
                    arguments = argument + " " + test.file_input
                    arguments = arguments.replace("[mcrl2file]", mcrl2_file)
                    arguments = arguments.replace("[lpsfile]", lps_file)
                    arguments = arguments.replace("[ltsfile]", lts_file)
                    arguments = arguments.replace("[trueinv]", true_file)
                    arguments = arguments.replace("[falseinv]", false_file)
                    arguments = arguments.replace("[rename]", rename_file)
                    arguments = arguments.replace("[mcffile]", mcf_file)
                    arguments = arguments.replace("[pbesfile]", pbes_file)

                    toolpath = os.path.join(args.toolpath, test.toolname)
                    if True:
                        print(f"Executing {test.toolname} {arguments}")
                        futures[
                            executor.submit(run_test, toolpath, arguments.split(" "))
                        ] = f"{test.toolname} {argument}"
                    # else:
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
