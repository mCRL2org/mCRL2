#!/usr/bin/env python

#~ Copyright 2012 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import sys
sys.path.append('../python')
from path import *
from random_pbes_generator import *
from replay import run_replay

def test_pbesstategraph(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    testfile = '../random/tests/pbesstategraph.yml'
    inputfiles = [txtfile]
    reporterrors = True
    settings = dict()
    settings['verbose'] = False
    settings['toolpath'] = '../../tools/bin'
    result = run_replay(testfile, inputfiles, reporterrors, settings)
    print filename, result
    return result

def main():
    options = parse_command_line()
    try:
        equation_count = 4
        atom_count = 5
        propvar_count = 4
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
