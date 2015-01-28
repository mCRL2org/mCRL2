#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen
#~ Copyright 2015, Wieger Wesselink
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from __future__ import division
import sys, logging, randgen, time, os, yaml
from run_test import Test, ToolInputError, UnusedToolsError, ToolCrashedError
from popen import MemoryExceededError, TimeExceededError
import argparse
import os.path

def run_replay(testfile, inputfiles, reporterrors, settings):
    for filename in [testfile] + inputfiles:
        if not os.path.isfile(filename):
            print('Error:', filename, 'does not exist!')
            return

    t = Test(testfile, settings)

    if settings['verbose']:
        print 'Running test ' + testfile
    t.reset()
    t.replay(inputfiles)

    try:
        result = t.run(reporterrors)
        if result == True:
            return True, ''
        else:
            return False, ''
    except ToolInputError as e:
        return False, next(x for x in e.value.split('\n') if 'error' in x)
    except UnusedToolsError as e:
        return False, 'UnusedToolsError'
    except ToolCrashedError as e:
        return False, 'ToolCrashedError'
    except MemoryExceededError as e:
        return None, 'Memory Exceeded'
    except TimeExceededError as e:
        return None, 'Time Exceeded'

def main():
    parser = argparse.ArgumentParser(description='Executes random tests')
    parser.add_argument('T', help='test file (.yml)')
    parser.add_argument('I', help='input files', nargs='*')
    parser.add_argument('-v', '--verbose', action='store_true', dest='verbose', help='print verbose output')
    parser.add_argument('-r' '--report', action='store_true', dest='reporterrors', help='capture error messages in node values')
    parser.add_argument('-t' '--toolpath', action='store_true', dest='toolpath', help='the location of the tools')

    options = parser.parse_args()

    testfile = options.T
    inputfiles = options.I
    reporterrors = not options.reporterrors

    settings = dict()
    settings['verbose'] = options.verbose

    run_replay(testfile, inputfiles, reporterrors, settings)

if __name__ == '__main__':
    main()
