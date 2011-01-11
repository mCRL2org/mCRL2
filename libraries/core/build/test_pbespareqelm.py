#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import platform
import re
from path import *
from random_pbes_generator import *
from mcrl2_tools import *

def test_pbes_pareqelm(filename, equation_count, atom_count = 5, propvar_count = 3, use_quantifiers = True):
    txtfile = filename + '.txt'
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    path(txtfile).write_text('%s' % p)
    pbesfile1 = filename + 'a.pbes'
    pbesfile2 = filename + 'b.pbes'
    run_txt2pbes(txtfile, pbesfile1)
    run_pbespareqelm(pbesfile1, pbesfile2)
    answer1 = run_pbes2bool(pbesfile1)
    answer2 = run_pbes2bool(pbesfile2)
    print filename, answer1, answer2
    if answer1 == None or answer2 == None:
      return
    if answer1 != answer2:
      raise Exception('Test %s.txt failed' % filename)

equation_count = 2
atom_count = 2
propvar_count = 2
use_quantifiers = True
for i in range(10000):
    test_pbes_pareqelm('%02d' % i, equation_count, atom_count, propvar_count, use_quantifiers)
