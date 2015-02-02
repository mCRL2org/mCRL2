#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Copyright 2014, 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import randgen, yaml
from popen import Popen, MemoryExceededError, TimeExceededError
from subprocess import  PIPE, STDOUT
import shutil
import psutil
import time
import os.path
import threading
import multiprocessing
import exceptions
import sys
import re
import codecs
from collections import OrderedDict

class ToolInputError(Exception):
    def __init__(self, name, value):
        self.tool = name
        self.value = value
    def __str__(self):
        return repr(self.value)

class UnusedToolsError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class ToolCrashedError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class Node:
    def __init__(self, label, type, ext):
        self.label = label
        self.type = type
        self.value = None
        return

    def pp(self):
        return 'Node(label = {0}, type = {1}, value = {2})'.format(self.label, self.type, self.value)

    def __str__(self):
        return repr(self.value)

class Tool:
    def __init__(self, label, name, input, output, args):
        import platform
        self.label = label
        self.name = name
        self.input = input
        self.output = output
        self.args = args
        self.error = ''
        self.executed = False
        self.infiles = True
        self.outfiles = True
        if platform.system() == 'Windows':
            # Don't display the Windows GPF dialog if the invoked program dies.
            # See comp.os.ms-windows.programmer.win32
            # How to suppress crash notification dialog?, Raymond Chen Jan 14,2004 -
            import ctypes
            SEM_NOGPFAULTERRORBOX = 0x0002 # From MSDN
            ctypes.windll.kernel32.SetErrorMode(SEM_NOGPFAULTERRORBOX);
            self.subprocess_flags = 0x8000000 #win32con.CREATE_NO_WINDOW?
        else:
            self.subprocess_flags = 0

    def canExecute(self):
        for i in self.input:
            if i.value == None:
                return False
        return True

    # Raises an exception if the execution was aborted
    def checkExecution(self, p, timeout, memlimit):
        if p.maxVirtualMem > memlimit:
            raise MemoryExceededError(p.maxVirtualMem)
        if p.userTime > timeout:
            raise TimeExceededError(p.userTime)

    def execute(self, dir, timeout, memlimit, verbose, maxVirtLimit = 100000000, usrTimeLimit = 5):
        args = []
        if dir == None:
            dir = ''
        name = os.path.join(dir, self.name)
        if self.infiles:
            args = [os.path.join(os.getcwd(), i.label) for i in self.input]
        if self.outfiles:
            args = args + [os.path.join(os.getcwd(), o.label) for o in self.output]
        if verbose:
            print 'Executing ' + ' '.join([name] + args + self.args)
        p = Popen([name] + args + self.args, stdout=PIPE, stdin=PIPE, stderr=PIPE, creationflags=self.subprocess_flags, maxVirtLimit=maxVirtLimit, usrTimeLimit=usrTimeLimit)

        input = None
        if not self.infiles:
            input = (b' ').join([i.value for i in self.input])

        self.threadedExecute(p, input)
        self.executed = True
        self.userTime = p.userTime
        self.maxVirtualMem = p.maxVirtualMem
        self.checkExecution(p, timeout, memlimit)

    def __str__(self):
        import StringIO
        out = StringIO.StringIO()
        out.write('name     = ' + str(self.name)     + '\n')
        out.write('input    = [{0}]\n'.format(', '.join([x.pp() for x in self.input])))
        out.write('output   = [{0}]\n'.format(', '.join([x.pp() for x in self.output])))
        out.write('args     = ' + str(self.args)     + '\n')
        out.write('error    = ' + str(self.error)    + '\n')
        out.write('executed = ' + str(self.executed) + '\n')
        out.write('infiles  = ' + str(self.infiles)  + '\n')
        out.write('outfiles = ' + str(self.outfiles) + '\n')
        return out.getvalue()

    def threadedExecute(self, process, input):
        res = process.communicate(input)
        for o in self.output:
            o.value = res[0]
            if o.value == '':
                o.value = res[1]
            if o.value == '':
                o.value = 'not None'
            self.error = self.error + res[1]

class Test:
    def __init__(self, file, settings):
        from collections import Counter

        # Reads a test from a YAML file
        self.name = file
        f = open(file)
        data = yaml.safe_load(f)

        # Add tool arguments specified in settings
        if 'tools' in settings:
            for tool in settings['tools']:
                data['tools'][tool]['args'] += settings['tools'][tool]['args']
        # print yaml.dump(data)

        self.options = data['options']
        self.nodes = []
        self.verbose = True if settings['verbose'] else False
        if 'toolpath' in settings:
            self.options['toolpath'] = settings['toolpath']
        for n in data['nodes']: # create nodes
            type = data['nodes'][n]
            self.nodes.append(Node(n, type, ''))

        self.tools = []
        for label in data['tools']: # create tools
            assert isinstance(data['tools'], dict)
            self.__addTool(data['tools'][label], label)

        for t in self.tools:
            if any(x for x in t.output if x.type == 'Bool'):
                t.outfiles = False

        self.res = data['result']
        f.close()

        # These are the global variables used for the computation of the test result
        self.globals = {'value': self.value, 'last_word': self.last_word, 'file_get_contents': self.file_get_contents}
        for n in self.nodes:
            self.globals[n.label] = n

        # calculate the initial nodes
        self.__calcInitials()

    def __str__(self):
        import StringIO
        out = StringIO.StringIO()
        out.write('name     = ' + str(self.name)     + '\n')
        out.write('options  = ' + str(self.options)  + '\n')
        out.write('verbose  = ' + str(self.verbose)  + '\n')
        out.write('res      = ' + str(self.res)      + '\n\n')
        out.write('\n'.join(['--- Node ---\n{0}'.format(node) for node in self.nodes]) + '\n\n')
        out.write('\n'.join(['--- Tool ---\n{0}'.format(tool) for tool in self.tools]) + '\n\n')
        out.write('\n'.join(['--- Init ---\n{0}'.format(node) for node in self.initials]))
        return out.getvalue()

    def __calcInitials(self):
        outputs = []
        inputs = []
        for t in self.tools:
            outputs = outputs + t.output
            inputs = inputs + t.input
        self.initials = OrderedDict.fromkeys([i for i in inputs if i not in outputs]).keys()

    def __addTool(self, td, label):
        import platform
        input = [i for i in self.nodes if i.label in td['input']]
        output = [o for o in self.nodes if o.label in td['output']]
        name = td['name']
        if platform.system() == 'Windows':
            name = name + '.exe'
        self.tools.append(Tool(label, td['name'], input, output, td['args']))

    def replay(self, inputfiles):
        if len(self.initials) != len(inputfiles):
            raise RuntimeError('Invalid number of input files provided: expected {0}, got {1}'.format(len(self.initials), len(inputfiles)))
        for i in range(len(self.initials)):
            f = open(inputfiles[i])
            self.initials[i].label = inputfiles[i]
            self.initials[i].value = f.read()

    def result(self):
        # Returns the result of the test after all tools have been executed
        if self.verbose:
            print 'Validating result'
        exec(self.res, self.globals)
        return self.globals['result']

    # Returns the value of the node. Enables 'value(l1)' in the YAML
    def value(self, node):
        try:
            if node.value or node.type == 'Bool':
                return node.value.strip()
            else:
                f = open(os.path.join(os.getcwd(), node.label), 'r')
                res = f.read()
                f.close()
                return res
        except IOError:
            print 'cannot open stored value file'
            raise IOError

    def file_get_contents(self, filename):
        with open(filename) as f:
            return f.read()

    def record(self):
        for n in self.initials:
            if n.type != 'Bool':
                shutil.copyfile(os.path.join(os.getcwd(), n.label), os.path.join(os.getcwd(), 'recorded', n.label + '%0.2f' % time.clock()))


    def last_word(self, term):
        if ' ' in term:
            return term.split(' ')[-1]
        else:
            return None

    def remaining_tasks(self):
        # Returns a list of tools that can be executed and have not been executed before
        return [t for t in self.tools if not t.executed and t.canExecute()]

    def reset(self):
        # Reset the test to the initial values
        for n in self.nodes:
            n.value = None
            try:
                os.remove(os.path.join(os.getcwd(), n.label))
            except OSError as e:
                pass
        for t in self.tools:
            t.executed = False
            t.error = ''

    def run(self, reporterrors):
        # Singlecore run
        tasks = self.remaining_tasks()
        while len(tasks) > 0:
            t = tasks[0]
            t.execute(self.options['toolpath'], timeout = 5, memlimit = 100000000, verbose = self.verbose)
            if reporterrors and t.error != '' and 'error' in t.error:
                raise ToolInputError(t.name, t.error)
            tasks = self.remaining_tasks()

        if reporterrors and not all(t.executed for t in self.tools):
            raise UnusedToolsError([t for t in self.tools if not t.executed])
        else:
            return self.result()

    def run_multicore(self, max):
        # Multicore run
        tasks = self.remaining_tasks()
        while len(tasks) > 0:
            pool = multiprocessing.Pool()
            jobs = []
            for i in range(min(len(tasks), max)):
                p = multiprocessing.Process(target=tasks[i].execute, args=(self.options['toolpath'],))

                p.daemon = True
                jobs.append(p)
                p.start()

            for j in jobs:
                j.join()
                if j.exitcode != 0:
                    return 'Error'
            tasks = self.remaining_tasks()

        if not all(t.executed for t in self.tools):
            return 'Error'
        else:
            return self.result()

    def write_output(self, dir):
        if not os.path.exists(dir):
            os.makedirs(dir)
        for l in self.initials:
            fname =  l.label + '_' + l.type
            f = open(os.path.join(dir, fname), 'w')
            f.write(l.value)
            f.close()

    # Returns the tool with the given label
    def tool(self, label):
        try:
            return next(tool for tool in self.tools if tool.label == label)
        except StopIteration:
            raise RuntimeError("could not find model a tool with label '{0}'".format(label))
