# Author(s): Wieger Wesselink, Jeroen Keiren
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
# 
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# 
#  \file print_manual_pages.py
#  \brief Generate (parts of the) tool manual pages in RST format

import os
import re
from optparse import OptionParser
from path import *

#extended_short_description = {

# change the line 'Written by X; Y' into 'Implemented by X, with contributions from Y'
def process_authors(filename):
  text = path(filename).text()
  text = re.compile(r'^Written by (.*)$', re.M).sub(r'Implemented by \1', text)
  text = re.compile(r'^(Implemented by .*);', re.M).sub(r'\1, with contributions from', text)
  path(filename).write_text(text)

def main():
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    parser.add_option("-m", "--mcrl2dir", dest="mcrl2dir", help="the location of the mcrl2 distribution")
    parser.add_option("-t", "--tooldir", dest="tooldir", help="the tools directory")
    parser.add_option("-g", "--generate-pages", action="store_true", help="generate manual pages for selected tools")
    (options, args) = parser.parse_args()

    # create subdirectory output/User_manual if it doesn't exist
    if not os.path.exists('tools'):
        os.makedirs('tools')

    if options.generate_pages:
        for dir in (path(options.mcrl2dir) / 'tools').dirs():
            toolname = dir.basename()

            # skip .svn directory
            if toolname.startswith('.'):
                continue

            tool_manual_file = dir / 'tool_manual.wiki'           

            destdir = path('tools') / toolname

            if not os.path.exists(destdir):
              os.makedirs(destdir)

            filename = path(destdir) / 'manual.xml'
            
            if options.tooldir == None:
                tool = toolname
            else:
                tool = path(options.tooldir) / toolname
            print tool
            os.system('%s --generate-xml > %s' % (tool, filename))
            process_authors(filename)

            text = filename.text()
            if not os.path.exists(tool_manual_file):
              mtext = ""
            else:
              mtext = tool_manual_file.text()
            #if len(mtext.strip()) > 0:
            #    text = re.compile(r'(== Short Description ==.*)(== Options ==)', re.S).sub(r'\1' + mtext + r'\2', text)
            filename.write_text(text)

if __name__ == '__main__':
    main()
