#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import copy
import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]

from text_utility import write_text
from testing import run_yml_test, cleanup_files
from testcommand import YmlTest

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'stage', 'bin')

def ymlfile(file):
    return '{}/tests/specifications/{}.yml'.format(MCRL2_ROOT, file)

class LpsconfcheckTest(YmlTest):
    # Example:
    # inputfiles: ['1.mcrl2']
    # command_line_options: ['-xT']
    # expected_result: (0, 10) meaning 0 confluent tau summands (out of 10)
    def __init__(self, name, inputfiles, command_line_options, expected_result, settings = dict()):
        settings = copy.deepcopy(settings)
        settings.update({'tools': {'t2': {'args': command_line_options}}})
        settings.update({'result': "result = l6.value and l7.value['confluent_tau_summands'] == {}".format(expected_result)})
        super(LpsconfcheckTest, self).__init__(name, ymlfile('lpsconfcheck'), inputfiles, settings)

class LpsconfcheckCtauTest(YmlTest):
    def __init__(self, name, inputfiles, command_line_options, expected_result, settings = dict()):
        settings = copy.deepcopy(settings)
        settings.update({'tools': {'t3': {'args': command_line_options}}})
        settings.update({'result': "result = l7.value and l8.value['confluent_tau_summands'] == {0}".format(expected_result)})
        super(LpsconfcheckCtauTest, self).__init__(name, ymlfile('lpsconfcheck_ctau'), inputfiles, settings)

class CountStatesTest(YmlTest):
    # expected_result is the expected number of states
    def __init__(self, name, inputfiles, expected_result, settings = dict()):
        settings = copy.deepcopy(settings)
        settings.update({'nodes': {'l5': {'value': expected_result}}})
        super(CountStatesTest, self).__init__(name, ymlfile('countstates'), inputfiles, settings)

if __name__ == '__main__':
    settings = {'toolpath': MCRL2_INSTALL_DIR, 'verbose': False, 'cleanup_files': True}
    testdir = 'output'
    if not os.path.exists(testdir):
        os.mkdir(testdir)
    os.chdir(testdir)
    LpsconfcheckTest('lpsconfcheck_triangular', [MCRL2_ROOT + '/examples/academic/cabp/cabp.mcrl2'], ['-xT'], (0, 10), settings).execute()
    CountStatesTest('countstates_abp', [MCRL2_ROOT + '/examples/academic/abp/abp.mcrl2'], 74, settings).execute()
