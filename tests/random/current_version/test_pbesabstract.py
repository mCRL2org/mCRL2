#!/usr/bin/env python

#~ Copyright 2010, 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_pbes_generator import *
from mcrl2_tools import *

def test_pbesabstract(p, filename, abstraction_value):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    pbesfile1 = filename + 'a.pbes'
    pbesfile2 = filename + 'b.pbes'
    run_txt2pbes(txtfile, pbesfile1)
    run_pbesabstract(pbesfile1, pbesfile2, abstraction_value)
    answer1 = run_pbes2bool(pbesfile1)
    answer2 = run_pbes2bool(pbesfile2)
    print filename, answer1, answer2, 'abstraction value =', abstraction_value
    if answer1 == None or answer2 == None:
      return True
    if abstraction_value != answer2:
      return answer1 == answer2
    return True

def main():
    options = parse_command_line()
    try:
        equation_count = 2
        atom_count = 2
        propvar_count = 2
        use_quantifiers = True

        for i in range(options.iterations):
            filename = 'pbes_abstract'
            p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
            abstraction_value = i % 2 == 0
            if not test_pbesabstract(p, filename, abstraction_value):
                m = CounterExampleMinimizer(p, lambda x: test_pbesabstract(x, filename + '_minimize', abstraction_value), 'pbesabstract')
                m.minimize()
                raise Exception('Test %s.txt failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
