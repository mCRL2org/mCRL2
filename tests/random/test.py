#~ Copyright 2015 Sjoerd Cranen.
#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)
import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
import testrunner

def generate_tests(name, testfunction, size):
    return [('random_{}_{}'.format(name, i), lambda *args: getattr(random_testing, testfunction)(*args)) for i in range(size)]

def generate_pbesrewr_tests(rewriter, testfunction, size):
    return [('random_pbesrewr_{}_{}'.format(rewriter, i), lambda source_path, name, settings: getattr(random_testing, testfunction)(source_path, name, rewriter, settings) ) for i in range(size)]

class RandomTestRunner(testrunner.TestRunner):
    def __init__(self):
        super(RandomTestRunner, self).__init__()
        self.test_path = os.path.join(os.getcwd(), os.path.dirname(__file__))
        self.tests = generate_tests('bessolve'       , 'run_bessolve_test'       , self._args.count) + \
                     generate_tests('lps2pbes'       , 'run_lps2pbes_test'       , self._args.count) + \
                     generate_tests('pbesabstract'   , 'run_pbesabstract_test'   , self._args.count) + \
                     generate_tests('pbesconstelm'   , 'run_pbesconstelm_test'   , self._args.count) + \
                     generate_tests('pbesinst_finite', 'run_pbesinst_finite_test', self._args.count) + \
                     generate_tests('pbesinst_lazy'  , 'run_pbesinst_lazy_test'  , self._args.count) + \
                     generate_tests('pbessolve'      , 'run_pbessolve_test'      , self._args.count) + \
                     generate_tests('alphabet'       , 'run_alphabet_test'       , self._args.count) + \
                     generate_tests('lpsconfcheck_c' , 'run_lpsconfcheck_c_test' , self._args.count) + \
                     generate_tests('lpsconfcheck_capital_c' , 'run_lpsconfcheck_capital_c_test' , self._args.count) + \
                     generate_tests('lpsconfcheck_d' , 'run_lpsconfcheck_d_test' , self._args.count) + \
                     generate_tests('lpsconfcheck_t' , 'run_lpsconfcheck_t_test' , self._args.count) + \
                     generate_tests('lpsconfcheck_z' , 'run_lpsconfcheck_z_test' , self._args.count) + \
                     generate_tests('lpsbinary'      , 'run_lpsbinary_test'      , self._args.count) + \
                     generate_tests('lpsconstelm'    , 'run_lpsparelm_test'      , self._args.count) + \
                     generate_tests('lpsparelm'      , 'run_lpsparelm_test'      , self._args.count) + \
                     generate_tests('lpssumelm'      , 'run_lpssumelm_test'      , self._args.count) + \
                     generate_tests('lpssuminst'     , 'run_lpssuminst_test'     , self._args.count) + \
                     generate_pbesrewr_tests('simplify'            , 'run_pbesrewr_test', self._args.count) + \
                     generate_pbesrewr_tests('pfnf'                , 'run_pbesrewr_test', self._args.count) + \
                     generate_pbesrewr_tests('quantifier-all'      , 'run_pbesrewr_test', self._args.count) + \
                     generate_pbesrewr_tests('quantifier-finite'   , 'run_pbesrewr_test', self._args.count) + \
                     generate_pbesrewr_tests('quantifier-one-point', 'run_pbesrewr_test', self._args.count) + \
                     generate_tests('pbesparelm'     , 'run_pbesparelm_test'     , self._args.count) + \
                     generate_tests('pbespareqelm'   , 'run_pbespareqelm_test'   , self._args.count) + \
                     generate_tests('pbesstategraph' , 'run_pbesstategraph_test' , self._args.count)

        self.settings = { 'toolpath': self._tool_path,
                          'verbose': self._args.verbose,
                          'cleanup_files': True
                        }

    def _get_commandline_parser(self):
        parser = super(RandomTestRunner, self)._get_commandline_parser()
        parser.add_argument('--verbose', dest='verbose', action='store_true', help='Display additional progress messages.')
        parser.add_argument('--count', metavar='Count', type=int, action='store', default=100, help='The number of repetitions for each random test')
        return parser

    def names(self):
        for test in self.tests:
            yield test[0]

    def run(self, testnum):
        global random_testing
        import random_testing
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
