#~ Copyright 2012 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_pbes_generator import *
from mcrl2_tools import *

def test_symbolic_exploration(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    pbesfile1 = filename + '.pbes'
    pbesfile2 = filename + '_optimized_clustered.pbes'
    pbesfile3 = filename + '_optimized_not_clustered.pbes'
    pbesfile4 = filename + '_not_optimized_clustered.pbes'
    pbesfile5 = filename + '_not_optimized_not_clustered.pbes'
    run_txt2pbes(txtfile, pbesfile1)
    run_symbolic_exploration(pbesfile1, pbesfile2, '-o1 -c')
    run_symbolic_exploration(pbesfile1, pbesfile3, '-o1')
    run_symbolic_exploration(pbesfile1, pbesfile4, '-o0 -c')
    run_symbolic_exploration(pbesfile1, pbesfile5, '-o0')
    answer1 = run_pbes2bool(pbesfile1)
    answer2 = run_pbes2bool(pbesfile2)
    answer3 = run_pbes2bool(pbesfile3)
    answer4 = run_pbes2bool(pbesfile4)
    answer5 = run_pbes2bool(pbesfile5)
    answers = [answer1, answer2, answer3, answer4, answer5]
    print filename, answer1, answer2, answer3, answer4, answer5
    return not (True in answers and False in answers)

def main():
    options = parse_command_line()
    try:
        equation_count = 3
        atom_count = 4
        propvar_count = 3
        use_quantifiers = True

        for i in range(options.iterations):
            filename = 'symbolic_exploration%d' % i
            p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
            if not test_symbolic_exploration(p, filename):
                m = CounterExampleMinimizer(p, lambda x: test_symbolic_exploration(x, filename + '_minimize'), 'symbolic_exploration')
                m.minimize()
                raise Exception('Test %s.txt failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
