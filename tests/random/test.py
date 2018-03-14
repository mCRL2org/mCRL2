#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
import testing
from random_testing import available_tests
from testcommand import YmlTest

class TestRunner(testing.TestRunner):
    def __init__(self):
        super(TestRunner, self).__init__()
        I = range(self._args.count)
        self.tests = []
        for name in sorted(available_tests):
            # TODO: remove this when g++ 4.8 is no longer used
            if name == 'pbessolve-counter-example':
                continue

            for i in I:
                self.tests.append(available_tests[name]('random_{}_{}'.format(name, i), self.settings))

    def _get_commandline_parser(self):
  	    parser = super(TestRunner, self)._get_commandline_parser()
  	    parser.add_argument('--count', metavar='Count', type=int, action='store', default=100, help='The number of repetitions for each random test')
  	    return parser

if __name__ == "__main__":
    sys.exit(TestRunner().main())
