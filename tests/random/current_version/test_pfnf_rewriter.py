#!/usr/bin/env python

# ~ Copyright 2011-2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'python')]
from path import *
from random_pbes_generator import *
from replay import run_replay

def test_pfnf_rewriter(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    testfile = '../../random/tests/pbesrewr-pfnf.yml'
    inputfiles = [txtfile]
    reporterrors = True
    settings = dict()
    settings['toolpath'] = '../../../stage/bin'
    settings['verbose'] = False
    result, msg = run_replay(testfile, inputfiles, reporterrors, settings)
    print filename, result, msg
    return result

def test_with_counter_example_minimization(f, p, filename, name):
    result = f(p, filename)
    if result == False:
        m = CounterExampleMinimizer(p, lambda x: f(x, filename + '_minimize'), name)
        m.minimize()
        raise RuntimeError('Test %s.txt failed' % filename)

def main():
    options = parse_command_line()
    try:
        equation_count = 3
        atom_count = 4
        propvar_count = 3
        use_quantifiers = True

        for i in range(options.iterations):
            filename = 'pfnf%03d' % i
            p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
            test_with_counter_example_minimization(test_pfnf_rewriter, p, filename, 'pfnf')
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
