#~ Copyright 2010 Wieger Wesselink.
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

def run_pbes2bool(filename, timeout = 3):
    dummy, text = timeout_command('pbes2bool %s' % filename, timeout)
    if text == None:
        print 'WARNING: timeout on %s' % filename
        return None
    return last_word(text) == 'true'

def run_txt2pbes(txtfile, pbesfile):
    run_program('txt2pbes', '%s %s' % (txtfile, pbesfile))

def run_pbesabstract(pbesfile1, pbesfile2, abstraction_value, selection = '*(*:*)'):
    run_program('pbesabstract', '--select=%s --abstraction-value=%s %s %s' % (selection, abstraction_value, pbesfile1, pbesfile2))

def run_pbesconstelm(pbesfile1, pbesfile2, rewriter = 'quantifier-all', timeout = 10):
    timeout_command('pbesconstelm -p%s %s %s' % (rewriter, pbesfile1, pbesfile2), timeout)
  