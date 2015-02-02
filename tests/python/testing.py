#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys

from random_bes_generator import make_bes
from random_pbes_generator import make_pbes, CounterExampleMinimizer
from random_process_generator import make_process_specification, generator_map
from replay import run_replay
from text_utility import write_text

MCRL2_ROOT = os.path.join(os.path.dirname(__file__), '..', '..')

def run_test(testfile, inputfiles, settings = dict()):
    reporterrors = True
    settings['toolpath'] = '../../stage/bin'
    settings['verbose'] = False
    #settings['verbose'] = True
    result, msg = run_replay(testfile, inputfiles, reporterrors, settings)
    print ', '.join(inputfiles), result, msg
    return result

def run_pbes_test(testfile, p, name, settings = dict()):
    filename = '{0}.txt'.format(name)
    with open(filename, 'w') as f:
        f.write(str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles, settings)

def run_pbes_test_with_counter_example_minimization(testfile, p, name, settings = dict()):
    result = run_pbes_test(testfile, p, name, settings)
    if result == False:
        m = CounterExampleMinimizer(p, lambda x: run_pbes_test(testfile, x, name + '_minimize', settings), name)
        m.minimize()
        raise RuntimeError('Test {0} failed'.format(name))
