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
  parser.add_argument("-r", "--release", dest="release", action="store_true",
                    help="Generate release documentation.")
  parser.add_argument("-f", "--force", dest="force", metavar="TARGET", action="append",
                    default=[],
                    choices=["all", "doxy", "doxyrst", "man", "libpdf"],
                    help="Force rebuild of selected targets. Choose from %(choices)s. "
                         "Use this flag more than once to force rebuilding multiple "
                         "targets.")
  parser.add_argument("-v", "--version", dest="version", metavar="REL.SVN", action="store",
                      default="unknown.unknown", 
                      help="The version number to be used in the documentation. This "
                           "should be a release number, followed by a period, followed "
                           "by the SVN revision number.")
  parser.add_argument("-t", "--temp", dest="temp", metavar="DIR", action="store",
                    default=os.path.join(os.path.dirname(__file__), '_temp'),
                    help="Store temporary files in DIR")
  parser.add_argument("-o", "--output", dest="out", metavar="DIR", action="store",
                    default=os.path.join(os.path.dirname(__file__), 'html'),
                    help="Store output to DIR")               
  return parser.parse_args()

#
# Main program
#
if __name__ == '__main__':

  args = getarguments()
  temppath = os.path.abspath(args.temp)
  outpath = os.path.abspath(args.out)
  if args.force:
    clearcache(temppath, args.force)
  if args.clean:
    clean(temppath, outpath)
  

  sys.path = [os.path.dirname(__file__)] + sys.path
  import source
  source.generate_rst('', temppath, outpath, [args.version] if args.release else args.version.rsplit('.', 1))
