#!/usr/bin/env python3
# Author(s): Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import pathlib
import subprocess
import sys
from argparse import ArgumentParser

SCRIPT_DIR = pathlib.Path(__file__).parent.resolve()

# Obtain the names of all files with the extension .spec in
# the current directory
def get_specifications(p: pathlib.Path):
    assert(p.is_dir())
    files = list(p.glob('*.spec'))
    specs = [str(spec.stem) for spec in files]
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

    arguments = []
    if options.verbose:
        arguments.append("-v")
    if options.debug:
        arguments.append("-d")

    # N.B. The script only works if the .spec files are in the current directory!
    data_types_dir = SCRIPT_DIR.joinpath("data_types")
    os.chdir(data_types_dir)
    specs = get_specifications(data_types_dir)
    for spec in specs:
        print(f"Generating code for {spec}")
        cmd = [f"{sys.executable}", data_types_dir.joinpath("codegen.py")] + arguments + [f"{spec}.spec", f"../../../libraries/data/include/mcrl2/data/{spec}.h"]

        completed_process = subprocess.run(cmd)
        if completed_process.returncode != 0:
            raise RuntimeError(
                f"Failed to generate code for {spec}. Aborting... (while executing command {cmd})"
            )


if __name__ == "__main__":
    main()
