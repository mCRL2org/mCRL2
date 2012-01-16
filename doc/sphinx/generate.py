#!/usr/bin/env python
import os
import sys
import shutil
import argparse

def clean(temppath, outpath):
  if os.path.exists(temppath):
    shutil.rmtree(temppath)
  usr = os.path.join(outpath, 'user_manual')
  dev = os.path.join(outpath, 'developer_manual')
  if os.path.exists(usr):
    shutil.rmtree(usr)
  if os.path.exists(dev):
    shutil.rmtree(dev)
  sys.exit(0)

def clearcache(temppath, tags):
  for tag in tags:
    try:
      os.unlink(os.path.join(temppath, tag + '.cached'))
    except OSError:
      pass

def getarguments():
  parser = argparse.ArgumentParser()
  parser.add_argument("-c", "--clean", dest="clean", action="store_true",
                    help="Clean up all generated files (output + temporary).")
  parser.add_argument("-f", "--force", dest="force", metavar="TARGET", action="append",
                    default=[],
                    choices=["all", "doxy", "doxyrst", "man", "libpdf"],
                    help="Force rebuild of selected targets. Choose from %(choices)s. "
                         "Use this flag more than once to force rebuilding multiple "
                         "targets.")
  return parser.parse_args()

#
# Main program
#
if __name__ == '__main__':
  temppath = os.path.abspath(os.path.join(os.path.dirname(__file__), '_temp'))
  outpath = os.path.abspath(os.path.join(os.path.dirname(__file__), 'html'))

  args = getarguments()
  if args.force:
    clearcache(temppath, args.force)
  if args.clean:
    clean(temppath, outpath)
  
  import source
  source.generate_rst('', temppath, outpath)
