#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
from path import *
from random_bes_generator import make_bes
from random_pbes_generator import timeout_command

def last_word(line):
    words = line.strip().split()
    return words[len(words) - 1]

def test_bes(filename, equation_count, term_size = 3):
    txtfile = filename + '.txt'
    besfile = filename + '.bes'
    answerfile = 'temp.answer'
    p = make_bes(equation_count, term_size)
    path(txtfile).write_text('%s' % p)
    os.system('txt2bes %s %s' % (txtfile, besfile))

    # bessolve gauss
    text, dummy = timeout_command('bessolve -sgauss %s' % besfile, 3)
    if text == None:
      print 'ERROR: timeout on %s' % besfile
      return None
    answer1 = last_word(text)

    # bessolve spm
    text, dummy = timeout_command('bessolve -sspm %s' % besfile, 3)
    if text == None:
      print 'ERROR: timeout on %s' % besfile
      return None
    answer2 = last_word(text)

    # pbes2bool
    dummy, text = timeout_command('pbes2bool %s' % besfile, 3)
    if text == None:
      print 'ERROR: timeout on %s' % besfile
      return None
    answer3 = last_word(text)

    print 'FILE', filename, answer1, answer2, answer3
    #if answer1 != answer2 or (answer3 != 'unknown' and answer1 != answer3):
    #print 'FILE', filename, answer2, answer3
    #if answer2 != answer3:
    #    print 'ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!'
    #    path('error.txt').write_text('error in %s\n' % filename, append=True)

for i in range(10000):
    test_bes('%02d' % i, 4)
