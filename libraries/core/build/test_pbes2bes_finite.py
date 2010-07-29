#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
from path import *
from random_pbes_generator import *

def last_word(line):
    words = line.strip().split()
    return words[len(words) - 1]

def test_pbes(filename, equation_count, atom_count = 5, propvar_count = 3):
    txtfile = filename + '.txt'
    besfile = filename + '.bes'
    pbesfile = filename + '.pbes'
    answerfile = 'temp.answer'
    p = make_pbes(equation_count, atom_count, propvar_count)
    path(txtfile).write_text('%s' % p)
    run_program('txt2pbes', '%s %s' % (txtfile, pbesfile))

    # pbes2bool
    #run_program('pbes2bool', pbesfile, answerfile)
    #answer1 = last_word(path(answerfile).text())

    # pbespgsolve
    run_program('pbespgsolve', '-l1000 -d %s' % pbesfile, answerfile)
    answer1 = last_word(path(answerfile).text())

    # bessolve
    run_program('pbes2bes', '-sfinite -v -l1000 %s %s' % (pbesfile, besfile), answerfile)
    if last_word(path(answerfile).text()) == "limit":
        answer2 = 'unknown'
    else:
        #run_program('pbes2bool', besfile, answerfile)
        run_program('pbespgsolve', '-l1000 -d %s' % besfile, answerfile)
        answer2 = last_word(path(answerfile).text())

    print 'FILE', filename, answer1, answer2
    if answer1 != answer2:
        print 'ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!'
        path('error.txt').write_text('error in %s\n' % filename, append=True)

for i in range(10000):
    test_pbes('%02d' % i, 2, 3, 2)
