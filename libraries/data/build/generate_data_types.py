import os
import re
from subprocess import call
from optparse import OptionParser

def get_specifications():
  files = os.listdir(".")
  specs = []
  for file in files:
    parts = re.split("\.", file)
    if len(parts) == 2 and parts[1] == "spec":
      specs = specs + [parts[0]]
  return specs

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

  specs = get_specifications()
  for spec in specs:
    print "Generating code for %s" % (spec)
    cmd = "python ./codegen.py %s %s.spec ../include/mcrl2/data/%s.h" % (arguments, spec, spec)
    retcode = call(cmd, shell=True)

if __name__ == "__main__":
  main()

