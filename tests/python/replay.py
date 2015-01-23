#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from __future__ import division
import sys, logging, randgen, time, os, yaml
from run_test import Test, ToolInputError, UnusedToolsError, ToolCrashedError
import argparse

parser = argparse.ArgumentParser(description='Executes random tests')
parser.add_argument('T', help='test file (.yml)')
parser.add_argument('I', help='input files', nargs='*')
parser.add_argument('-v', '--verbose', action='store_true', dest='verbose', help='print verbose output')
parser.add_argument('-r' '--report', action='store_true', dest='reporterrors', help='capture error messages in node values')

options = parser.parse_args()

testfile = options.T
inputfiles = options.I
reporterrors = not options.reporterrors

settings = dict()
settings['verbose'] = options.verbose

t = Test(testfile, settings)

if settings['verbose']:
    print 'Running test ' + testfile
t.reset()
t.replay(inputfiles)

try:

    result = t.run(reporterrors)
    if result == True:
        print 'Result: pass'
    else:
        print 'Result: fail'

except ToolInputError as e:
    print 'Result: ' + next(x for x in e.value.split('\n') if 'error' in x)

except UnusedToolsError as e:
    print 'Result: UnusedToolsError'

except ToolCrashedError as e:
    print 'Result: ToolCrashedError'
