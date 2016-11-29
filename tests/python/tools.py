#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Copyright 2014, 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from subprocess import  PIPE
import os.path
import re
from text_utility import read_text

def is_list_of(l, types):
    if not isinstance(l, list):
        return False
    for x in l:
        if not isinstance(x, types):
            return False
    return True

class Node:
    def __init__(self, label, type, value):
        self.label = label
        self.type = type
        self.value = value
        return

    def __str__(self):
        return 'Node(label = {0}, type = {1}, value = {2})'.format(self.label, self.type, self.value)

    def filename(self):
        return '{}.{}'.format(self.label, self.type.lower())

class Tool(object):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert is_list_of(input_nodes, Node)
        assert is_list_of(output_nodes, Node)
        import platform
        self.label = label
        self.name = name
        self.toolpath = toolpath
        self.input_nodes = input_nodes
        self.output_nodes = output_nodes
        self.args = args
        self.executed = False
        self.value = {}
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

    def can_execute(self):
        if self.executed:
            return False
        for i in self.input_nodes:
            if i.value == None:
                return False
        return True

    # Raises an exception if the execution was aborted or produced an error
    def check_execution(self, process, timeout, memlimit, returncode):
        import platform
        import popen
        if process.user_time > timeout:
            raise popen.TimeExceededError(process.user_time)
        if process.max_virtual_memory > memlimit:
            raise popen.MemoryExceededError(process.max_virtual_memory)
        if returncode != 0:
            print('warning: tool {} ended with return code {}'.format(self.name, returncode))
        if platform.system() == 'Windows' and returncode == -1073741571:
            raise popen.StackOverflowError(self.name)
        if platform.system() == 'Linux' and returncode == -11:
            raise popen.SegmentationFault(self.name)
        if self.stderr and 'error' in self.stderr:
            raise RuntimeError('Tool {} failed: {}'.format(self.name, self.stderr))

    def arguments(self, runpath = None):
        if not runpath:
            runpath = os.getcwd()
        args = [os.path.join(runpath, node.filename()) for node in self.input_nodes]
        args = args + [os.path.join(runpath, node.filename()) for node in self.output_nodes if node.type != 'Bool']
        return args

    def assign_outputs(self):
        for node in self.output_nodes:
            if node.type == 'TEXT':
                text = read_text(node.filename())
                node.value = text
            else:
                node.value = 'executed'

    def parse_number(self, text, key, regex):
        m = re.search(regex, text)
        if m != None:
            self.value[key] = int(m.group(1))

    def parse_numbers(self, text, key1, key2, regex):
        m = re.search(regex, text)
        if m != None:
            self.value[key1] = int(m.group(1))
            self.value[key2] = int(m.group(2))

    def parse_boolean(self, text, key, regex, negated_regex = None):
        if negated_regex:
            m = re.search(negated_regex, text, re.DOTALL)
            if m != None:
                self.value[key] = False
        if regex:
            m = re.search(regex, text, re.DOTALL)
            if m != None:
                self.value[key] = True

    def parse_output(self):
        text = self.stdout + self.stderr
        self.parse_number(text, 'summand-count'               , 'Number of summands                  : (\d+)')
        self.parse_number(text, 'tau-summand-count'           , 'Number of tau-summands              : (\d+)')
        self.parse_number(text, 'global-variable-count'       , 'Number of declared global variables : (\d+)')
        self.parse_number(text, 'process-parameter-count'     , 'Number of process parameters        : (\d+)')
        self.parse_number(text, 'action-label-count'          , 'Number of declared action labels    : (\d+)')
        self.parse_number(text, 'used-action-label-count'     , 'Number of used actions              : (\d+)')
        self.parse_number(text, 'used-multi-action-count'     , 'Number of used multi-actions        : (\d+)')
        self.parse_number(text, 'state-count'                 , 'Number of states: (\d+)')
        self.parse_number(text, 'state-label-count'           , 'Number of state labels: (\d+)')
        self.parse_number(text, 'action-label-count'          , 'Number of action labels: (\d+)')
        self.parse_number(text, 'transition-count'            , 'Number of transitions: (\d+)')
        self.parse_number(text, 'equation-count'              , "Number of equations: (\d+)")
        self.parse_number(text, 'mu-count'                    , "Number of mu's:      (\d+)")
        self.parse_number(text, 'nu-count'                    , "Number of nu's:      (\d+)")
        self.parse_number(text, 'block-nesting-depth'         , "Block nesting depth: (\d+)")
        self.parse_numbers(text, 'confluent-tau-summand-count', 'tau-summand-count', '(\d+) of (\d+) tau summands were found to be confluent')
        self.parse_boolean(text, 'has-state-labels'           , 'Has state labels.', 'Does not have state labels.')
        self.parse_boolean(text, 'has-action-labels'          , 'Has action labels.')
        self.parse_boolean(text, 'is-deterministic'           , 'LTS is deterministic.', 'LTS is not deterministic.')
        self.parse_boolean(text, 'is-closed'                  , 'is closed', 'is not closed')
        self.parse_boolean(text, 'is-well-formed'             , 'well formed', 'not well formed')
        self.parse_boolean(text, 'is-well-typed'              , 'is well typed', 'is not well typed')
        self.parse_boolean(text, 'has-deadlock'               , 'deadlock-detect: deadlock found')
        self.parse_boolean(text, 'result'                     , r'LTSs are strongly bisimilar', 'LTSs are not strongly bisimilar')
        self.parse_boolean(text, 'result'                     , r'LTSs are branching bisimilar', 'LTSs are not branching bisimilar')
        self.parse_boolean(text, 'result'                     , r'LTSs are equal \(branching bisimilarity using the almost-O\(m log n\) Groote/Wijs algorithm\)', 'LTSs are not equal \(branching bisimilarity using the almost-O\(m log n\) Groote/Wijs algorithm\)')
        self.parse_boolean(text, 'result'                     , r'LTSs are equal \(branching bisimilarity using the O\(m log n\) Groote/Keiren/Jansen/Wijs algorithm\)', 'LTSs are not equal \(branching bisimilarity using the O\(m log n\) Groote/Keiren/Jansen/Wijs algorithm\)')
        self.parse_boolean(text, 'result'                     , r'LTSs are divergence preserving branching bisimilar', 'LTSs are not divergence preserving branching bisimilar')
        self.parse_boolean(text, 'result'                     , r'LTSs are weak bisimilar', 'LTSs are not weak bisimilar')
        self.parse_boolean(text, 'result'                     , r'LTSs are divergence preserving weak bisimilar', 'LTSs are not divergence preserving weak bisimilar')
        self.parse_boolean(text, 'result'                     , r'LTSs are strongly simulation equivalent', 'LTSs are not strongly simulation equivalent')
        self.parse_boolean(text, 'result'                     , r'LTSs are strongly trace equivalent', 'LTSs are not strongly trace equivalent')
        self.parse_boolean(text, 'result'                     , r'LTSs are weak trace equivalent', 'LTSs are not weak trace equivalent')
        self.parse_boolean(text, 'result'                     , r'LTSs are equal', 'LTSs are not equal')

    def command(self, runpath = None):
        args = self.arguments(runpath)
        name = os.path.join(self.toolpath, self.name)
        return ' '.join([name] + args + self.args)

    def check_exists(self, name):
        import platform
        if os.path.exists(name):
            return True
        if not name.endswith('.exe') and platform.system() == 'Windows':
            if os.path.exists(name + '.exe'):
                return True
        return False

    def execute(self, timeout, memlimit, verbose):
        import popen
        args = self.arguments()
        name = os.path.join(self.toolpath, self.name)
        if verbose:
            print('Executing ' + ' '.join([name] + args + self.args))
        if not self.check_exists(name):
            raise popen.ToolNotFoundError(name)
        process = popen.Popen([name] + args + self.args, stdout=PIPE, stdin=PIPE, stderr=PIPE, creationflags=self.subprocess_flags, maxVirtLimit=memlimit, usrTimeLimit=timeout)

        input = None
        stdout, stderr = process.communicate(input)
        self.stdout = stdout.decode("utf-8")
        self.stderr = stderr.decode("utf-8")
        self.executed = True
        self.user_time = process.user_time
        self.max_virtual_memory = process.max_virtual_memory
        self.check_execution(process, timeout, memlimit, process.returncode)
        self.assign_outputs()
        self.parse_output()
        return process.returncode

    def __str__(self):
        import io
        out = io.StringIO()
        out.write('label    = ' + str(self.label)    + '\n')
        out.write('name     = ' + str(self.name)     + '\n')
        out.write('input    = [{0}]\n'.format(', '.join([str(x) for x in self.input_nodes])))
        out.write('output   = [{0}]\n'.format(', '.join([str(x) for x in self.output_nodes])))
        out.write('args     = ' + str(self.args)     + '\n')
        out.write('stderr   = ' + str(self.stderr)    + '\n')
        out.write('executed = ' + str(self.executed) + '\n')
        return out.getvalue()

class SolveTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        super(SolveTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def assign_outputs(self):
        text = self.stdout.strip() + self.stderr.strip()
        if text.endswith('true'):
            value = True
        elif text.endswith('false'):
            value = False
        else:
            value = None
        self.value['solution'] = value

class Lps2PbesTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 2
        assert len(output_nodes) == 1
        super(Lps2PbesTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def arguments(self, runpath = None):
        if not runpath:
            runpath = os.getcwd()
        return [os.path.join(runpath, self.input_nodes[0].filename()),
                '-f' + os.path.join(runpath, self.input_nodes[1].filename()),
                os.path.join(runpath, self.output_nodes[0].filename())
               ]

class Lts2LpsTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 2
        assert len(output_nodes) == 1
        super(Lts2LpsTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def arguments(self, runpath = None):
        if not runpath:
            runpath = os.getcwd()
        return [os.path.join(runpath, self.input_nodes[0].filename()),
                '-l' + os.path.join(runpath, self.input_nodes[1].filename()),
                os.path.join(runpath, self.output_nodes[0].filename())
               ]

class Lps2LtsTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        super(Lps2LtsTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def parse_output(self):
        super(Lps2LtsTool, self).parse_output()
        # The tool lps2lts does not print a message if no deadlock is found...
        if '-D' in self.args and not 'has-deadlock' in self.value:
            self.value['has-deadlock'] = False

class ToolFactory(object):
    def create_tool(self, label, name, toolpath, input_nodes, output_nodes, args):
        if name == 'lps2pbes':
            return Lps2PbesTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'lps2lts':
            return Lps2LtsTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'lts2lps':
            return Lts2LpsTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name in ['pbespgsolve', 'pbes2bool', 'bessolve']:
            return SolveTool(label, name, toolpath, input_nodes, output_nodes, args)
        return Tool(label, name, toolpath, input_nodes, output_nodes, args)
