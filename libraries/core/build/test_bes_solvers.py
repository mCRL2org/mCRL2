#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
from path import *
from random_bes_generator import make_bes
from mcrl2_tools import *

def last_word(line):
    words = line.strip().split()
    return words[len(words) - 1]

def test_bes(filename, equation_count, term_size = 2, error_file = 'bes_errors.txt'):
    txtfile = filename + '.txt'
    besfile = filename + '.bes'
    answerfile = 'temp.answer'
    p = make_bes(equation_count, term_size)
    path(txtfile).write_text('%s' % p)
    os.system('txt2bes %s %s' % (txtfile, besfile))

def test_bessolve(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    besfile = filename + '.bes'
    run_txt2bes(txtfile, besfile)

    # bessolve gauss
    answer1 = run_bessolve(besfile, strategy = 'gauss')

    # bessolve spm
    answer2 = run_bessolve(besfile, strategy = 'spm')

    # pbes2bool
    answer3 = run_pbes2bool(besfile)

    print filename, answer1, answer2, answer3
    if answer1 == None or answer2 == None or answer3 == None:
      return True
    return answer1 == answer2 and answer1 == answer3

def main():
    options = parse_command_line()
    try:
        equation_count = 4
        term_size = 3

        for i in range(10000):
            filename = 'bessolve'
            p = make_bes(equation_count, term_size)
            if not test_bessolve(p, filename):
                print p
                raise Exception('Test %s.txt failed' % filename)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()


for i in range(10000):
    test_bes('%02d' % i, 4, 3, error_file)
