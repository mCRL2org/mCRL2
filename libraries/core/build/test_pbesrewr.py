#~ Copyright 2010, 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_pbes_generator import *
from mcrl2_tools import *

def test_pbesrewr1(p, filename, pbes_rewriter):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    pbesfile1 = filename + '1.pbes'
    pbesfile2 = filename + '2.pbes'
    run_txt2pbes(txtfile, pbesfile1)
    run_pbesrewr(pbesfile1, pbesfile2, pbes_rewriter)
    run_txt2pbes(txtfile, pbesfile1)
    answer1 = run_pbes2bool(pbesfile1)
    answer2 = run_pbes2bool(pbesfile2)
    print filename, pbes_rewriter, answer1, answer2
    if answer1 == None or answer2 == None:
      return True
    return answer1 == answer2

def test_pbesrewr(p, filename):
    result = True
    result = result and test_pbesrewr1(p, filename, 'simplify')
    result = result and test_pbesrewr1(p, filename, 'quantifier-all')
    result = result and test_pbesrewr1(p, filename, 'quantifier-finite')
    result = result and test_pbesrewr1(p, filename, 'quantifier-one-point')
    result = result and test_pbesrewr1(p, filename, 'pfnf')
    result = result and test_pbesrewr1(p, filename, 'bqnf-quantifier')
    return result

def main():
    options = parse_command_line()
    try:
        equation_count = 5
        atom_count = 4
        propvar_count = 3
        use_quantifiers = True

        for i in range(options.iterations):
            filename = 'pbesrewr'
            p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
            if not test_pbesrewr(p, filename):
                m = CounterExampleMinimizer(p, lambda x: test_pbes_solvers(x, filename + '_minimize'), 'pbesrewr')
                m.minimize()
                raise Exception('Test %s.txt failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
