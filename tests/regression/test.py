#~ Copyright 2015 Sjoerd Cranen.
#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
from testing import run_yml_test
import testrunner

class TestRunner(testrunner.TestRunner):
    def __init__(self):
        super(TestRunner, self).__init__()
        self.test_path = os.path.join(os.getcwd(), os.path.dirname(__file__))
        self.tests = sorted([(run_yml_test, 'regression_pbessolve_{}'.format(file[:-4]), 'pbessolve', ['pbessolve/{}'.format(file)])
                      for file in os.listdir(os.path.join(self.test_path, 'pbessolve'))
                      if file.endswith('.txt')]) + \
                     [
                       (run_yml_test, 'ticket_283',  'mcrl22lps', ['tickets/283/1.mcrl2']),
                       (run_yml_test, 'ticket_325',  'alphabet', ['tickets/325/1.mcrl2']),
                       (run_yml_test, 'ticket_1127', 'mcrl22lps', ['tickets/1127/1.mcrl2']),
                       (run_yml_test, 'ticket_1143', 'pbesrewr-onepoint', ['tickets/1143/1.txt']),
                       (run_yml_test, 'ticket_1144', 'lpsbisim2pbes', ['tickets/1144/test1.txt', 'tickets/1144/test2.txt']),
                       (run_yml_test, 'ticket_1167', 'countstates', [self._source_path + '/examples/academic/abp/abp.mcrl2'], {'nodes': {'l5': {'value': 74}}}),
                       (run_yml_test, 'ticket_1234', 'lpsbinary', [self._source_path + '/examples/academic/cabp/cabp.mcrl2']),
                       (run_yml_test, 'ticket_1241', 'alphabet', ['tickets/1241/1.mcrl2']),
                       (run_yml_test, 'ticket_1249', 'alphabet', ['tickets/1249/1.mcrl2']),
                       (run_yml_test, 'ticket_1315', 'alphabet', ['tickets/1315/1.mcrl2']),
                       (run_yml_test, 'ticket_1316', 'alphabet', ['tickets/1316/1.mcrl2']),
                       (run_yml_test, 'ticket_1317', 'alphabet', ['tickets/1317/1.mcrl2']),
                       (run_yml_test, 'ticket_1318', 'alphabet', ['tickets/1318/1.mcrl2']),
                       (run_yml_test, 'ticket_1319', 'alphabet', ['tickets/1319/1.mcrl2']),
                       (run_yml_test, 'ticket_1321', 'alphabet', ['tickets/1321/1.mcrl2']),
                       (run_yml_test, 'lpsconfcheck_triangular_cabp', 'lpsconfcheck_triangular', [self._source_path + '/examples/academic/cabp/cabp.mcrl2']),
                       (run_yml_test, 'lpsconfcheck_triangular_trains', 'lpsconfcheck_triangular', [self._source_path + '/examples/academic/trains/trains.mcrl2']),
                       (run_yml_test, 'lpsconfcheck_triangular_no_sums_trains', 'lpsconfcheck_triangular_no_sums', [self._source_path + '/examples/academic/trains/trains.mcrl2']),
                     ]
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

    def _get_commandline_parser(self):
        parser = super(TestRunner, self)._get_commandline_parser()
        parser.add_argument('--verbose', dest='verbose', action='store_true', help='Display additional progress messages.')
        return parser

    def names(self):
        for test in self.tests:
            yield test[1]

    def run(self, testnum):
        if testnum < len(self.tests):
            test = self.tests[testnum]
            testfunc, name, specification, inputfiles = test[:4]

            # The specification identifies a YAML file in the specifications directory
            specification = '{}/tests/specifications/{}.yml'.format(self._source_path, specification)

            # Create a settings dict based on self.settings and, optionally, the test-specific
            # settings
            settings = {}
            settings.update(self.settings)
            if len(test) > 4:
                settings.update(test[4])

            # Interpret relative input file paths as being relative to the regression test
            # directory.
            inputfiles = [file if os.path.isabs(file) else os.path.join(self.test_path, file)
                          for file in inputfiles]

            # Create sandbox directory for the test to run in
            os.mkdir(name)
            os.chdir(name)

            # Run the test
            testfunc(name, specification, inputfiles, settings)

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
    #TestRunner().run(25)
