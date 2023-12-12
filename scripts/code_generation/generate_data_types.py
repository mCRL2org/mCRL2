#!/usr/bin/env python3
# Author(s): Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
from subprocess import call
from argparse import ArgumentParser

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))


# Obtain the names of all files with the extension .spec in
# the current directory
def get_specifications():
    specs = []
    for file in os.listdir("."):
        base, ext = os.path.splitext(file)
        if ext == ".spec":
            specs.append(base)
    return specs


# For all files in the current directory with the extension .spec, generated
# the code. For the file a.spec code is generated into
#  ../include/mcrl2/data/a.h
def main():
    option_parser = ArgumentParser()
    option_parser.add_argument(
        "-v", "--verbose", action="store_true", help="give verbose output"
    )
    option_parser.add_argument(
        "-d", "--debug", action="store_true", help="give debug output"
    )
    options = option_parser.parse_args()

    arguments = ""
    if options.verbose:
        arguments += " -v"
    if options.debug:
        arguments += " -d"

    # N.B. The script only works if the .spec files are in the current directory!
    os.chdir(os.path.join(SCRIPT_DIR, "data_types"))
    specs = get_specifications()
    for spec in specs:
        print(f"Generating code for {spec}")
        cmd = f"{sys.executable} ./codegen.py {arguments} {spec}.spec ../../../libraries/data/include/mcrl2/data/{spec}.h"
        retcode = call(cmd, shell=True)
        if retcode != 0:
            raise RuntimeError(
                f"Failed to generate code for {spec}. Aborting... (while executing command {cmd})"
            )


if __name__ == "__main__":
    main()
