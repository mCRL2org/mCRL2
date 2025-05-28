#!/usr/bin/env python3

# Copyright 2024-2025 Maurice Laveaux
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import sys

SCRIP_PATH = os.path.dirname(__file__)

# Makes sure that the script can find the modules when ran directly.
sys.path.append(os.path.join(SCRIP_PATH, "../../"))
from tests.utility.run_process import RunProcess, TimeExceededError, MemoryExceededError


def main():
    """Finds all the .mcrl2 files in the example directory and tries to parse them with mcrl22lps."""

    # Parse some configuration options
    parser = argparse.ArgumentParser(
        prog="run_examples.py",
        description="Prepares the examples specifications for testing",
        epilog="",
    )

    parser.add_argument("-i", "--timeout", action="store", default=60, type=int)
    parser.add_argument("-r", "--jittyc", action="store_true")
    parser.add_argument(
        "-t", "--mcrl2-toolpath", action="store", type=str, required=True
    )
    parser.add_argument(
        "-l", "--ltsmin-toolpath", action="store", type=str, required=True
    )
    parser.add_argument(
        "-o",
        "--output",
        dest="output",
        metavar="o",
        action="store",
        help="Run the tests in the given directory, where the outputs are produced",
    )

    args = parser.parse_args()

    output = "."
    if args.output:
        output = os.path.join(os.getcwd(), args.output)

    mcrl22lps_exe = os.path.join(args.mcrl2_toolpath, "mcrl22lps")
    ltscompare_exe = os.path.join(args.mcrl2_toolpath, "ltscompare")

    lps2lts_seq_exe = os.path.join(args.ltsmin_toolpath, "lps2lts-seq")
    lps2lts_mc_exe = os.path.join(args.ltsmin_toolpath, "lps2lts-mc")
    lps2lts_sym_exe = os.path.join(args.ltsmin_toolpath, "lps2lts-sym")
    etf2lts_seq_exe = os.path.join(args.ltsmin_toolpath, "etf2lts-seq")
    ltsmin_convert_exe = os.path.join(args.ltsmin_toolpath, "ltsmin-convert")

    # Start the linearisation process
    for example in [
        "examples/academic/abp/abp.mcrl2",
        "examples/academic/allow/allow.mcrl2",
        "examples/academic/cabp/cabp.mcrl2",
        "examples/academic/dining/dining8.mcrl2",
        "examples/industrial/1394/1394-fin.mcrl2",
        "examples/industrial/brp/brp.mcrl2",
        "examples/industrial/chatbox/chatbox.mcrl2",
        "examples/industrial/lift/lift3-final.mcrl2",
        "examples/industrial/lift/lift3-init.mcrl2",
    ]:
        mcrl2_path = os.path.join(SCRIP_PATH, "../../", example)

        name, _ = os.path.splitext(os.path.basename(mcrl2_path))

        arguments = []
        if args.jittyc:
            arguments = ["--mcrl2-rewriter=jittyc"]
        else:
            arguments = ["--mcrl2-rewriter=jitty"]

        for arg in arguments:
            print(f"Testing example {name} with arg {arg}")

            RunProcess(
                mcrl22lps_exe,
                ["-D", "-f", "-n", mcrl2_path, os.path.join(output, f"{name}.lps")],
                2000,
                args.timeout,
            )

            # Run all the different state space exploration algorithms, convert to aut file and check equivalences to the sequential result.
            try:
                print("running lps2lts-seq")
                RunProcess(
                    lps2lts_seq_exe,
                    [
                        os.path.join(output, f"{name}.lps"),
                        os.path.join(output, f"{name}.seq.gcf"),
                    ]
                    + [arg],
                    2000,
                    args.timeout,
                )
                RunProcess(
                    ltsmin_convert_exe,
                    [
                        "--rdwr",
                        "--filter=action_labels",
                        os.path.join(output, f"{name}.seq.gcf"),
                        os.path.join(output, f"{name}.seq.aut"),
                    ],
                    2000,
                    args.timeout,
                )

                print("running lps2lts-mc")
                RunProcess(
                    lps2lts_mc_exe,
                    [
                        "--procs=1",
                        os.path.join(output, f"{name}.lps"),
                        os.path.join(output, f"{name}.mc.gcf"),
                    ]
                    + [arg],
                    2000,
                    args.timeout,
                )
                RunProcess(
                    ltsmin_convert_exe,
                    [
                        "--rdwr",
                        "--filter=action_labels",
                        os.path.join(output, f"{name}.mc.gcf"),
                        os.path.join(output, f"{name}.mc.aut"),
                    ],
                    2000,
                    args.timeout,
                )

                ltscompare = RunProcess(
                    ltscompare_exe,
                    [
                        "-ebisim",
                        os.path.join(output, f"{name}.seq.aut"),
                        os.path.join(output, f"{name}.mc.aut"),
                    ],
                    2000,
                    args.timeout,
                )
                print(ltscompare.stdout)
                if "false" in ltscompare.stdout:
                    raise RuntimeError("Invalid multi-core result")

                print("running lps2lts-sym")
                RunProcess(
                    lps2lts_sym_exe,
                    [
                        os.path.join(output, f"{name}.lps"),
                        os.path.join(output, f"{name}.sym.etf"),
                    ]
                    + [arg],
                    2000,
                    args.timeout,
                )
                RunProcess(
                    etf2lts_seq_exe,
                    [
                        os.path.join(output, f"{name}.sym.etf"),
                        os.path.join(output, f"{name}.sym.gcf"),
                    ],
                    2000,
                    args.timeout,
                )
                RunProcess(
                    ltsmin_convert_exe,
                    [
                        "--rdwr",
                        "--filter=action_labels",
                        os.path.join(output, f"{name}.sym.gcf"),
                        os.path.join(output, f"{name}.sym.aut"),
                    ],
                    2000,
                    args.timeout,
                )

                ltscompare = RunProcess(
                    ltscompare_exe,
                    [
                        "-ebisim",
                        os.path.join(output, f"{name}.seq.aut"),
                        os.path.join(output, f"{name}.sym.aut"),
                    ],
                    2000,
                    args.timeout,
                )
                print(ltscompare.stdout)
                if "false" in ltscompare.stdout:
                    raise RuntimeError("Invalid symbolic result")

            except (TimeExceededError, MemoryExceededError) as e:
                print(f"Tool timed out or reached max memory ({e})")


if __name__ == "__main__":
    main()
