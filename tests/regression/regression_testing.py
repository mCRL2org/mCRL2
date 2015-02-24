#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import copy
import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
from testcommand import YmlTest

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'stage', 'bin')

def ymlfile(file):
    return '{}/tests/specifications/{}.yml'.format(MCRL2_ROOT, file)

class LpsconfcheckTest(YmlTest):
    def __init__(self, name, inputfiles, confluence_type, expected_result, settings = dict()):
        assert confluence_type in 'cdCTZ'
        super(LpsconfcheckTest, self).__init__(name, ymlfile('lpsconfcheck'), inputfiles, settings)
        self.set_command_line_options('t2', ['-x' + confluence_type])
        self.settings.update({'result': "result = l6.value and l7.value['confluent_tau_summands'] == {}".format(expected_result)})

class LpsconfcheckCtauTest(YmlTest):
    def __init__(self, name, inputfiles, confluence_type, expected_result, settings = dict()):
        assert confluence_type in 'cdCTZ'
        super(LpsconfcheckCtauTest, self).__init__(name, ymlfile('lpsconfcheck_ctau'), inputfiles, settings)
        self.set_command_line_options('t3', ['-x' + confluence_type])
        self.settings.update({'result': "result = l7.value and l8.value['confluent_tau_summands'] == {0}".format(expected_result)})

class PbesrewrTest(YmlTest):
    def __init__(self, name, inputfiles, rewriter, settings = dict()):
        super(PbesrewrTest, self).__init__(name, ymlfile('pbesrewr'), inputfiles, settings)
        self.set_command_line_options('t2', ['-p' + rewriter])

class CountStatesTest(YmlTest):
    # expected_result is the expected number of states
    def __init__(self, name, inputfiles, expected_result, lps2lts_options = [], settings = dict()):
        super(CountStatesTest, self).__init__(name, ymlfile('countstates'), inputfiles, settings)
        self.settings.update({'nodes': {'l5': {'value': expected_result}}})
        if lps2lts_options:
            self.set_command_line_options('t2', lps2lts_options)

class PbesstategraphTest(YmlTest):
    def __init__(self, name, inputfiles, command_line_options, settings = dict()):
        super(PbesstategraphTest, self).__init__(name, ymlfile('pbesstategraph'), inputfiles, settings)
        self.set_command_line_options('t2', command_line_options)

if __name__ == '__main__':
    settings = {'toolpath': MCRL2_INSTALL_DIR, 'verbose': False, 'cleanup_files': True}
    testdir = 'output'
    if not os.path.exists(testdir):
        os.mkdir(testdir)
    os.chdir(testdir)
    LpsconfcheckTest('lpsconfcheck_1', [MCRL2_ROOT + '/examples/academic/cabp/cabp.mcrl2'], 'T', (0, 10), settings = settings).execute_in_sandbox()
    LpsconfcheckCtauTest('lpsconfcheck_2', [MCRL2_ROOT + '/examples/academic/cabp/cabp.mcrl2'], 'T', (0, 18), settings = settings).execute_in_sandbox()
    CountStatesTest('countstates_abp', [MCRL2_ROOT + '/examples/academic/abp/abp.mcrl2'], 74, settings = settings).execute_in_sandbox()
