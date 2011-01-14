#~ Copyright 2010, 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_pbes_generator import *
from mcrl2_tools import *

def test_pbes2bes(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    pbesfile = filename + '.pbes'
    besfile = filename + '.bes'
    run_txt2pbes(txtfile, pbesfile)
    if run_pbes2bes(pbesfile, besfile, strategy = 'lazy'):
        answer1 = run_pbes2bool(pbesfile)
        answer2 = run_bessolve(besfile)
        print filename, answer1, answer2   
        if answer1 == None or answer2 == None:
          return True
        return answer1 == answer2
    return True

equation_count = 2
atom_count = 2
propvar_count = 2
use_quantifiers = True

for i in range(10000):
    filename = 'pbes2bes'
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    if not test_pbes2bes(p, filename):
        m = CounterExampleMinimizer(p, lambda x: test_pbes2bes(x, filename + '_minimize'), 'pbes2bes')
        m.minimize()
        raise Exception('Test %s.txt failed' % filename)
