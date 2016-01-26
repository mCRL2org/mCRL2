#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import copy
import os
import sys
sys.path += [os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'python'))]
from testcommand import YmlTest

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'stage', 'bin')

def abspath(file):
    return os.path.abspath(os.path.join(os.path.dirname(__file__), file))

def mcrl2file(file):
    return os.path.abspath(MCRL2_ROOT + file)

def ymlfile(file):
    return '{}/tests/specifications/{}.yml'.format(MCRL2_ROOT, file)

class LpsconfcheckTest(YmlTest):
    def __init__(self, name, inputfiles, confluence_type, expected_result, settings = dict()):
        assert confluence_type in 'cdCTZ'
        super(LpsconfcheckTest, self).__init__(name, ymlfile('lpsconfcheck'), inputfiles, settings)
        self.set_command_line_options('t2', ['-x' + confluence_type])
        self.settings.update({'result': "result = t5.value['result'] and t2.value['confluent-tau-summand-count'] == {}  and t2.value['tau-summand-count'] == {}".format(expected_result[0], expected_result[1])})

class LpsconfcheckCtauTest(YmlTest):
    def __init__(self, name, inputfiles, confluence_type, expected_result, settings = dict()):
        assert confluence_type in 'cdCTZ'
        super(LpsconfcheckCtauTest, self).__init__(name, ymlfile('lpsconfcheck_ctau'), inputfiles, settings)
        self.set_command_line_options('t3', ['-x' + confluence_type])
        self.settings.update({'result': "result = t6.value['result'] and t3.value['confluent-tau-summand-count'] == {}  and t3.value['tau-summand-count'] == {}".format(expected_result[0], expected_result[1])})

class PbesrewrTest(YmlTest):
    def __init__(self, name, inputfiles, rewriter, settings = dict()):
        super(PbesrewrTest, self).__init__(name, ymlfile('pbesrewr'), inputfiles, settings)
        self.set_command_line_options('t2', ['-p' + rewriter])

class CountStatesTest(YmlTest):
    # expected_result is the expected number of states
    def __init__(self, name, inputfiles, expected_result, lps2lts_options = [], settings = dict()):
        super(CountStatesTest, self).__init__(name, ymlfile('countstates'), inputfiles, settings)
        if lps2lts_options:
            self.set_command_line_options('t2', lps2lts_options)

class PbesstategraphTest(YmlTest):
    def __init__(self, name, inputfiles, command_line_options, settings = dict()):
        super(PbesstategraphTest, self).__init__(name, ymlfile('pbesstategraph'), inputfiles, settings)
        self.set_command_line_options('t2', command_line_options)

regression_tests = {
    'ticket_283'    : lambda name, settings: YmlTest(name, ymlfile('mcrl22lps'),         [abspath('tickets/283/1.mcrl2')], settings),
    'ticket_325'    : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/325/1.mcrl2')], settings),
    'ticket_904'    : lambda name, settings: YmlTest(name, ymlfile('mcrl22lps'),         [abspath('tickets/904/1.mcrl2')], settings),
    'ticket_952'    : lambda name, settings: YmlTest(name, ymlfile('txt2pbes'),          [abspath('tickets/952/1.txt')], settings),
    'ticket_1090'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1090'),       [abspath('tickets/1090/1.mcrl2'), abspath('tickets/1090/1.mcf')], settings),
    'ticket_1114a'  : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1114/1.mcrl2')], settings),
    'ticket_1114b'  : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1114/2.mcrl2')], settings),
    'ticket_1143'   : lambda name, settings: PbesrewrTest(name, [abspath('tickets/1143/1.txt')], 'quantifier-one-point'),
    'ticket_1144'   : lambda name, settings: YmlTest(name, ymlfile('lpsbisim2pbes'),     [abspath('tickets/1144/test1.txt'), abspath('tickets/1144/test2.txt')], settings),
    'ticket_1167'   : lambda name, settings: CountStatesTest(name, [mcrl2file('/examples/academic/abp/abp.mcrl2')], 74),
    'ticket_1206'   : lambda name, settings: YmlTest(name, ymlfile('lps2lts'),           [abspath('tickets/1206/1.mcrl2')], settings),
    'ticket_1218'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1218/1.mcrl2')], settings),
    'ticket_1234'   : lambda name, settings: YmlTest(name, ymlfile('lpsbinary'),         [mcrl2file('/examples/academic/cabp/cabp.mcrl2')], settings),
    'ticket_1241'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1241/1.mcrl2')], settings),
    'ticket_1249'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1249/1.mcrl2')], settings),
    'ticket_1297'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1297'),       [abspath('tickets/1297/1.mcrl2')], settings),
    'ticket_1301'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1301'),       [abspath('tickets/1301/1.mcrl2'), abspath('tickets/1301/1.mcf')], settings),
    'ticket_1311'   : lambda name, settings: PbesstategraphTest(name,                    [abspath('tickets/1311/1.txt')], ['-g'], settings),
    'ticket_1314'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1314'),       [abspath('tickets/1314/1.mcrl2'), abspath('tickets/1314/1.mcf')], settings),
    'ticket_1315'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1315/1.mcrl2')], settings),
    'ticket_1316'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1316/1.mcrl2')], settings),
    'ticket_1317'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1317/1.mcrl2')], settings),
    'ticket_1318'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1318/1.mcrl2')], settings),
    'ticket_1319'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1319/1.mcrl2')], settings),
    'ticket_1320'   : lambda name, settings: YmlTest(name, ymlfile('lps2pbes'),          [abspath('tickets/1320/1.mcrl2'), abspath('tickets/1320/1.mcf')], settings),
    'ticket_1321'   : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1321/1.mcrl2')], settings),
    'ticket_1322'   : lambda name, settings: YmlTest(name, ymlfile('pbesstategraph'),    [abspath('tickets/1322/1.txt')], settings),
    'ticket_1345'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1345'),       [abspath('tickets/1345/1.txt')], settings),
    'lpsconfcheck_1': lambda name, settings: LpsconfcheckCtauTest(name, [mcrl2file('/examples/academic/cabp/cabp.mcrl2')], 'T', (0, 18), settings),
    'lpsconfcheck_2': lambda name, settings: LpsconfcheckCtauTest(name, [mcrl2file('/examples/academic/trains/trains.mcrl2')], 'T', (0, 9), settings),
    }

pbessolve_tests = { 'pbessolve_{}'.format(filename[:-4]) : lambda name, settings: YmlTest(name, ymlfile('pbessolve'), [abspath('pbessolve/{}'.format(filename))]) for filename in sorted(os.listdir(abspath('pbessolve'))) }

slow_regression_tests = {
    'ticket_1093'    : lambda name, settings: YmlTest(name, ymlfile('alphabet'),          [abspath('tickets/1093/1.mcrl2')], settings.update({ 'timeout': 300, 'memlimit': 500000000 })),
    'lpsconfcheck_3' : lambda name, settings: LpsconfcheckCtauTest(name, [mcrl2file('/examples/industrial/chatbox/chatbox.mcrl2')], 'x', (40, 72), settings.update({ 'timeout': 300, 'memlimit': 500000000 })),
    }

if __name__ == '__main__':
    settings = {'toolpath': MCRL2_INSTALL_DIR, 'verbose': True, 'cleanup_files': True}
    testdir = 'output'
    if not os.path.exists(testdir):
        os.mkdir(testdir)
    os.chdir(testdir)
    LpsconfcheckTest('lpsconfcheck_1', [MCRL2_ROOT + '/examples/academic/cabp/cabp.mcrl2'], 'T', (0, 10), settings = settings).execute_in_sandbox()
    LpsconfcheckCtauTest('lpsconfcheck_2', [MCRL2_ROOT + '/examples/academic/cabp/cabp.mcrl2'], 'T', (0, 18), settings = settings).execute_in_sandbox()
    CountStatesTest('countstates_abp', [MCRL2_ROOT + '/examples/academic/abp/abp.mcrl2'], 74, settings = settings).execute_in_sandbox()
