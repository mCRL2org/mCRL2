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

for tool in TOOLS.split():
    os.system('..%sstage%s%s --generate-wiki-page' % (os.sep, os.sep, tool))
