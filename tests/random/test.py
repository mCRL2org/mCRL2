#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
import testing
from random_testing import *
from testcommand import YmlTest

class TestRunner(testing.TestRunner):
    def __init__(self):
        super(TestRunner, self).__init__()
        I = range(self._args.count)
        self.tests = [ AlphabetTest('alphabet_{}'.format(i), self.settings)                         for i in I ] + \
                     [ LpsSuminstTest('lpssuminst_{}'.format(i), self.settings)                     for i in I ] + \
                     [ LpsSumelmTest('lpssumelm_{}'.format(i), self.settings)                       for i in I ] + \
                     [ LpsParelmTest('lpsparelm_{}'.format(i), self.settings)                       for i in I ] + \
                     [ LpsConfcheckTest('lpsconfcheck_c_{}'.format(i), 'c', self.settings)          for i in I ] + \
                     [ LpsConfcheckTest('lpsconfcheck_capital_c_{}'.format(i), 'C', self.settings)  for i in I ] + \
                     [ LpsConfcheckTest('lpsconfcheck_d_{}'.format(i), 'd', self.settings)          for i in I ] + \
                     [ LpsConfcheckTest('lpsconfcheck_t_{}'.format(i), 'T', self.settings)          for i in I ] + \
                     [ LpsConfcheckTest('lpsconfcheck_z_{}'.format(i), 'Z', self.settings)          for i in I ] + \
                     [ LpsConstelmTest('lpsconstelm_{}'.format(i), self.settings)                   for i in I ] + \
                     [ LpsBinaryTest('lpsbinary_{}'.format(i), self.settings)                       for i in I ] + \
                     [ Lps2pbesTest('lps2pbes_{}'.format(i), self.settings)                         for i in I ] + \
                     [ PbesabstractTest('pbesabstract_{}'.format(i), self.settings)                 for i in I ] + \
                     [ PbesconstelmTest('pbesconstelm_{}'.format(i), self.settings)                 for i in I ] + \
                     [ PbesparelmTest('pbesparelm_{}'.format(i), self.settings)                     for i in I ] + \
                     [ PbespareqelmTest('pbespareqelm_{}'.format(i), self.settings)                 for i in I ] + \
                     [ PbesrewrTest('pbesrewr_{}'.format(i), 'simplify', self.settings)             for i in I ] + \
                     [ PbesrewrTest('pbesrewr_{}'.format(i), 'pfnf', self.settings)                 for i in I ] + \
                     [ PbesrewrTest('pbesrewr_{}'.format(i), 'quantifier-all', self.settings)       for i in I ] + \
                     [ PbesrewrTest('pbesrewr_{}'.format(i), 'quantifier-finite', self.settings)    for i in I ] + \
                     [ PbesrewrTest('pbesrewr_{}'.format(i), 'quantifier-one-point', self.settings) for i in I ] + \
                     [ Pbesinst_lazyTest('pbesinst_lazy_{}'.format(i), self.settings)               for i in I ] + \
                     [ Pbesinst_finiteTest('pbesinst_finite_{}'.format(i), self.settings)           for i in I ] + \
                     [ PbessolveTest('pbessolve_{}'.format(i), self.settings)                       for i in I ] + \
                     [ PbesstategraphTest('pbesstategraph_{}'.format(i), self.settings)             for i in I ] + \
                     [ BessolveTest('bessolve_{}'.format(i), self.settings)                         for i in I ]

        for test in self.tests:
            test.name = 'random_' + test.name

    def _get_commandline_parser(self):
  	    parser = super(TestRunner, self)._get_commandline_parser()
  	    parser.add_argument('--count', metavar='Count', type=int, action='store', default=100, help='The number of repetitions for each random test')
  	    return parser

if __name__ == "__main__":
    sys.exit(TestRunner().main())
