#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Copyright 2014, 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import os.path
import shutil
import yaml
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
import testrunner
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

        self.verbose = settings.get('verbose', True)
        self.toolpath = settings.get('toolpath', '')
        self.cleanup_files = settings.get('cleanup_files', False)
        self.timeout = 5
        self.memlimit = 100000000

        # Reads a test from a YAML file
        self.name = file
        f = open(file)
        data = yaml.safe_load(f)

        # Add tool arguments specified in settings
        if 'tools' in settings:
            for tool in settings['tools']:
                if 'args' in settings['tools'][tool]:
                    data['tools'][tool]['args'] += settings['tools'][tool]['args']

        # Overwrite node values specified in settings
        if 'nodes' in settings:
            for label in settings['nodes']:
                data['nodes'][label]['value'] = settings['nodes'][label]['value']

        # Overwrite result value with the one specified in settings
        if 'result' in settings:
            data['result'] = settings['result']

        if 'memlimit' in settings:
            self.memlimit = settings['memlimit']

        if 'timeout' in settings:
            self.timeout = settings['timeout']

        #print yaml.dump(data)

        self.nodes = []
        for label in data['nodes']: # create nodes
            self._add_node(data['nodes'][label], label)

        self.tools = []
        for label in data['tools']: # create tools
            assert isinstance(data['tools'], dict)
            self._add_tool(data['tools'][label], label)

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
        input = sorted([node for node in self.nodes if node.label in data['input']], key = lambda node: node.label)
        output = sorted([node for node in self.nodes if node.label in data['output']], key = lambda node: node.label)
        name = data['name']
        if platform.system() == 'Windows':
            name = name + '.exe'
        self.tools.append(ToolFactory().create_tool(label, data['name'], self.toolpath, input, output, data['args']))

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
        exec(self.res, self.globals)
        return self.globals['result']

    def remaining_tasks(self):
        # Returns a list of tools that can be executed and have not been executed before
        return [tool for tool in self.tools if tool.can_execute()]

    def cleanup(self):
        if self.cleanup_files:
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
                tool.execute(timeout = self.timeout, memlimit = self.memlimit, verbose = self.verbose)
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
            if result:
                self.cleanup()
            return result

    # Returns the tool with the given label
    def tool(self, label):
        try:
            return next(tool for tool in self.tools if tool.label == label)
        except StopIteration:
            raise RuntimeError("could not find model a tool with label '{0}'".format(label))

    def print_commands(self, runpath):
        from topological_sort import topological_sort, insert_edge
        G = {}
        for node in self.nodes:
            G[node.label] = (set(), set())
        for tool in self.tools:
            G[tool.label] = (set(), set())
        for tool in self.tools:
            for node in tool.input_nodes:
                insert_edge(G, node.label, tool.label)
            for node in tool.output_nodes:
                insert_edge(G, tool.label, node.label)
        labels = topological_sort(G)
        toolmap = {}
        for tool in self.tools:
            toolmap[tool.label] = tool
        print '\n'.join([toolmap[label].command(runpath) for label in labels if label in toolmap])

def run_yml_test(name, testfile, inputfiles, settings):
    for filename in [testfile] + inputfiles:
        if not os.path.isfile(filename):
            print('Error:', filename, 'does not exist!')
            return
    t = Test(testfile, settings)
    if 'verbose' in settings and settings['verbose']:
        print 'Running test ' + testfile
    t.setup(inputfiles)
    result = t.run()
    print name, result
    if result == False:
        for filename in inputfiles:
            text = read_text(filename)
            print '- file {}\n{}\n'.format(filename, text)
    return result

def run_pbes_test(name, testfile, p, settings):
    filename = '{0}.txt'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    os.remove(filename)

def run_pbes_test_with_counter_example_minimization(name, testfile, p, settings):
    result = run_pbes_test(name, testfile, p, settings)
    if result == False:
        m = CounterExampleMinimizer(p, lambda x: run_pbes_test(testfile, x, name + '_minimize', settings), name)
        m.minimize()
        raise RuntimeError('Test {0} failed'.format(name))
    return result

class TestRunner(testrunner.TestRunner):
    def __init__(self):
        super(TestRunner, self).__init__()
        self.settings = {'toolpath': self._tool_path,
                         'verbose': self._args.verbose,
                         'cleanup_files': not self._args.keep_files}
        self.tests = []

    def main(self):
        if self._args.print_names:
            self.print_names()
        if self._args.command is not None:
            try:
                test = self.tests[self._args.command]
                test.print_commands(os.path.join(os.getcwd(), test.name))
            except Exception as e:
                sys.exit(str(e))
        super(TestRunner, self).main()

    def ymlfile(self, name):
        return '{}/tests/specifications/{}.yml'.format(self._source_path, name)

    def mcrl2file(self, file):
        return self._source_path + file

    def _get_commandline_parser(self):
        parser = super(TestRunner, self)._get_commandline_parser()
        parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Display additional progress messages.')
        parser.add_argument('-k', '--keep-files', dest='keep_files', action='store_true', help='Keep the files produced by the test')
        parser.add_argument('-p', '--print-names', dest='print_names', action='store_true', help='Print the names and the numbers of the tests')
        parser.add_argument('-c', '--print-commands', dest='command', metavar='N', type=int, action='store', help='Print the commands of test N, or exit with return value 1 if N is too large.')
        return parser

    def names(self):
        for test in self.tests:
            yield test.name

    # displays names and numbers of the tests
    def print_names(self):
        for i, test in enumerate(self.tests):
            print '{} {}'.format(i, test.name)

    def run(self, testnum):
        if testnum < len(self.tests):
            test = self.tests[testnum]
            test.settings.update(self.settings)
            test.execute_in_sandbox()
        else:
            raise RuntimeError('Invalid test number')
