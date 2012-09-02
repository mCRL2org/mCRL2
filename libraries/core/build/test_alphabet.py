#~ Copyright 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from path import *
from random_process_generator import *
from mcrl2_tools import *

def test_alphabet(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    lpsfile1 = filename + 'a.lps'
    lpsfile2 = filename + 'b.lps'
    ltsfile1 = filename + 'a.lts'
    ltsfile2 = filename + 'b.lts'
    run_mcrl22lps(txtfile, lpsfile1)
    run_mcrl22lps(txtfile, lpsfile2, '--no-alpha')
    run_lps2lts(lpsfile1, ltsfile1)
    run_lps2lts(lpsfile2, ltsfile2)
    answer = run_ltscompare(ltsfile1, ltsfile2, '-ebisim')
    return answer

def main():
    options = parse_command_line()
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 7
    try:
        for i in range(options.iterations):
            filename = 'alphabet'
            print i
            p = make_process_specification(generator_map, actions, process_identifiers, 7)
            if not test_alphabet(p, filename):
                raise Exception('Test %s.txt failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
