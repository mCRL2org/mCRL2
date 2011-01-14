#~ Copyright 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import random
from path import *

def run_program(program, options, redirect = None):
    arg = '%s %s' % (program, options)
    if redirect != None:
        if os.name == 'nt':
            arg = arg + ' 2> %s 1>&2' % redirect
        else:
            arg = arg + ' >& %s' % redirect
    #print arg
    os.system(arg)

def timeout_command(command, timeout = -1):
    """call shell-command and either return its output or kill it
    if it doesn't normally exit within timeout seconds and return None"""
    import subprocess, datetime, os, time, signal, platform

    cmd = command.split(" ")
    start = datetime.datetime.now()
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    while process.poll() is None:
        time.sleep(0.1)
        now = datetime.datetime.now()
        if (now - start).seconds > timeout:
            if platform.system() == 'Windows':
                #os.kill(process.pid, signal.CTRL_C_EVENT)
                #os.kill(process.pid, signal.CTRL_BREAK_EVENT)
                os.kill(process.pid, signal.SIGABRT)
            else:
                os.kill(process.pid, signal.SIGKILL)
                os.waitpid(-1, os.WNOHANG)
            return None, None

    return (process.stdout.read(), process.stderr.read())

def last_word(line):
    words = line.strip().split()
    return words[len(words) - 1]

# returns True, False or None if a timeout occurs
def run_pbes2bool(filename, timeout = 3):
    dummy, text = timeout_command('pbes2bool %s' % filename, timeout)
    if text == None:
        print 'WARNING: timeout on %s' % filename
        return None
    return last_word(text) == 'true'

def run_txt2pbes(txtfile, pbesfile):
    run_program('txt2pbes', '%s %s' % (txtfile, pbesfile))

def run_pbesabstract(pbesfile1, pbesfile2, abstraction_value, selection = '*(*:*)'):
    run_program('pbesabstract', '--select=%s --abstraction-value=%d %s %s' % (selection, abstraction_value, pbesfile1, pbesfile2))

def run_pbesconstelm(pbesfile1, pbesfile2, rewriter = 'quantifier-all', timeout = 10):
    timeout_command('pbesconstelm -p%s %s %s' % (rewriter, pbesfile1, pbesfile2), timeout)

def run_pbesparelm(pbesfile1, pbesfile2, timeout = 10):
    timeout_command('pbesparelm %s %s' % (pbesfile1, pbesfile2), timeout)

def run_pbespareqelm(pbesfile1, pbesfile2, timeout = 10):
    timeout_command('pbespareqelm %s %s' % (pbesfile1, pbesfile2), timeout)

# returns the output of pbespp
def run_pbespp(pbesfile, timeout = 10):
    text, dummy = timeout_command('pbespp %s' % pbesfile, timeout)
    return text

# returns True if the operation succeeded within the given amount of time
def run_pbes2bes(pbesfile, besfile, strategy = 'lazy', output = 'bes', selection = '', timeout = 10):
    options = '-s%s -o%s' % (strategy, output)
    if selection != '':
        options = options + ' -f%s' % selection
    dummy, text = timeout_command('pbes2bes %s %s %s' % (options, pbesfile, besfile), timeout)
    if text == None:
        print 'WARNING: timeout on %s' % pbesfile     
        return False
    if text.startswith('error'):
        print 'WARNING: pbes2bes failed on %s' % pbesfile     
        return False
    return True

# returns True, False or None if a timeout occurs
def run_bessolve(filename, strategy = 'spm', timeout = 10):
    command = 'bessolve -s%s %s' % (strategy, filename)
    text, dummy = timeout_command(command, timeout)
    if text == None:
        print 'WARNING: timeout on "%s"' % command
        return None
    result = last_word(text)
    if result == 'true':
        return True
    elif result == 'false':
        return False
    print 'WARNING: unknown failure on "%s"' % command
    return None
