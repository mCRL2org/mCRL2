#!/usr/bin/env python3

# Copyright 2024-2025 Maurice Laveaux
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import re

init_regex = re.compile(r"init (.*);")
proc_regex = re.compile(r"proc ([\s\S]*);")
process_regex = re.compile(r"P\((.*)\)")


def main():
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument(
        "FILE1", metavar="FILE1", type=str, help="the first mCRL2 file"
    )
    cmdline_parser.add_argument(
        "FILE2", metavar="FILE2", type=str, help="the second mCRL2 file"
    )
    cmdline_parser.add_argument(
        "OUTPUT", metavar="OUTPUT", type=str, help="the output directory"
    )
    cmdline_parser.add_argument(
        "--allow", type=str, action="store", help="The action names for the allow block"
    )
    cmdline_parser.add_argument(
        "--hide", type=str, action="store", help="The action names for the hide block"
    )
    cmdline_parser.add_argument(
        "--comm", type=str, action="store", help="The communications for the comm block"
    )
    args = cmdline_parser.parse_args()

    # Read the mCRL2 files, strip the init blocks and create a new init block
    with open(args.FILE1, "r", encoding="utf-8") as file1:
        with open(args.FILE2, "r", encoding="utf-8") as file2:
            file1_txt = file1.read()
            file2_txt = file2.read()

            init_p = ""
            init_q = ""

            # Replace P by Q
            q_process = re.sub(
                process_regex, lambda match: f"Q({match.group(1)})", file2_txt
            )

            # Read the init processes and strip them from the mcrl2 file
            def match_init_p(match):
                nonlocal init_p
                init_p = match.group(1)
                return ""

            def match_init_q(match):
                nonlocal init_q
                init_q = match.group(1)
                return ""

            p_process = re.sub(init_regex, lambda match: match_init_p(match), file1_txt)
            q_process = re.sub(init_regex, lambda match: match_init_q(match), q_process)

            # Add the Terminate action when it occurs in Q, but not in P
            if "Terminate" in q_process and "Terminate" not in p_process:
                p_process = f"act Terminate; \n {p_process}"

            # Strip the data specification
            result = re.search(proc_regex, q_process)
            assert result is not None

            q_process = result.group(1)

            with open(args.OUTPUT, "w", encoding="utf-8") as output:
                output.write(p_process)
                output.write("\n")
                output.write(f"proc {q_process};\n\n")

                # Write the new init process.
                output.write(
                    f"init hide({{ {args.hide} }}, allow({{ {args.allow} }}, comm({{ {args.comm} }}, {init_p} || {init_q})));"
                )


if __name__ == "__main__":
    main()
