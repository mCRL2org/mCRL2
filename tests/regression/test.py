import os
import sys
import yaml
import shutil # TODO: remove this import
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
import testrunner
from testing import run_yml_test

class TestRunner(testrunner.TestRunner):
    def __init__(self):
        super(TestRunner, self).__init__()
        self.test_path = os.path.join(os.getcwd(), os.path.dirname(__file__))
        self.tests = sorted([('pbessolve_{}'.format(file[:-4]), 'pbessolve', ['pbessolve/{}'.format(file)])
                      for file in os.listdir(os.path.join(self.test_path, 'pbessolve'))
                      if file.endswith('.txt')]) + \
                     [('ticket_283',  'mcrl22lps', ['tickets/283/1.mcrl2']),
                      ('ticket_325',  'alphabet', ['tickets/325/1.mcrl2']),
                     #('ticket_352',  'alphabet', ['tickets/352/1.mcrl2']),
                     #('ticket_397',  'mcrl22lps', ['tickets/397/1.mcrl2']),
                     #('ticket_1090', 'lps2pbes', ['tickets/1090/form.mcf', 'tickets/1090/spec.mcrl2']),

                     # TODO: increase timeout value
                     #('ticket_1093', 'alphabet', ['tickets/1093/1.mcrl2']),

                     #('ticket_1122', 'txt2lps', ['tickets/1122/1.mcrl2']),
                      ('ticket_1127', 'mcrl22lps', ['tickets/1127/1.mcrl2']),
                      ('ticket_1143', 'pbesrewr-onepoint', ['tickets/1143/1.txt']),
                      ('ticket_1144', 'lpsbisim2pbes', ['tickets/1144/test1.txt', 'tickets/1144/test2.txt']),
                      ('ticket_1167', 'countstates', [self._source_path + '/examples/academic/abp/abp.mcrl2'],
                         {'nodes': {'l5': {'value': 74}}}),
                      ('ticket_1234', 'lpsbinary', [self._source_path + '/examples/academic/cabp/cabp.mcrl2']),
                      ('ticket_1241', 'alphabet', ['tickets/1241/1.mcrl2']),
                     #('ticket_1247', 'alphabet', ['tickets/1247/1.mcrl2']),
                      ('ticket_1249', 'alphabet', ['tickets/1249/1.mcrl2']),
                      ('ticket_1315', 'alphabet', ['tickets/1315/1.mcrl2']),
                      ('ticket_1316', 'alphabet', ['tickets/1316/1.mcrl2']),
                      ('ticket_1317', 'alphabet', ['tickets/1317/1.mcrl2']),
                      ('ticket_1318', 'alphabet', ['tickets/1318/1.mcrl2']),
                      ('ticket_1319', 'alphabet', ['tickets/1319/1.mcrl2']),
                      ('ticket_1321', 'alphabet', ['tickets/1321/1.mcrl2'])]
        self.settings = {'toolpath': self._tool_path,
                         'verbose': True,
                         'cleanup_files': True}

    def name(self, testnum):
        if testnum < len(self.tests):
            return self.tests[testnum][0]

    def run(self, testnum):
        if testnum < len(self.tests):
            test = self.tests[testnum]
            name, specification, inputfiles = test[:3]

            # The specification identifies a YAML file in the specifications directory
            specification = '{}/tests/specifications/{}.yml'.format(self._source_path, specification)

            # Create a settings dict based on self.settings and, optionally, the test-specific
            # settings
            settings = {}
            settings.update(self.settings)
            if len(test) > 3:
                settings.update(test[3])

            # Interpret relative input file paths as being relative to the regression test
            # directory.
            inputfiles = [file if os.path.isabs(file) else os.path.join(self.test_path, file)
                          for file in inputfiles]

            # Create sandbox directory for the test to run in
            os.mkdir(name)
            os.chdir(name)

            # Run the test
            run_yml_test(name, specification, inputfiles, settings)

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
