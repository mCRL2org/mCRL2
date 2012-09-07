#~ Copyright 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_pbes_generator import *
from mcrl2_tools import *

def test_pbesstategraph(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    pbesfile1 = filename + 'a.pbes'
    pbesfile2 = filename + 'b.pbes'
    pbesfile3 = filename + 'c.pbes'
    run_txt2pbes(txtfile, pbesfile1)
    run_pbesstategraph(pbesfile1, pbesfile2)
    run_pbesstategraph(pbesfile1, pbesfile3, '-s0')
    answer1 = run_pbes2bool(pbesfile1)
    answer2 = run_pbes2bool(pbesfile2)
    answer3 = run_pbes2bool(pbesfile3)
    print filename, answer1, answer2, answer3
    answers = [answer1, answer2, answer3]
    return not (True in answers and False in answers)

def main():
    options = parse_command_line()
    try:
        equation_count = 3
        atom_count = 4
        propvar_count = 3
        use_quantifiers = True

        for i in range(options.iterations):
            filename = 'pbes_stategraph_%d' % i
            p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
            if not test_pbesstategraph(p, filename):
                m = CounterExampleMinimizer(p, lambda x: test_pbesstategraph(x, filename + '_minimize'), 'pbesstategraph')
                m.minimize()
                raise Exception('Test %s.txt failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
