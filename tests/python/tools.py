#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Copyright 2014, 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from popen import Popen, MemoryExceededError, TimeExceededError
from subprocess import  PIPE, STDOUT
import os.path
import types

def is_list_of(l, types):
    if not isinstance(l, list):
        return False
    for x in l:
        if not isinstance(x, types):
            return False
    return True

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
    def __init__(self, label, name, input_nodes, output_nodes, args):
        assert is_list_of(input_nodes, Node)
        assert is_list_of(output_nodes, Node)
        import platform
        self.label = label
        self.name = name
        self.input_nodes = input_nodes
        self.output_nodes = output_nodes
        self.args = args
        self.error = ''
        self.executed = False
        self.has_input_nodes = True
        self.has_output_nodes = True
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
        for i in self.input_nodes:
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
        if self.has_input_nodes:
            args = [os.path.join(os.getcwd(), i.label) for i in self.input_nodes]
        if self.has_output_nodes:
            args = args + [os.path.join(os.getcwd(), o.label) for o in self.output_nodes]
        if verbose:
            print 'Executing ' + ' '.join([name] + args + self.args)
        p = Popen([name] + args + self.args, stdout=PIPE, stdin=PIPE, stderr=PIPE, creationflags=self.subprocess_flags, maxVirtLimit=maxVirtLimit, usrTimeLimit=usrTimeLimit)

        input = None
        if not self.has_input_nodes:
            input = (b' ').join([i.value for i in self.input_nodes])

        self.threadedExecute(p, input)
        self.executed = True
        self.userTime = p.userTime
        self.maxVirtualMem = p.maxVirtualMem
        self.checkExecution(p, timeout, memlimit)

    def __str__(self):
        import StringIO
        out = StringIO.StringIO()
        out.write('label    = ' + str(self.label)    + '\n')
        out.write('name     = ' + str(self.name)     + '\n')
        out.write('input    = [{0}]\n'.format(', '.join([x.pp() for x in self.input_nodes])))
        out.write('output   = [{0}]\n'.format(', '.join([x.pp() for x in self.output_nodes])))
        out.write('args     = ' + str(self.args)     + '\n')
        out.write('error    = ' + str(self.error)    + '\n')
        out.write('executed = ' + str(self.executed) + '\n')
        out.write('has_input_nodes  = ' + str(self.has_input_nodes)  + '\n')
        out.write('has_output_nodes = ' + str(self.has_output_nodes) + '\n')
        return out.getvalue()

    def threadedExecute(self, process, input):
        res = process.communicate(input)
        for o in self.output_nodes:
            o.value = res[0]
            if o.value == '':
                o.value = res[1]
            if o.value == '':
                o.value = 'not None'
            self.error = self.error + res[1]

class ToolFactory:
    def create_tool(self, label, name, input_nodes, output_nodes, args):
        return Tool(label, name, input_nodes, output_nodes, args)