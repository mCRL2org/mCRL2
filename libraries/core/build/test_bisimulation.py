#!/usr/bin/env python

#~ Copyright 2012 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_process_generator import *
from mcrl2_tools import *

def test_bisimulation(p, filename):
    mcrl2file = filename + '.mcrl2'
    path(mcrl2file).write_text('%s' % p)
    lpsfile1 = filename + 'a.lps'
    lpsfile2 = filename + 'b.lps'
    ltsfile1 = filename + 'a.lts'
    ltsfile2 = filename + 'b.lts'
    pbesfile = filename + '.pbes'
    run_mcrl22lps(mcrl2file, lpsfile1, '-n')
    run_mcrl22lps(mcrl2file, lpsfile2, '-n --no-alpha')
    run_lps2lts(lpsfile1, ltsfile1)
    run_lps2lts(lpsfile2, ltsfile2)
    run_lpsbisim2pbes(lpsfile1, lpsfile2, pbesfile, '-bstrong-bisim')
    answer_text = run_ltscompare(ltsfile1, ltsfile2, '-ebisim')
    answer1 = answer_text.find('LTSs are strongly bisimilar') != -1
    answer2 = run_pbespgsolve(pbesfile)
    print filename, answer1, answer2
    return answer1 == answer2

def main():
    options = parse_command_line()
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    try:
        for i in range(options.iterations):
            filename = 'bisimulation_%d' % i
            p = make_process_specification(generator_map, actions, process_identifiers, 7)
            if not test_bisimulation(p, filename):
                raise Exception('Test %s.mcrl2 failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
