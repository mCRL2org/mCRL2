#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
import testing
from regression_testing import regression_tests, pbessolve_tests

class TestRunner(testing.TestRunner):
    def __init__(self):
        super(TestRunner, self).__init__()
        self.tests = []
        for name in sorted(regression_tests):
            self.tests.append(regression_tests[name]('regression_{}'.format(name), self.settings))
        for name in sorted(pbessolve_tests):
            self.tests.append(pbessolve_tests[name]('regression_{}'.format(name), self.settings))

if __name__ == "__main__":
    sys.exit(TestRunner().main())
