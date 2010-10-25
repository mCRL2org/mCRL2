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

def redirect_command(cmd, filename):
    if platform.system() == 'Windows':
         arg = '%s 1> %s 2>&1' % (cmd, filename)
    else:
         arg = '%s >& %s' % (cmd, filename)
    os.system(arg)

def test_pbes_abstract(filename, abstraction_value, equation_count, atom_count = 5, propvar_count = 3):
    txtfile = filename + '.txt'
    besfile = filename + '.bes'
    pbesfile = filename + '.pbes'
    pbesfile2 = filename + '_abstract.pbes'
    answerfile = 'temp.answer'
    p = make_pbes(equation_count, atom_count, propvar_count)
    path(txtfile).write_text('%s' % p)
    os.system('txt2pbes %s %s' % (txtfile, pbesfile))
   
    # get the parameters
    lines = path(txtfile).lines()
    parameter_map = []
    for line in lines:
      line = re.sub(r'^pbes\s+', '', line)
      if (not line.startswith('mu ') and not line.startswith(r'nu ')) or not line.endswith(' = '):
        continue
      line = re.sub('\s+=\s+$', '', line)
      parameter_map.append[line]
    params = ';'.join(parameter_map)
    os.system('pbesabstract --select=%s --abstraction-value=%d %s %s' % (params, abstraction_value, pbesfile, pbesfile2))

    # pbes2bool
    redirect_command('pbes2bool %s' % pbesfile, answerfile)
    answer1 = last_word(path(answerfile).text())

    # pbes2bool
    redirect_command('pbes2bool %s' % pbesfile2, answerfile)
    answer2 = last_word(path(answerfile).text())

    answer1 = (answer1 == 'true')
    answer2 = (answer2 == 'true')
    print abstraction_value, answer1, answer2
    
    if abstraction_value != answer1:
        assert(answer1 == answer2)
   
for i in range(10000):
    test_pbes_abstract('%02d' % i, i % 2 == 0, 5, 4, 3)
