import glob
import os
import sys
import yaml
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
import testrunner
from testing import run_test

MCRL2_ROOT = os.path.join(os.path.dirname(__file__), '..', '..')

class TestRunner(testrunner.TestRunner):
    def __init__(self):
        self.tests = []

        #self.addtest('ticket_1167', '../specifications/countstates.yml', [MCRL2_ROOT + '/examples/academic/abp/abp.mcrl2'], 'nodes:\n  l4.txt:\n    value: 20')

        # add pbessolve tests
        index = 0
        for filename in sorted(glob.glob(os.path.join(os.path.dirname(__file__), 'pbessolve', '*.txt'))):
            self.addtest('pbessolve{0}'.format(index), '../specifications/pbessolve.yml', [filename])
            index = index + 1

        # add trac tickets tests
        self.addtest('ticket_283',  '../specifications/mcrl22lps.yml', ['tickets/283/1.mcrl2'])
        self.addtest('ticket_325',  '../specifications/alphabet.yml', ['tickets/325/1.mcrl2'])
        #self.addtest('ticket_352',  '../specifications/alphabet.yml', ['tickets/352/1.mcrl2'])
        #self.addtest('ticket_397',  '../specifications/mcrl22lps.yml', ['tickets/397/1.mcrl2'])
        #self.addtest('ticket_1090', '../specifications/lps2pbes.yml', ['tickets/1090/form.mcf', 'tickets/1090/spec.mcrl2'])
        self.addtest('ticket_1093', '../specifications/alphabet.yml', ['tickets/1093/1.mcrl2'])
        #self.addtest('ticket_1122', '../specifications/txt2lps.yml', ['tickets/1122/1.mcrl2'])
        self.addtest('ticket_1127', '../specifications/mcrl22lps.yml', ['tickets/1127/1.mcrl2'])
        self.addtest('ticket_1143', '../specifications/pbesrewr-onepoint.yml', ['tickets/1143/1.txt'])
        self.addtest('ticket_1144', '../specifications/lpsbisim2pbes.yml', ['tickets/1144/test1.txt', 'tickets/1144/test2.txt'])
        #self.addtest('ticket_1167', '../specifications/countstates.yml', [MCRL2_ROOT + '/examples/academic/abp/abp.mcrl2'])
        self.addtest('ticket_1234', '../specifications/lpsbinary.yml', [MCRL2_ROOT + '/examples/academic/cabp/cabp.mcrl2'])
        self.addtest('ticket_1241', '../specifications/alphabet.yml', ['tickets/1241/1.mcrl2'])
        #self.addtest('ticket_1247', '../specifications/alphabet.yml', ['tickets/1247/1.mcrl2'])
        self.addtest('ticket_1249', '../specifications/alphabet.yml', ['tickets/1249/1.mcrl2'])
        self.addtest('ticket_1315', '../specifications/alphabet.yml', ['tickets/1315/1.mcrl2'])
        self.addtest('ticket_1316', '../specifications/alphabet.yml', ['tickets/1316/1.mcrl2'])
        self.addtest('ticket_1317', '../specifications/alphabet.yml', ['tickets/1317/1.mcrl2'])
        self.addtest('ticket_1318', '../specifications/alphabet.yml', ['tickets/1318/1.mcrl2'])
        self.addtest('ticket_1319', '../specifications/alphabet.yml', ['tickets/1319/1.mcrl2'])
        self.addtest('ticket_1321', '../specifications/alphabet.yml', ['tickets/1321/1.mcrl2'])

    def addtest(self, name, testfile, inputfiles, settings = ''):
        if settings:
            settings = yaml.load(settings)
        else:
            settings = dict()
        self.tests.append((name, testfile, inputfiles, settings))

    def name(self, testnum):
        if testnum < len(self.tests):
            (name, testfile, inputfiles, settings) = self.tests[testnum]
            return name

    def run(self, testnum):
        if testnum < len(self.tests):
            (name, testfile, inputfiles, settings) = self.tests[testnum]
            run_test(testfile, inputfiles, settings)
        else:
            raise RuntimeError('Invalid test number')

if __name__ == "__main__":
    sys.exit(TestRunner().main())
