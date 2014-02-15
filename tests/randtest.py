#~ Copyright 2013, 2014 Mark Geelen.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from __future__ import division
import sys, logging, randgen, time, os, yaml
from test import Test, ToolInputError, UnusedToolsError, ToolCrashedError
import argparse

parser = argparse.ArgumentParser(description='Executes random tests')
parser.add_argument('T', help='test file (.yml)')
parser.add_argument('-v', '--verbose', action='store_true', dest='verbose', help='print verbose output')
parser.add_argument('I', type=int, help='the number of tests that is performed')
parser.add_argument('N', type=int, help='the length of the randomly generated strings')
parser.add_argument('-r' '--report', action='store_true', dest='reporterrors', help='capture error messages in node values')
options = parser.parse_args()

termlength = options.N
testfile = options.T
numtests = options.I
reporterrors = not options.reporterrors

settings = dict()
settings['verbose'] = options.verbose

t = Test(testfile, settings)

grammarfiles = t.options['grammar']
constraintsfiles = t.options['constraints']

constraints = yaml.load(''.join([open(f).read() for f in constraintsfiles]))
grammar = ''.join([open(f).read() for f in grammarfiles])
generator = randgen.RandGen(grammar, constraints)

if not t.validTermLengths(generator, termlength):
    print 'invalid term length: ' + str(termlength)
else:
    for i in range(numtests):
        if settings['verbose']:
            print 'Running test ' + testfile
        t.reset()
        t.initialize(generator, termlength)

        try:

            result = t.run(reporterrors)
            if result == True:
                print 'Result: success'
            else:
                print 'Result: failed'

        except ToolInputError as e:
            print 'Result: ' + next(x for x in e.value.split('\n') if 'error' in x)

        except UnusedToolsError as e:
            print 'Result: UnusedToolsError'

        except ToolCrashedError as e:
            print 'Result: ToolCrashedError'