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
        self.tests = [('regression_pbessolve_{}'.format(file[:-4]), run_yml_test, [ymlfile('pbessolve'), [abspath('pbessolve/{}'.format(file))]]) for file in pbessolve_files] + \
                     [
                       ('regression_ticket_283' , run_yml_test,         [ ymlfile('mcrl22lps'),         [abspath('tickets/283/1.mcrl2')]                                       ] ),
                       ('regression_ticket_325' , run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/325/1.mcrl2')]                                       ] ),
                       ('regression_ticket_1127', run_yml_test,         [ ymlfile('mcrl22lps'),         [abspath('tickets/1127/1.mcrl2')]                                      ] ),
                       ('regression_ticket_1143', run_yml_test,         [ ymlfile('pbesrewr-onepoint'), [abspath('tickets/1143/1.txt')]                                        ] ),
                       ('regression_ticket_1144', run_yml_test,         [ ymlfile('lpsbisim2pbes'),     [abspath('tickets/1144/test1.txt'), abspath('tickets/1144/test2.txt')] ] ),
                       ('regression_ticket_1167', run_countstates_test, [ [mcrl2file('/examples/academic/abp/abp.mcrl2')], 74                                                  ] ),
                       ('regression_ticket_1234', run_yml_test,         [ ymlfile('lpsbinary'),         [mcrl2file('/examples/academic/cabp/cabp.mcrl2')]                      ] ),
                       ('regression_ticket_1241', run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/1241/1.mcrl2')]                                      ] ),
                       ('regression_ticket_1249', run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/1249/1.mcrl2')]                                      ] ),
                       ('regression_ticket_1315', run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/1315/1.mcrl2')]                                      ] ),
                       ('regression_ticket_1316', run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/1316/1.mcrl2')]                                      ] ),
                       ('regression_ticket_1317', run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/1317/1.mcrl2')]                                      ] ),
                       ('regression_ticket_1318', run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/1318/1.mcrl2')]                                      ] ),
                       ('regression_ticket_1319', run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/1319/1.mcrl2')]                                      ] ),
                       ('regression_ticket_1321', run_yml_test,         [ ymlfile('alphabet'),          [abspath('tickets/1321/1.mcrl2')]                                      ] ),
                     ]
                     #(run_yml_test, 'lpsconfcheck_triangular_cabp', 'lpsconfcheck_triangular', [self._source_path + '/examples/academic/cabp/cabp.mcrl2']),
                     #(run_yml_test, 'lpsconfcheck_triangular_trains', 'lpsconfcheck_triangular', [self._source_path + '/examples/academic/trains/trains.mcrl2']),
                     #(run_yml_test, 'lpsconfcheck_triangular_no_sums_trains', 'lpsconfcheck_triangular_no_sums', [self._source_path + '/examples/academic/trains/trains.mcrl2']),
                     # + \
                     #generate_test('lpsconfcheck_triangular_cabp', 'run_lpsconfcheck_test', [self._source_path + '/examples/academic/cabp/cabp.mcrl2'], ['-xT'], (0, 10)) + \
                     #[]
                     # TODO: increase timeout value
                     #('lpsconfcheck_triangular_no_sums_chatbox', 'lpsconfcheck_triangular_no_sums', [self._source_path + '/examples/industrial/chatbox/chatbox.mcrl2']),
                     #('ticket_1247', 'alphabet', ['tickets/1247/1.mcrl2']),
                     #('ticket_352',  'alphabet', ['tickets/352/1.mcrl2']),
                     #('ticket_397',  'mcrl22lps', ['tickets/397/1.mcrl2']),
                     #('ticket_1090', 'lps2pbes', ['tickets/1090/form.mcf', 'tickets/1090/spec.mcrl2']),
                     # TODO: increase timeout value
                     #('ticket_1093', 'alphabet', ['tickets/1093/1.mcrl2']),
                     #('ticket_1122', 'txt2lps', ['tickets/1122/1.mcrl2']),

        self.settings = {'toolpath': self._tool_path,
                         'verbose': self._args.verbose,
                         'cleanup_files': True}

    def ymlfile(self, name):
        return '{}/tests/specifications/{}.yml'.format(self._source_path, name)

    def mcrl2file(self, file):
        return self._source_path + file

    def abspath(self, file):
        return os.path.join(self.test_path, file)

    def _get_commandline_parser(self):
        parser = super(TestRunner, self)._get_commandline_parser()
        parser.add_argument('--verbose', dest='verbose', action='store_true', help='Display additional progress messages.')
        return parser

    def names(self):
        for test in self.tests:
            yield test[0]

    def run(self, testnum):
        if testnum < len(self.tests):
            test = self.tests[testnum]
            name, testfunc, arguments = test[:3]

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
            os.rmdir(name)
        else:
            raise RuntimeError('Invalid test number')

if __name__ == "__main__":
    sys.exit(TestRunner().main())
    #TestRunner().run(5)
