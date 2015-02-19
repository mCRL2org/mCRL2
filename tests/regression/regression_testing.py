#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]

from text_utility import write_text
from testing import run_yml_test, cleanup_files

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'stage', 'bin')

def ymlfile(file):
    return '{}/tests/specifications/{}.yml'.format(MCRL2_ROOT, file)

# Example:
# inputfiles: ['1.mcrl2']
# command_line_options: ['-xT']
# expected_result: (0, 10) meaning 0 confluent tau summands (out of 10)
def run_lpsconfcheck_test(name, inputfiles, command_line_options, expected_result, settings):
    settings.update({'tools': {'t2': {'args': command_line_options}}})
    settings.update({'result': "result = l6.value and l7.value['confluent_tau_summands'] == {}".format(expected_result)})
    testfile = ymlfile('lpsconfcheck')
    run_yml_test(name, testfile, inputfiles, settings)

# Example:
# inputfiles: ['1.mcrl2']
# command_line_options: ['-xT']
# expected_result: (0, 10) meaning 0 confluent tau summands (out of 10)
def run_lpsconfcheck_ctau_test(name, inputfiles, command_line_options, expected_result, settings):
    settings.update({'tools': {'t3': {'args': command_line_options}}})
    settings.update({'result': "result = l7.value and l8.value['confluent_tau_summands'] == {}".format(expected_result)})
    testfile = ymlfile('lpsconfcheck_ctau')
    run_yml_test(name, testfile, inputfiles, settings)

# expected_result is the expected number of states
def run_countstates_test(name, inputfiles, expected_result, settings):
    settings.update({'nodes': {'l5': {'value': expected_result}}})
    testfile = ymlfile('countstates')
    run_yml_test(name, testfile, inputfiles, settings)

if __name__ == '__main__':
    import copy
    settings = {'toolpath': MCRL2_INSTALL_DIR, 'verbose': False, 'cleanup_files': True}
    testdir = 'output'
    if not os.path.exists(testdir):
        os.mkdir(testdir)
    os.chdir(testdir)
    run_lpsconfcheck_test('lpsconfcheck_triangular', [MCRL2_ROOT + '/examples/academic/cabp/cabp.mcrl2'], ['-xT'], (0, 10), copy.deepcopy(settings))
    run_countstates_test('countstates_abp', [MCRL2_ROOT + '/examples/academic/abp/abp.mcrl2'], 74, copy.deepcopy(settings))
