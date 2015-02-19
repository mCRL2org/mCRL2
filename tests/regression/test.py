#!/usr/bin/env python

#~ Copyright 2015 Sjoerd Cranen.
#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
from testing import run_yml_test
from regression_testing import *
from testcommand import YmlTest
import testrunner

class TestRunner(testrunner.TestRunner):
    def __init__(self):
        super(TestRunner, self).__init__()
        ymlfile = self.ymlfile
        mcrl2file = self.mcrl2file
        abspath = self.abspath
        self.test_path = os.path.join(os.getcwd(), os.path.dirname(__file__))
        pbessolve_files = sorted(os.listdir(os.path.join(self.test_path, 'pbessolve')))
        self.tests = [
                       YmlTest('ticket_283',  ymlfile('mcrl22lps'),         [abspath('tickets/283/1.mcrl2')]),
                       YmlTest('ticket_325' , ymlfile('alphabet'),          [abspath('tickets/325/1.mcrl2')]),
                       YmlTest('ticket_1090', ymlfile('ticket_1090'),       [abspath('tickets/1090/1.mcrl2'), abspath('tickets/1090/1.mcf')]),
                       YmlTest('ticket_1093', ymlfile('alphabet'),          [abspath('tickets/1093/1.mcrl2')], { 'timeout': 300, 'memlimit': 500000000 }),
                       YmlTest('ticket_1122', ymlfile('ticket_1122'),       [abspath('tickets/1122/1.mcrl2')]),
                       YmlTest('ticket_1127', ymlfile('mcrl22lps'),         [abspath('tickets/1127/1.mcrl2')]),
                       YmlTest('ticket_1143', ymlfile('pbesrewr-onepoint'), [abspath('tickets/1143/1.txt')]),
                       YmlTest('ticket_1144', ymlfile('lpsbisim2pbes'),     [abspath('tickets/1144/test1.txt'), abspath('tickets/1144/test2.txt')]),
                       CountStatesTest('ticket_1167', [mcrl2file('/examples/academic/abp/abp.mcrl2')], 74),
                       YmlTest('ticket_1218', ymlfile('alphabet'),          [abspath('tickets/1218/1.mcrl2')]),
                       YmlTest('ticket_1234', ymlfile('lpsbinary'),         [mcrl2file('/examples/academic/cabp/cabp.mcrl2')]),
                       YmlTest('ticket_1241', ymlfile('alphabet'),          [abspath('tickets/1241/1.mcrl2')]),
                       YmlTest('ticket_1249', ymlfile('alphabet'),          [abspath('tickets/1249/1.mcrl2')]),
                       YmlTest('ticket_1315', ymlfile('alphabet'),          [abspath('tickets/1315/1.mcrl2')]),
                       YmlTest('ticket_1316', ymlfile('alphabet'),          [abspath('tickets/1316/1.mcrl2')]),
                       YmlTest('ticket_1317', ymlfile('alphabet'),          [abspath('tickets/1317/1.mcrl2')]),
                       YmlTest('ticket_1318', ymlfile('alphabet'),          [abspath('tickets/1318/1.mcrl2')]),
                       YmlTest('ticket_1319', ymlfile('alphabet'),          [abspath('tickets/1319/1.mcrl2')]),
                       LpsconfcheckCtauTest('lpsconfcheck_1', [mcrl2file('/examples/academic/cabp/cabp.mcrl2')], ['-t8', '-xT'], (0, 18)),
                       LpsconfcheckCtauTest('lpsconfcheck_2', [mcrl2file('/examples/academic/trains/trains.mcrl2')], ['-t8', '-xT'], (0, 9)),
                       #LpsconfcheckCtauTest('lpsconfcheck_3', [mcrl2file('/examples/industrial/chatbox/chatbox.mcrl2')], ['-t8', '-xT', '-u'], (40, 72), { 'timeout': 300, 'memlimit': 500000000 } ),
                     ] + \
                     [
                       YmlTest('regression_pbessolve_{}'.format(file[:-4]), ymlfile('pbessolve'), [abspath('pbessolve/{}'.format(file))]) for file in pbessolve_files
                     ]
        for test in self.tests:
            test.name = 'regression_' + test.name

        self.settings = {'toolpath': self._tool_path,
                         'verbose': self._args.verbose,
                         'cleanup_files': not self._args.keep_files}

    def main(self):
        if self._args.print_names:
            self.print_names()
        if self._args.command is not None:
            try:
                test = self.tests[self._args.command]
                test.print_commands(os.path.join(os.getcwd(), test.name))
            except Exception as e:
                sys.exit(str(e))
        super(TestRunner, self).main()

    def ymlfile(self, name):
        return '{}/tests/specifications/{}.yml'.format(self._source_path, name)

    def mcrl2file(self, file):
        return self._source_path + file

    def abspath(self, file):
        return os.path.join(self.test_path, file)

    def _get_commandline_parser(self):
        parser = super(TestRunner, self)._get_commandline_parser()
        parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Display additional progress messages.')
        parser.add_argument('-k', '--keep-files', dest='keep_files', action='store_true', help='Keep the files produced by the test')
        parser.add_argument('-p', '--print-names', dest='print_names', action='store_true', help='Print the names and the numbers of the tests')
        parser.add_argument('-c', '--print-commands', dest='command', metavar='N', type=int, action='store', help='Print the commands of test N, or exit with return value 1 if N is too large.')
        return parser

    def names(self):
        for test in self.tests:
            yield test.name

    # displays names and numbers of the tests
    def print_names(self):
        for i, test in enumerate(self.tests):
            print '{} {}'.format(i, test.name)

    def run(self, testnum):
        if testnum < len(self.tests):
            test = self.tests[testnum]
            test.settings.update(self.settings)

            # Create sandbox directory for the test to run in
            os.mkdir(test.name)
            os.chdir(test.name)

            # Run the test
            test.execute()

            # If the test did not throw an exception, we assume it was successful and remove
            # the directory we created. We assume that run_yml_test has deleted any files it
            # might have created in the test directory, so we only have to remove an empty
            # directory here.
            os.chdir('..')
            if not self._args.keep_files:
                os.rmdir(test.name)
        else:
            raise RuntimeError('Invalid test number')

if __name__ == "__main__":
    sys.exit(TestRunner().main())
    #TestRunner().run(7)
