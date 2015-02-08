#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Copyright 2014, 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import os.path
import shutil
import yaml
from popen import Popen, MemoryExceededError, TimeExceededError
from subprocess import  PIPE, STDOUT
from text_utility import read_text, write_text
from tools import Node, Tool, ToolFactory

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'stage', 'bin')

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

        self.verbose = settings['verbose']
        self.toolpath = settings['toolpath']
        self.cleanup_files = settings['cleanup_files']

        self.nodes = []
        for label in data['nodes']: # create nodes
            self._add_node(data['nodes'][label], label)

        self.tools = []
        for label in data['tools']: # create tools
            assert isinstance(data['tools'], dict)
            self._add_tool(data['tools'][label], label)

        for tool in self.tools:
            if any(x for x in tool.output_nodes if x.type == 'Bool'):
                tool.has_output_nodes = False

        self.res = data['result']
        f.close()

        # These are the global variables used for the computation of the test result
        self.globals = {}
        for node in self.nodes:
            self.globals[node.label] = node

        # Contains a list of input nodes of this test, sorted by label
        self.input_nodes = self.compute_input_nodes()

    def __str__(self):
        import StringIO
        out = StringIO.StringIO()
        out.write('name     = ' + str(self.name)     + '\n')
        out.write('verbose  = ' + str(self.verbose)  + '\n')
        out.write('res      = ' + str(self.res)      + '\n\n')
        out.write('\n'.join(['--- Node ---\n{0}'.format(node) for node in self.nodes]) + '\n\n')
        out.write('\n'.join(['--- Tool ---\n{0}'.format(tool) for tool in self.tools]) + '\n\n')
        out.write('\n'.join(['--- Init ---\n{0}'.format(node) for node in self.input_nodes]))
        return out.getvalue()

    # Returns the input nodes of the test, ordered by label
    def compute_input_nodes(self):
        outputs = []
        for tool in self.tools:
            outputs = outputs + tool.output_nodes
        result = [node for node in self.nodes if not node in outputs]
        return sorted(result, key = lambda node: node.label)

    def _add_node(self, data, label):
        value = None
        if 'value' in data:
            value = data['value']
        self.nodes.append(Node(label, data['type'], value))

    def _add_tool(self, data, label):
        import platform
        input = [node for node in self.nodes if node.label in data['input']]
        output = [node for node in self.nodes if node.label in data['output']]
        name = data['name']
        if platform.system() == 'Windows':
            name = name + '.exe'
        self.tools.append(ToolFactory().create_tool(label, data['name'], input, output, data['args']))

    def setup(self, inputfiles):
        for node in self.input_nodes:
            if node.value:
                write_text(node.filename(), str(node.value))
        input_nodes = [node for node in self.input_nodes if node.value == None]
        if len(input_nodes) != len(inputfiles):
            raise RuntimeError('Invalid number of input files provided: expected {0}, got {1}'.format(len(input_nodes), len(inputfiles)))
        for i in range(len(inputfiles)):
            shutil.copy(inputfiles[i], self.input_nodes[i].filename())
            self.input_nodes[i].value = read_text(inputfiles[i])

    def result(self):
        # Returns the result of the test after all tools have been executed
        if self.verbose:
            print 'Validating result'
        exec(self.res, self.globals)
        return self.globals['result']

    def remaining_tasks(self):
        # Returns a list of tools that can be executed and have not been executed before
        return [tool for tool in self.tools if tool.can_execute()]

    def cleanup(self):
        for node in self.nodes:
            try:
                os.remove(node.filename())
            except Exception as e:
                if self.verbose:
                    print e

    def run(self):
        # Singlecore run
        tasks = self.remaining_tasks()
        while len(tasks) > 0:
            tool = tasks[0]
            try:
                tool.execute(self.toolpath, timeout = 5, memlimit = 100000000, verbose = self.verbose)
            except MemoryExceededError as e:
                if self.verbose:
                    print 'Memory limit exceeded: ' + str(e)
                self.cleanup()
                return None
            except TimeExceededError as e:
                if self.verbose:
                    print 'Time limit exceeded: ' + str(e)
                self.cleanup()
                return None
            tasks = self.remaining_tasks()

        if not all(tool.executed for tool in self.tools):
            not_executed = [tool for tool in self.tools if not tool.executed]
            raise UnusedToolsError(not_executed)
        else:
            for node in self.nodes:
                if not os.path.exists(node.filename()):
                    raise RuntimeError('Error in test {}: output file {} is missing!'.format(self.name, node.filename()))
            result = self.result()
            if result and self.cleanup_files:
                self.cleanup()
            return result

    # Returns the tool with the given label
    def tool(self, label):
        try:
            return next(tool for tool in self.tools if tool.label == label)
        except StopIteration:
            raise RuntimeError("could not find model a tool with label '{0}'".format(label))

def run_replay(testfile, inputfiles, settings, remove_files = True):
    for filename in [testfile] + inputfiles:
        if not os.path.isfile(filename):
            print('Error:', filename, 'does not exist!')
            return

    t = Test(testfile, settings)

    if 'verbose' in settings and settings['verbose']:
        print 'Running test ' + testfile
    t.setup(inputfiles)
    return t.run()

def run_yml_test(name, testfile, inputfiles, settings):
    result = run_replay(testfile, inputfiles, settings)
    print name, result
    if result == False:
        for filename in inputfiles:
            text = read_text(filename)
            print '- file {}\n{}\n'.format(filename, text)
    return result

def cleanup_files(result, files, settings):
    if result != False and settings['cleanup_files']:
        for filename in files:
            os.remove(filename)

def run_pbes_test(name, testfile, p, settings):
    filename = '{0}.txt'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_pbes_test_with_counter_example_minimization(name, testfile, p, settings):
    result = run_pbes_test(name, testfile, p, settings)
    if result == False:
        m = CounterExampleMinimizer(p, lambda x: run_pbes_test(testfile, x, name + '_minimize', settings), name)
        m.minimize()
        raise RuntimeError('Test {0} failed'.format(name))
    return result
