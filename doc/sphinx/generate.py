#!/usr/bin/env python
import os

#
# Main program
#
if __name__ == '__main__':
  import source
  temppath = os.path.abspath(os.path.join(os.path.dirname(__file__), '_temp'))
  outpath = os.path.abspath(os.path.join(os.path.dirname(__file__), 'html'))
  source.generate_rst('', temppath, outpath)
