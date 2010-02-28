#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
from path import *

LPSPARSE = '../../lps/example/dist/lpsparse'

for file in path('../../../examples/academic').files('*.mcrl2'):
    print file
    cmd = '%s %s' % (LPSPARSE, file)
    os.system(cmd)