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
                       ('ticket_283' , run_yml_test,             [ ymlfile('mcrl22lps'),         [abspath('tickets/283/1.mcrl2')]                                       ] ),
                       ('ticket_325' , run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/325/1.mcrl2')]                                       ] ),
                       ('ticket_1090', run_yml_test,             [ ymlfile('ticket_1090'),       [abspath('tickets/1090/1.mcrl2'), abspath('tickets/1090/1.mcf')]       ] ),
                       ('ticket_1093', run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/1093/1.mcrl2')]                                      ], { 'timeout': 300, 'memlimit': 500000000 }),
                       ('ticket_1122', run_yml_test,             [ ymlfile('ticket_1122'),       [abspath('tickets/1122/1.mcrl2')]                                      ] ),
                       ('ticket_1127', run_yml_test,             [ ymlfile('mcrl22lps'),         [abspath('tickets/1127/1.mcrl2')]                                      ] ),
                       ('ticket_1143', run_yml_test,             [ ymlfile('pbesrewr-onepoint'), [abspath('tickets/1143/1.txt')]                                        ] ),
                       ('ticket_1144', run_yml_test,             [ ymlfile('lpsbisim2pbes'),     [abspath('tickets/1144/test1.txt'), abspath('tickets/1144/test2.txt')] ] ),
                       ('ticket_1167', run_countstates_test,     [ [mcrl2file('/examples/academic/abp/abp.mcrl2')], 74                                                  ] ),
                       ('ticket_1234', run_yml_test,             [ ymlfile('lpsbinary'),         [mcrl2file('/examples/academic/cabp/cabp.mcrl2')]                      ] ),
                       ('ticket_1241', run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/1241/1.mcrl2')]                                      ] ),
                       ('ticket_1249', run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/1249/1.mcrl2')]                                      ] ),
                       ('ticket_1315', run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/1315/1.mcrl2')]                                      ] ),
                       ('ticket_1316', run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/1316/1.mcrl2')]                                      ] ),
                       ('ticket_1317', run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/1317/1.mcrl2')]                                      ] ),
                       ('ticket_1318', run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/1318/1.mcrl2')]                                      ] ),
                       ('ticket_1319', run_yml_test,             [ ymlfile('alphabet'),          [abspath('tickets/1319/1.mcrl2')]                                      ] ),
                       ('lpsconfcheck_1', run_lpsconfcheck_ctau_test, [ [mcrl2file('/examples/academic/cabp/cabp.mcrl2')], ['-t8', '-xT'], (0, 18)                      ] ),
                       ('lpsconfcheck_2', run_lpsconfcheck_ctau_test, [ [mcrl2file('/examples/academic/trains/trains.mcrl2')], ['-t8', '-xT'], (0, 9)                   ] ),
                       #('lpsconfcheck_3', run_lpsconfcheck_ctau_test, [ [mcrl2file('/examples/industrial/chatbox/chatbox.mcrl2')], ['-t8', '-xT', '-u'], (40, 72)  ], { 'timeout': 300, 'memlimit': 500000000 } ),
                     ] + \
                     [
                       ('regression_pbessolve_{}'.format(file[:-4]), run_yml_test, [ymlfile('pbessolve'), [abspath('pbessolve/{}'.format(file))]]) for file in pbessolve_files
                     ]

        self.settings = {'toolpath': self._tool_path,
                         'verbose': self._args.verbose,
                         'cleanup_files': not self._args.keep_files}

    def main(self):
        if self._args.print_names:
            self.print_names()
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
        return parser

    def names(self):
        for test in self.tests:
            yield test[0]

    # displays names and numbers of the tests
    def print_names(self):
        for i, name in enumerate(self.names()):
            print '{} {}'.format(i, name)

    def run(self, testnum):
        if testnum < len(self.tests):
            test = self.tests[testnum]
            name, testfunc, arguments = test[:3]
            name = 'regression_{}'.format(name)

            # Create a settings dict based on self.settings and, optionally, the test-specific
            # settings
            settings = {}
            settings.update(self.settings)
            if len(test) > 3:
                settings.update(test[3])

            arguments.append(settings)

            # Create sandbox directory for the test to run in
            os.mkdir(name)
            os.chdir(name)

            # Run the test
            testfunc(name, *arguments)

            # If the test did not throw an exception, we assume it was successful and remove
            # the directory we created. We assume that run_yml_test has deleted any files it
            # might have created in the test directory, so we only have to remove an empty
            # directory here.
            os.chdir('..')
            if not self._args.keep_files:
                os.rmdir(name)
        else:
            raise RuntimeError('Invalid test number')

if __name__ == "__main__":
    sys.exit(TestRunner().main())
    #TestRunner().run(7)
