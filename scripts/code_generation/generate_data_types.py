#!/usr/bin/env python3
# Author(s): Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import re
from subprocess import call
from optparse import OptionParser

# Obtain the names of all files with the extension .spec in
# the current directory
def get_specifications():
  files = os.listdir(".")
  specs = []
  for file in files:
    parts = re.split("\.", file)
    if len(parts) == 2 and parts[1] == "spec":
      specs = specs + [parts[0]]
  return specs

# For all files in the current directory with the extension .spec, generated
# the code. For the file a.spec code is generated into 
#  ../include/mcrl2/data/a.h
def main():
  usage = "usage: %prog [options]"
  option_parser = OptionParser(usage)
  option_parser.add_option("-v", "--verbose", action="store_true", help="give verbose output")
  option_parser.add_option("-d", "--debug", action="store_true", help="give debug output")
  (options, args) = option_parser.parse_args()

  arguments = ""
  if options.verbose:
    arguments += " -v"
  if options.debug:
    arguments += " -d"

  # N.B. The script only works if the .spec files are in the current directory!
  os.chdir('data_types')
  specs = get_specifications()
  for spec in specs:
    print(("Generating code for %s" % (spec)))
    cmd = "python3 ./codegen.py %s %s.spec ../../../libraries/data/include/mcrl2/data/%s.h" % (arguments, spec, spec)
    retcode = call(cmd, shell=True)
    if retcode != 0:
      raise Exception("Failed to generate code for %s. Aborting... (while executing command %s)" % (spec, cmd))

if __name__ == "__main__":
  main()

