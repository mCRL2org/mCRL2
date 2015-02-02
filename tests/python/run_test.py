#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Copyright 2014, 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import randgen
import yaml
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
from tools import Node, Tool, ToolFactory

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

        # Add node values specified in settings
        if 'nodes' in settings:
            for label in settings['nodes']:
                data['nodes'][label]['value'] = settings['nodes'][label]['value']

        #print yaml.dump(data)

        self.options = data['options']
        self.verbose = True if settings['verbose'] else False
        if 'toolpath' in settings:
            self.options['toolpath'] = settings['toolpath']

        self.nodes = []
        for label in data['nodes']: # create nodes
            self.__addNode(data['nodes'][label], label)

        self.tools = []
        for label in data['tools']: # create tools
            assert isinstance(data['tools'], dict)
            self.__addTool(data['tools'][label], label)

        for t in self.tools:
            if any(x for x in t.output_nodes if x.type == 'Bool'):
                t.has_output_nodes = False

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
            outputs = outputs + t.output_nodes
            inputs = inputs + t.input_nodes
        self.initials = OrderedDict.fromkeys([i for i in inputs if i not in outputs]).keys()

    def __addNode(self, data, label):
        value = ''
        if 'value' in data:
            value = data['value']
        self.nodes.append(Node(label, data['type'], value))

    def __addTool(self, data, label):
        import platform
        input = [i for i in self.nodes if i.label in data['input']]
        output = [o for o in self.nodes if o.label in data['output']]
        name = data['name']
        if platform.system() == 'Windows':
            name = name + '.exe'
        self.tools.append(ToolFactory().create_tool(label, data['name'], input, output, data['args']))

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
