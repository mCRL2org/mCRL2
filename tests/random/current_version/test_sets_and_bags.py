#!/usr/bin/env python

#~ Copyright 2012 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import random
from path import *
from random_pbes_generator import *
from mcrl2_tools import *

EXPECTED_RESULT = '''pbes mu X1 =
       true;
     mu X2 =
       true;
     mu X3 =
       true;
     mu X4 =
       true;
     mu X5 =
       true;
     mu X6 =
       true;

init X1;
'''

def make_random_set(size, max_element):
    result = set()
    for i in range(size):
        result.add(random.randint(0, max_element))
    return result

def make_pbes(s1, s2):
    s12_union = s1 | s2
    s12_difference = s1 - s2
    s12_intersection = s1 & s2
    s1               = pp(s1              )
    s2               = pp(s2              )
    s12_union        = pp(s12_union       )
    s12_difference   = pp(s12_difference  )
    s12_intersection = pp(s12_intersection)
    s3 = set( [x for x in s1 if x % 2 == 0] )
    s4 = set( [x for x in s2 if x % 2 == 0] )
    s34_union = s3 | s4
    s34_difference = s3 - s4
    s34_intersection = s3 & s4
    s3               = '{ x : Nat | x in %s && x mod 2 == 0 }' % pp(s3)
    s4               = '{ x : Nat | x in %s && x mod 2 == 0 }' % pp(s4)
    s34_union        = pp(s34_union       )
    s34_difference   = pp(s34_difference  )
    s34_intersection = pp(s34_intersection)
    return '''pbes
mu X1 = val(%s + %s == %s) ;
mu X2 = val(%s - %s == %s) ;
mu X3 = val(%s * %s == %s) ;
mu X4 = val(%s + %s == %s) ;
mu X5 = val(%s - %s == %s) ;
mu X6 = val(%s * %s == %s) ;

init X1 ;
''' % (s1, s2, s12_union, s1, s2, s12_difference, s1, s2, s12_intersection, s3, s4, s34_union, s3, s4, s34_difference, s3, s4, s34_intersection)

def pp(s):
    return '{ %s }' % (', '.join(map(str, [x for x in s])))

def test_sets(filename, index):
    print index
    txtfile = filename + '.txt'
    pbesfile1 = filename + 'a.pbes'
    pbesfile2 = filename + 'b.pbes'

    s1 = make_random_set(6, 10)
    s2 = make_random_set(6, 10)

    p = make_pbes(s1, s2)
    path(txtfile).write_text(p)
    run_txt2pbes(txtfile, pbesfile1)
    run_pbesrewr(pbesfile1, pbesfile2)
    text = run_pbespp(pbesfile2)

    if text != EXPECTED_RESULT:
      print 'ERROR:'
      print p
      print ''
      print text
      return False
    return True

def main():
    options = parse_command_line()
    try:
        for i in range(options.iterations):
            filename = 'sets_and_bags'
            test_sets(filename, i)
    finally:
        if not options.keep_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
