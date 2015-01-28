#!/usr/bin/env python

#~ Copyright 2012 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_process_generator import *
from mcrl2_tools import *

def test_lps2pbes_structured(p, filename):
    mcrl2file = filename + '.mcrl2'
    path(mcrl2file).write_text('%s' % p)
    lpsfile = filename + '.lps'
    ltsfile = filename + '.lts'
    mcffile = 'nodeadlock.mcf'
    pbesfile = filename + '.pbes'
    path(mcffile).write_text('[true*]<true>true')
    add_temporary_files(mcffile)
    run_mcrl22lps(mcrl2file, lpsfile, '--no-alpha')
    text = run_lps2lts(lpsfile, ltsfile, '-D')
    answer1 = text.find('deadlock-detect: deadlock found') != -1
    run_lpspbes(lpsfile, mcffile, pbesfile, '--structured')
    answer2 = not run_pbes2bool(pbesfile)
    print answer1, answer2
    return answer1 == answer2

def main():
    options = parse_command_line()
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 7
    try:
        for i in range(options.iterations):
            filename = 'lps2pbes_structured'
            print i
            p = make_process_specification(generator_map, actions, process_identifiers, 7, parallel_operators = [make_block, make_hide, make_comm, make_allow])
            if not test_lps2pbes_structured(p, filename):
                raise Exception('Test %s.txt failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
