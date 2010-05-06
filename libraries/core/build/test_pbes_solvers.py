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
    os.system('txt2pbes %s %s' % (txtfile, pbesfile))

    # pbes2bool
    os.system('pbes2bool %s >& %s' % (pbesfile, answerfile))
    answer1 = last_word(path(answerfile).text())

    # pbespgsolve
    os.system('pbespgsolve -l1000 -d %s >& %s' % (pbesfile, answerfile))
    answer2 = last_word(path(answerfile).text())

    # bessolve
    os.system('pbes2bes -v -l1000 %s %s >& %s' % (pbesfile, besfile, answerfile))
    if last_word(path(answerfile).text()) == "limit":
        answer3 = 'unknown'
    else:
        os.system('bessolve %s >& %s' % (besfile, answerfile))
        answer3 = last_word(path(answerfile).text())

    print 'FILE', filename, answer1, answer2, answer3
    if answer1 != answer2 or (answer3 != 'unknown' and answer1 != answer3):
    #print 'FILE', filename, answer2, answer3
    #if answer2 != answer3:
        print 'ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!'
        path('error.txt').write_text('error in %s\n' % filename, append=True)

for i in range(10000):
    test_pbes('%02d' % i, 5, 4, 3)
