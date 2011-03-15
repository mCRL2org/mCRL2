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

def test_bes(filename, equation_count, term_size = 2, error_file = 'bes_errors.txt'):
    txtfile = filename + '.txt'
    besfile = filename + '.bes'
    answerfile = 'temp.answer'
    p = make_bes(equation_count, term_size)
    path(txtfile).write_text('%s' % p)
    os.system('txt2bes %s %s' % (txtfile, besfile))

    # bessolve gauss
    cmd = 'bessolve -sgauss %s' % besfile
    text, dummy = timeout_command(cmd, 3)
    if text == None:
      print "timeout on command '%s'" % cmd
      return None
    answer1 = last_word(text)

    # bessolve spm
    cmd = 'bessolve -sspm %s' % besfile
    text, dummy = timeout_command(cmd, 3)
    if text == None:
      print "timeout on command '%s'" % cmd
      return None
    answer2 = last_word(text)

    # pbes2bool
    cmd = 'pbes2bool %s' % besfile
    dummy, text = timeout_command(cmd, 3)
    if text == None:
      print "timeout on command '%s'" % cmd
      return None
    answer3 = last_word(text)

    print 'FILE', filename, answer1, answer2, answer3
    if answer1 != answer2 or answer2 != answer3:
        print 'ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!'
        path(error_file).write_text(filename + ' ' + answer1 + ' ' + answer2 + ' ' + answer3, append = True)

error_file = 'bes_errors.txt'
path(error_file).write_text('')
for i in range(10000):
    test_bes('%02d' % i, 4, 3, error_file)
