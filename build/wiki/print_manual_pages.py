# Author(s): Wieger Wesselink
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
# 
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# 
#  \file print_manual_pages.py
#  \brief Generate (parts of the) tool manual pages in wikimedia format

import os
import re
from optparse import OptionParser
from path import *

TOOLS = '''
  lps2pbes
  lpsbisim2pbes
  lpsconstelm
  lpsinfo
  lpsparelm
  lpsrewr
  pbes2bes
  pbesconstelm
  pbesinfo
  pbesparelm
  pbespareqelm
  pbespgsolve
  pbesrewr
  txt2lps
  txt2pbes
'''

# Some tools have additional information in the short description that is not present
# in the command line help.
extended_short_description = {
  'lpsconstelm' : '''

If it can be determined that certain parameters of this LPS remain constant throughout any run of the process, all occurrences of these process parameter are replaced by the initial value and the process parameters are removed from the LPS.

If the initial value of a process parameter is a global variable and remains a global variable throughout the run of the process, the process variable is considered constant.

If the initial value of a process parameter is a global variable and is only changed once to a certain value, the process parameter is constant and the specific value is used for substitution.
''',
  'lps2pbes' : '''

The concrete syntax of state formulas can be found at 
<http://www.mcrl2.org/mcrl2/wiki/index.php/Language_reference/mu-calculus_syntax>.
''',
  'lpsinfo' : '''

By default, the following information about the LPS is shown: 
* number of summands
* number of tau-summands
* number of free variables
* number of process parameters
* number of action labels
* number of used versus the number of declared actions
* number of sorts
''',
  'lpsparelm' : '''

Removing parameters from the LPS can lead to a reduction when generating a state space of an LPS.
''',
  'lpsrewr' : '''

The following data expressions are rewritten: 
* conditions, action parameters, time expressions and next states of LPS summands
* process parameters of the initial state
* conditions and right-hand sides of data equations

Rewriting LPS summands and the initial state is done to simplify these parts of the LPS. Rewriting data equations is done to speed up state space generation. In most cases, this results in a performance gain of at most 5%.
''',
  'pbesinfo' : '''

By default, the following information about the PBES is shown: 
* information if the PBES is closed and well-formed;
* number of equations, &mu;s and &nu;s.
''',
  'txt2pbes' : '''

The textual description should adhere to the following [[Language reference/PBES syntax|BNF syntax description]].
''',
}

# change the line 'Written by X; Y' into 'Implemented by X, with contributions from Y'
def process_authors(filename):
  text = path(filename).text()
  text = re.compile(r'^Written by (.*)$', re.M).sub(r'Implemented by \1', text)
  text = re.compile(r'^(Implemented by .*);', re.M).sub(r'\1, with contributions from', text)
  path(filename).write_text(text)

def main():
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    parser.add_option("-t", "--tooldir", dest="tooldir", help="the tools directory")
    parser.add_option("-g", "--generate-pages", action="store_true", help="generate manual pages for selected tools")
    parser.add_option("-i", "--import-pages", action="store_false", help="import manual pages into mediawiki")
    parser.add_option("-u", "--user", dest="user", help="the mediawiki user name")
    (options, args) = parser.parse_args()

    # create subdirectory output/User_manual if it doesn't exist
    if not os.path.exists('output/User_manual'):
        os.makedirs('output/User_manual')

    if options.generate_pages:
        # generate tool pages
        for toolname in TOOLS.split():
            filename = 'output/User_manual%s%s' % (os.sep, toolname)
            if options.tooldir == None:
                tool = toolname
            else:
                tool = path(options.tooldir) / toolname
            print tool
            os.system('%s --generate-wiki-page > %s' % (tool, filename))
            process_authors(filename)
        
        # update tool pages with custom additions
        for toolname in TOOLS.split():
            filename = 'output/User_manual%s%s' % (os.sep, toolname)
            manual = path(filename).text()
            if extended_short_description.has_key(toolname):
                manual = re.compile(r'(== Short Description ==.*)(== Options ==)', re.S).sub(r'\1' + extended_short_description[tool] + r'\2', manual)
                path(filename).write_text(manual)

        # import changes into mediawiki
        if options.import_pages:
            os.chdir('output')
            for file in path('.').walkfiles():
                file = re.sub(r'^\.\/', '', file)
                cmd = 'php /var/www/win/mcrl2/wiki/maintenance/importTextFile.php --user %s --comment update --title %s %s' % (options.user, file, file)
                os.system(cmd)

if __name__ == '__main__':
    main()
