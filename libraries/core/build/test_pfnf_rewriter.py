#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import platform
import re
from path import *
from random_pbes_generator import *

def last_word(line):
    words = line.strip().split()
    return words[len(words) - 1]

# performs the test with pbes   filename + '.txt'
# returns True if the test succeeds
def run_test(filename):
    txtfile = filename + '.txt'
    pbesfile = filename + '.pbes'
    pbesfile2 = filename + '_rewr.pbes'
    answerfile = 'temp.answer'
    run_program('txt2pbes', '%s %s' % (txtfile, pbesfile))

    run_program('pbesrewr', '-ppfnf %s %s' % (pbesfile, pbesfile2))

    # pbes2bool
    dummy, text = timeout_command('pbes2bool %s' % pbesfile, 3)
    if text == None:
      print 'ERROR: timeout on %s' % pbesfile
      return None
    answer1 = last_word(text)

    # pbes2bool
    dummy, text = timeout_command('pbes2bool %s' % pbesfile2, 3)
    if text == None:
      print 'ERROR: timeout on %s' % pbesfile2
      return None
    answer2 = last_word(text)

    answer1 = (answer1 == 'true')
    answer2 = (answer2 == 'true')
    print filename, answer1, answer2
    
    return answer1 == answer2

def test_pfnf_rewriter(filename, equation_count, atom_count = 5, propvar_count = 3):
    txtfile = filename + '.txt'
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers = True)   
    path(txtfile).write_text('%s' % p)
    if run_test(filename) == False:
      raise Exception('Test %s.txt failed' % filename)

equation_count = 2
atom_count = 2
propvar_count = 2
for i in range(10000):
    test_pfnf_rewriter('%02d' % i, equation_count, atom_count, propvar_count)
