import glob
import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
import testrunner
from testing import run_test

class TestRunner(testrunner.TestRunner):
    def __init__(self):
        self.tests = []

        # add pbessolve tests
        index = 0
        for filename in sorted(glob.glob(os.path.join(os.path.dirname(__file__), 'pbessolve', '*.txt'))):
            self.tests.append(('pbessolve', 'pbessolve{0}'.format(index), filename))
            index = index + 1

    def name(self, testnum):
        if testnum < len(self.tests):
            return self.tests[testnum][1]

    def run(self, testnum):
        if testnum < len(self.tests):
            testfile = '../specifications/pbessolve.yml'
            filename = self.tests[testnum][2]
            run_test(testfile, [filename])
        else:
            raise RuntimeError('Invalid test number')

if __name__ == "__main__":
    sys.exit(TestRunner().main())
