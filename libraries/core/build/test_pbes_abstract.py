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
def run_test(filename, abstraction_value):
    txtfile = filename + '.txt'
    besfile = filename + '.bes'
    pbesfile = filename + '.pbes'
    pbesfile2 = filename + '_abstract.pbes'
    answerfile = 'temp.answer'
    run_program('txt2pbes', '%s %s' % (txtfile, pbesfile))

    run_program('pbesabstract', '--select=*\(*:*\) --abstraction-value=%s %s %s' % (abstraction_value, pbesfile, pbesfile2))

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
    print filename, abstraction_value, answer1, answer2
    
    if abstraction_value != answer2:
      return answer1 == answer2
    else:
      return True

def remove_equation(var, p):
  p = re.sub('[mn]u %s.*' % var, '', p)
  p = re.sub(r'%s\([^()]*\)' % var, 'val(true)', p)
  p = re.sub(var, 'val(true)', p)
  return p

def test_pbes_abstract(filename, abstraction_value, equation_count, atom_count = 5, propvar_count = 3):
    txtfile = filename + '.txt'
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers = True)   
    path(txtfile).write_text('%s' % p)
    if run_test(filename, abstraction_value) == False:
      raise Exception('Test %s.txt failed' % filename)
   
for i in range(10000):
    test_pbes_abstract('%02d' % i, i % 2, 4, 3, 2)
