#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import re
import sys
import traceback

# Makes sure that the script can find the modules when ran directly.
sys.path.append(os.path.join(os.path.dirname(__file__),'../../'))

from tests.utility.testing import YmlTest

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'install', 'bin')
PWD = os.path.abspath(os.path.dirname(__file__))

def abspath(file):
    return os.path.abspath(os.path.join(PWD, file))

def mcrl2file(file):
    return os.path.abspath(os.path.join(MCRL2_ROOT, file))

def ymlfile(file):
    return '{}/tests/specifications/{}.yml'.format(MCRL2_ROOT, file)

class LpsconfcheckTest(YmlTest):
    def __init__(self, name, inputfiles, confluence_type, expected_result, settings):
        assert confluence_type in 'cdCTZ'
        super(LpsconfcheckTest, self).__init__(name, ymlfile('lpsconfcheck'), inputfiles, settings)
        self.add_command_line_options('t2', ['-x' + confluence_type])
        self.settings.update({'result': "result = t5.value['result'] and t2.value['confluent-tau-summand-count'] == {}  and t2.value['tau-summand-count'] == {}".format(expected_result[0], expected_result[1])})

class LpsconfcheckCtauTest(YmlTest):
    def __init__(self, name, inputfiles, confluence_type, expected_result, settings):
        assert confluence_type in 'cdCTZ'
        super(LpsconfcheckCtauTest, self).__init__(name, ymlfile('lpsconfcheck_ctau'), inputfiles, settings)
        self.add_command_line_options('t3', ['-x' + confluence_type])
        self.settings.update({'result': "result = t6.value['result'] and t3.value['confluent-tau-summand-count'] == {}  and t3.value['tau-summand-count'] == {}".format(expected_result[0], expected_result[1])})

class PbesrewrTest(YmlTest):
    def __init__(self, name, inputfiles, rewriter, settings):
        super(PbesrewrTest, self).__init__(name, ymlfile('pbesrewr'), inputfiles, settings)
        self.add_command_line_options('t2', ['-p' + rewriter])

class CountStatesTest(YmlTest):
    # expected_result is the expected number of states
    def __init__(self, name, inputfiles, expected_result, lps2lts_options, settings):
        super(CountStatesTest, self).__init__(name, ymlfile('countstates'), inputfiles, settings)
        if lps2lts_options:
            self.add_command_line_options('t2', lps2lts_options)

class PbesstategraphTest(YmlTest):
    def __init__(self, name, inputfiles, command_line_options, settings):
        super(PbesstategraphTest, self).__init__(name, ymlfile('pbesstategraph'), inputfiles, settings)
        self.add_command_line_options('t2', command_line_options)

regression_tests = {
    'ticket-283'    : lambda name, settings: YmlTest(name, ymlfile('mcrl22lps'),         [abspath('tickets/283/1.mcrl2')], settings),
    'ticket-325'    : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/325/1.mcrl2')], settings),
    'ticket-904'    : lambda name, settings: YmlTest(name, ymlfile('mcrl22lps'),         [abspath('tickets/904/1.mcrl2')], settings),
    'ticket-952'    : lambda name, settings: YmlTest(name, ymlfile('txt2pbes'),          [abspath('tickets/952/1.txt')], settings),
    'ticket-1090'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1090'),       [abspath('tickets/1090/1.mcrl2'), abspath('tickets/1090/1.mcf')], settings),
    'ticket-1114a'  : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1114/1.mcrl2')], settings),
    'ticket-1114b'  : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1114/2.mcrl2')], settings),
    'ticket-1143'   : lambda name, settings: PbesrewrTest(name, [abspath('tickets/1143/1.txt')], 'quantifier-one-point', settings),
    'ticket-1144'   : lambda name, settings: YmlTest(name, ymlfile('lpsbisim2pbes'),     [abspath('tickets/1144/test1.txt'), abspath('tickets/1144/test2.txt')], settings),
    'ticket-1167'   : lambda name, settings: CountStatesTest(name, [mcrl2file('examples/academic/abp/abp.mcrl2')], 74, [], settings = settings),
    'ticket-1206'   : lambda name, settings: YmlTest(name, ymlfile('lps2lts'),           [abspath('tickets/1206/1.mcrl2')], settings),
    'ticket-1218'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1218/1.mcrl2')], settings),
    'ticket-1234'   : lambda name, settings: YmlTest(name, ymlfile('lpsbinary'),         [mcrl2file('examples/academic/cabp/cabp.mcrl2')], settings),
    'ticket-1241'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1241/1.mcrl2')], settings),
    'ticket-1249'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1249/1.mcrl2')], settings),
    'ticket-1297'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1297'),       [abspath('tickets/1297/1.mcrl2')], settings),
    'ticket-1301'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1301'),       [abspath('tickets/1301/1.mcrl2'), abspath('tickets/1301/1.mcf')], settings),
    'ticket-1311'   : lambda name, settings: PbesstategraphTest(name,                    [abspath('tickets/1311/1.txt')], ['-g'], settings),
    'ticket-1315'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1315/1.mcrl2')], settings),
    'ticket-1316'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1316/1.mcrl2')], settings),
    'ticket-1317'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1317/1.mcrl2')], settings),
    'ticket-1318'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1318/1.mcrl2')], settings),
    'ticket-1319'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1319/1.mcrl2')], settings),
    'ticket-1320'   : lambda name, settings: YmlTest(name, ymlfile('lps2pbes'),          [abspath('tickets/1320/1.mcrl2'), abspath('tickets/1320/1.mcf')], settings),
    'ticket-1321'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1321/1.mcrl2')], settings),
    'ticket-1322'   : lambda name, settings: YmlTest(name, ymlfile('pbesstategraph'),    [abspath('tickets/1322/1.txt')], settings),
    'ticket-1345'   : lambda name, settings: YmlTest(name, ymlfile('ticket_1345'),       [abspath('tickets/1345/1.txt')], settings),
    'ticket-1413'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1413/1.mcrl2')], settings),
    'ticket-1483'   : lambda name, settings: YmlTest(name, ymlfile('pbesparseprint'),    [abspath('tickets/1483/1.txt')], settings),
    'ticket-1519'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),   [abspath('tickets/1519/1.mcrl2')], settings),
    'lpsconfcheck1' : lambda name, settings: LpsconfcheckCtauTest(name, [mcrl2file('examples/academic/cabp/cabp.mcrl2')], 'T', (0, 18), settings),
    'onepoint1'     : lambda name, settings: PbesrewrTest(name, [abspath('onepoint/1.txt')], 'quantifier-one-point', settings),
    'pbessolve'     : lambda name, settings: YmlTest(name, ymlfile('pbessolve'), [abspath('pbessolve-structure-graph/1.txt')], settings),
    #'pfnf1'         : lambda name, settings: PbesrewrTest(name, [abspath('pfnf/1.txt')], 'pfnf', settings),
    }

pbessolve_tests       = { 'pbessolve-{}'.format(filename[:-4]) : lambda name, settings: YmlTest(name, ymlfile('pbespgsolve'), [abspath('pbessolve/{}'.format(filename))], settings) for filename in sorted(os.listdir(abspath('pbessolve'))) }
alphabet_reduce_tests = { 'alphabet-reduce-{}'.format(filename[:-6]) : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'), [abspath('alphabet-reduce/{}'.format(filename))], settings) for filename in sorted(os.listdir(abspath('alphabet-reduce'))) }

def update_settings(settings, u):
    settings.update(u)
    return settings

slow_regression_tests = {
    'ticket-1093'   : lambda name, settings: YmlTest(name, ymlfile('alphabet-reduce'),          [abspath('tickets/1093/1.mcrl2')], update_settings(settings, { 'timeout': 300, 'memlimit': 500000000 })),
    'lpsconfcheck2' : lambda name, settings: LpsconfcheckCtauTest(name, [mcrl2file('examples/academic/trains/trains.mcrl2')], 'T', (0, 9), settings),
    'lpsconfcheck3' : lambda name, settings: LpsconfcheckCtauTest(name, [mcrl2file('examples/industrial/chatbox/chatbox.mcrl2')], 'Z', (40, 72), update_settings(settings, { 'timeout': 300, 'memlimit': 500000000 })),
    }

# Return all tests that match with pattern. In case of an exact match, only this exact match is returned.
def matching_tests(tests, pattern):
    matches = [name for name in sorted(tests) if re.search(pattern, name)]
    if pattern in matches:
        return [pattern]
    return matches

def test1():
    settings = {'toolpath': MCRL2_INSTALL_DIR, 'verbose': True, 'cleanup_files': True}
    testdir = 'output'
    if not os.path.exists(testdir):
        os.mkdir(testdir)
    os.chdir(testdir)
    LpsconfcheckTest('lpsconfcheck_1',     [mcrl2file('examples/academic/cabp/cabp.mcrl2')], 'T', (0, 10), settings = settings).execute_in_sandbox()
    LpsconfcheckCtauTest('lpsconfcheck_2', [mcrl2file('examples/academic/cabp/cabp.mcrl2')], 'T', (0, 18), settings = settings).execute_in_sandbox()
    CountStatesTest('countstates_abp',     [mcrl2file('examples/academic/abp/abp.mcrl2')], 74, [], settings = settings).execute_in_sandbox()

def print_names(tests):
    for name in sorted(tests):
        print(name)

# Command line interface to run tests.
def main(tests):
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument('-t', '--toolpath', dest='toolpath', help='The path where the mCRL2 tools are installed')
    cmdline_parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Display additional progress messages.')
    cmdline_parser.add_argument('-k', '--keep-files', dest='keep_files', action='store_true', help='Keep the files produced by the test')
    cmdline_parser.add_argument('-n', '--names', dest='names', action='store_true', help='Print the names of the available tests')
    cmdline_parser.add_argument('-p', '--pattern', dest='pattern', metavar='P', default='.', action='store', help='Run the tests that match with pattern P')
    cmdline_parser.add_argument('-o', '--output', dest='output', metavar='o', action='store', help='Run the tests in the given directory')
    args = cmdline_parser.parse_args()
    if args.names:
        print_names(tests)
        return
    toolpath = args.toolpath
    if not toolpath:
        toolpath = MCRL2_INSTALL_DIR
    settings = {'toolpath': toolpath, 'verbose': args.verbose, 'cleanup_files': not args.keep_files, 'allow-non-zero-return-values': True}

    if args.output:
        if not os.path.exists(args.output):
            os.mkdir(args.output)
        os.chdir(args.output)

    test_failed = False
    for name in matching_tests(tests, args.pattern):
        try:
            test = tests[name](name, settings)
            test.execute_in_sandbox()
        except Exception as e:
            print('An exception occurred:', e.__class__, e)
            traceback.print_exc()
            test_failed = True

    if (test_failed):
        sys.exit(-1)

if __name__ == '__main__':
    tests = regression_tests
    tests.update(pbessolve_tests)
    tests.update(alphabet_reduce_tests)
    main(tests)
    #main(slow_regression_tests)
