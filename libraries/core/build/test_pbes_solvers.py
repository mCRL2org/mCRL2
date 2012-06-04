#~ Copyright 2010, 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_pbes_generator import *
from mcrl2_tools import *

def test_pbes_solvers(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    pbesfile = filename + '.pbes'
    run_txt2pbes(txtfile, pbesfile)
    answer1 = run_pbes2bool(pbesfile)
    answer2 = run_pbespgsolve(pbesfile)
    print filename, answer1, answer2
    if answer1 == None or answer2 == None:
      return True
    return answer1 == answer2

def main():
    options = parse_command_line()
    try:
        equation_count = 5
        atom_count = 4
        propvar_count = 3
        use_quantifiers = True

        for i in range(10000):
            filename = 'pbes_solvers'
            p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
            if not test_pbes_solvers(p, filename):
                m = CounterExampleMinimizer(p, lambda x: test_pbes_solvers(x, filename + '_minimize'), 'pbes_solvers')
                m.minimize()
                raise Exception('Test %s.txt failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
