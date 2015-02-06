#~ Copyright 2015 Sjoerd Cranen.
#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import yaml
import shutil # TODO: remove this import
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
import testrunner
from testing import run_yml_test
from random_testing import *

def generate_tests(name, testfunction, size):
    return [('{}_{}'.format(name, i), testfunction) for i in range(size)]

def generate_pbesrewr_tests(rewriter, testfunction, size):
    return [('pbesrewr-{}_{}'.format(rewriter, i), lambda source_path, name, settings: testfunction(source_path, name, rewriter, settings) ) for i in range(size)]

class RandomTestRunner(testrunner.TestRunner):
    def __init__(self):
        super(RandomTestRunner, self).__init__()
        self.test_path = os.path.join(os.getcwd(), os.path.dirname(__file__))
        testcount = 100
        self.tests = generate_tests('bessolve'       , run_bessolve_test       , testcount) + \
                     generate_tests('lps2pbes'       , run_lps2pbes_test       , testcount) + \
                     generate_tests('pbesabstract'   , run_pbesabstract_test   , testcount) + \
                     generate_tests('pbesconstelm'   , run_pbesconstelm_test   , testcount) + \
                     generate_tests('pbesinst_finite', run_pbesinst_finite_test, testcount) + \
                     generate_tests('pbesinst_lazy'  , run_pbesinst_lazy_test  , testcount) + \
                     generate_tests('pbessolve'      , run_pbessolve_test      , testcount) + \
                     generate_tests('alphabet'       , run_alphabet_test       , testcount) + \
                     generate_pbesrewr_tests('simplify'            , run_pbesrewr_test, testcount) + \
                     generate_pbesrewr_tests('pfnf'                , run_pbesrewr_test, testcount) + \
                     generate_pbesrewr_tests('quantifier-all'      , run_pbesrewr_test, testcount) + \
                     generate_pbesrewr_tests('quantifier-finite'   , run_pbesrewr_test, testcount) + \
                     generate_pbesrewr_tests('quantifier-one-point', run_pbesrewr_test, testcount)
                     #generate_tests('pbesparelm'     , run_pbesparelm_test     , testcount) + \
                     #generate_tests('pbespareqelm'   , run_pbespareqelm_test   , testcount) + \
                     #generate_tests('pbesstategraph' , run_pbesstategraph_test , testcount) + \
                     #generate_tests('lpsbinary'      , run_lpsbinary_test      , testcount) + \
                     #generate_tests('lpsconstelm'    , run_lpsparelm_test      , testcount) + \
                     #generate_tests('lpsparelm'      , run_lpsparelm_test      , testcount) + \
                     #generate_tests('lpssumelm'      , run_lpssumelm_test      , testcount) + \
                     #generate_tests('lpssuminst'     , run_lpssuminst_test     , testcount) + \

        self.settings = { 'toolpath': self._tool_path,
                          'verbose': True,
                          'cleanup_files': True
                        }

    def name(self, testnum):
        if testnum < len(self.tests):
            return self.tests[testnum][0]

    def run(self, testnum):
        if testnum < len(self.tests):
            test = self.tests[testnum]
            name, testfunc = test[:2]

            # Create a settings dict based on self.settings and, optionally, the test-specific
            # settings
            settings = {}
            settings.update(self.settings)
            if len(test) > 2:
                settings.update(test[2])

            # Create sandbox directory for the test to run in
            os.mkdir(name)
            os.chdir(name)

            testfunc(self._source_path, name, settings)

            # If the test did not throw an exception, we assume it was successful and remove
            # the directory we created. We assume that run_yml_test has deleted any files it
            # might have created in the test directory, so we only have to remove an empty
            # directory here.
            os.chdir('..')
            os.rmdir(name)
        else:
            raise RuntimeError('Invalid test number')

if __name__ == "__main__":
    sys.exit(RandomTestRunner().main())
    #RandomTestRunner().run(25)
