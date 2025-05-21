#!/usr/bin/env python

# Copyright 2013, 2014 Mark Geelen.
# Copyright 2014, 2015 Wieger Wesselink.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import os.path
import shutil
import copy
import yaml
import io
from collections import defaultdict

from .run_process import (
    MemoryExceededError,
    TimeExceededError,
    StackOverflowError,
    ToolRuntimeError,
    SegmentationFault,
)
from .text_utility import read_text, write_text
from .tools import Node, ToolFactory
from .topological_sort import topological_sort


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


class YmlTest(object):
    def __init__(self, name, ymlfile, inputfiles, settings):

        if not settings:
            raise RuntimeError("ERROR in Test.__init__: settings == None")

        settings = copy.deepcopy(settings)
        self.name = name
        self.ymlfile = ymlfile
        self.inputfiles = inputfiles
        self.settings = settings
        self.python_path = settings['python_path']
        self.verbose = settings.get("verbose", True)
        self.toolpath = settings.get("toolpath", "")
        self.cleanup_files = settings.get("cleanup_files", False)
        self.timeout = 5
        self.memlimit = 1000
        self.allow_non_zero_return_values = settings.get(
            "allow-non-zero-return-values", False
        )

        # Reads a test from a YAML file
        with open(ymlfile, encoding="UTF-8") as f:
            data = yaml.safe_load(f)

            # Add tool arguments specified in settings
            if "tools" in settings:
                for tool in settings["tools"]:
                    if "args" in settings["tools"][tool]:
                        data["tools"][tool]["args"] += settings["tools"][tool]["args"]

            # Overwrite node values specified in settings
            if "nodes" in settings:
                for label in settings["nodes"]:
                    data["nodes"][label]["value"] = settings["nodes"][label]["value"]

            # Overwrite result value with the one specified in settings
            if "result" in settings:
                data["result"] = settings["result"]

            if "memlimit" in settings:
                self.memlimit = settings["memlimit"]

            if "timeout" in settings:
                self.timeout = settings["timeout"]

            # print yaml.dump(data)

            self.nodes = []
            for label in data["nodes"]:  # create nodes
                self._add_node(data["nodes"][label], label)

            self.tools = []
            for label in data["tools"]:  # create tools
                assert isinstance(data["tools"], dict)
                self._add_tool(data["tools"][label], label)

            self.result_code = data["result"]

        # These are the global variables used for the computation of the test result
        self.globals = {}
        for node in self.nodes:
            self.globals[node.label] = node
        for tool in self.tools:
            self.globals[tool.label] = tool

        # Contains a list of input nodes of this test, sorted by label
        self.input_nodes = self.compute_input_nodes()

        self.tasks = self.make_task_schedule()

    def __str__(self):

        out = io.StringIO()
        out.write("name        = " + str(self.name) + "\n")
        out.write("verbose     = " + str(self.verbose) + "\n")
        out.write("result_code = " + str(self.result_code) + "\n\n")
        out.write(
            "\n".join(["--- Node ---\n{0}".format(node) for node in self.nodes])
            + "\n\n"
        )
        out.write(
            "\n".join(["--- Tool ---\n{0}".format(tool) for tool in self.tools])
            + "\n\n"
        )
        out.write(
            "\n".join(["--- Init ---\n{0}".format(node) for node in self.input_nodes])
        )
        return out.getvalue()

    # Returns the input nodes of the test, ordered by label
    def compute_input_nodes(self):
        outputs = []
        for tool in self.tools:
            outputs = outputs + tool.output_nodes
        result = [node for node in self.nodes if not node in outputs]
        return sorted(result, key=lambda node: node.label)

    def _add_node(self, data, label):
        value = None
        if "value" in data:
            value = data["value"]
        self.nodes.append(Node(label, data["type"], value))

    def _find_node(self, label):
        return

    def _add_tool(self, data, label):
        input_nodes = [
            next(node for node in self.nodes if node.label == key)
            for key in data["input"]
        ]
        output_nodes = sorted(
            [node for node in self.nodes if node.label in data["output"]],
            key=lambda node: node.label,
        )
        self.tools.append(
            ToolFactory().create_tool(
                label,
                data["name"],
                self.toolpath,
                input_nodes,
                output_nodes,
                data["args"],
                self.python_path,
            )
        )

    def setup(self, inputfiles):
        input_nodes = [node for node in self.input_nodes if node.value is None]
        if len(input_nodes) != len(inputfiles):
            raise RuntimeError(
                "Tool {}, Invalid number of input files provided: expected {}, got {}".format(
                    self, len(input_nodes), len(inputfiles)
                )
            )
        for i in range(len(inputfiles)):
            shutil.copy(inputfiles[i], self.input_nodes[i].filename())
            self.input_nodes[i].value = read_text(inputfiles[i])

    def result(self):
        # Returns the result of the test after all tools have been executed
        try:
            exec(self.result_code, self.globals)
        except Exception as e:
            if isinstance(e, KeyError):
                print(
                    "A KeyError occurred during evaluation of the test result: {}".format(
                        e
                    )
                )
                print("result_code", self.result_code)
                print(self)
            else:
                print(
                    "An exception occurred during evaluation of the test result: {}".format(
                        e
                    )
                )
                print("result_code", self.result_code)
                print(self)
            return False
        return self.globals["result"]

    # Returns a valid schedule for executing the tools in this test
    def make_task_schedule(self):
        # Create a label based mapping E that contains outgoing edges for all nodes.
        E = defaultdict(lambda: set([]))
        for tool in self.tools:
            for node in tool.input_nodes:
                E[node.label].add(tool.label)
            for node in tool.output_nodes:
                E[tool.label].add(node.label)

        # Create a label based graph G
        G = defaultdict(lambda: (set([]), set([])))  # (predecessors, successors)
        for tool in self.tools:
            u = tool.label
            for v in E[u]:
                for w in E[v]:
                    G[u][1].add(w)
                    G[w][0].add(u)

        # Add isolated nodes
        for tool in self.tools:
            u = tool.label
            if not u in G:
                G[u] = (set([]), set([]))

        # Create a mapping tool_map from labels to tools
        tool_map = {}
        for tool in self.tools:
            tool_map[tool.label] = tool

        schedule = topological_sort(G)
        return [tool_map[label] for label in schedule]

    def cleanup(self):
        if self.cleanup_files:
            filenames = [node.filename() for node in self.nodes] + ["commands"]
            for filename in filenames:
                try:
                    os.remove(filename)
                except Exception as e:
                    if self.verbose:
                        print(e)

    def dump_file_contents(self):
        filenames = [node.filename() for node in self.nodes]
        for file in filenames:
            if os.path.exists(file) and (
                file.endswith(".mcrl2")
                or file.endswith(".pbesspec")
                or file.endswith(".mcf")
            ):
                contents = read_text(file)
                print(f"Contents of file {file}:\n{contents}")

    def _run(self):
        tasks = self.tasks[:]
        commands = [tool.command() for tool in tasks]

        while len(tasks) > 0:
            tool = tasks.pop(0)
            try:
                returncode = tool.execute(
                    timeout=self.timeout, memlimit=self.memlimit, verbose=self.verbose
                )
                if returncode != 0 and not self.allow_non_zero_return_values:
                    self.dump_file_contents()
                    self.print_commands(no_paths=True)
                    raise RuntimeError(
                        f"The execution of tool {tool.name} ended with return code {returncode}"
                    )
            except MemoryExceededError as e:
                print("Memory limit exceeded: " + str(e))
                self.cleanup()
                return None
            except TimeExceededError as e:
                print("Time limit exceeded: " + str(e))
                self.cleanup()
                return None
            except StackOverflowError:
                print(
                    "Stack overflow detected during execution of the tool "
                    + tool.name
                )
                self.cleanup()
                return None
            except (ToolRuntimeError, SegmentationFault) as e:
                self.dump_file_contents()
                self.print_commands(no_paths=True)
                raise e

        if all(tool.executed for tool in self.tools):
            for node in self.nodes:
                if not os.path.exists(node.filename()):
                    raise RuntimeError(
                        f"Error in test {self.name}: output file {node.filename()} is missing!"
                    )

            write_text("commands", "\n".join(commands))
            result = self.result()
            if not result:
                self.dump_file_contents()
                for tool in self.tools:
                    if tool.value != {}:
                        print(
                            "Output of {} {}: {}".format(
                                tool.name, " ".join(tool.args), tool.value
                            )
                        )
                self.print_commands(no_paths=True)
            else:
                self.cleanup()
            return result
        else:
            not_executed = [tool for tool in self.tools if not tool.executed]
            raise UnusedToolsError(not_executed)

    # Returns the tool with the given label
    def find_tool(self, label):
        try:
            return next(tool for tool in self.tools if tool.label == label)
        except StopIteration:
            raise RuntimeError(
                "could not find model a tool with label '{0}'".format(label)
            )

    # If no_paths is True, then all paths in the command are excluded
    def print_commands(self, working_directory=None, no_paths=False):
        print("#--- commands ---#")
        print(
            "\n".join(
                [tool.command(working_directory, no_paths) for tool in self.tasks]
            )
        )

    def result_string(self, result):
        if result is True:
            return "Pass"
        elif result is False:
            return "FAIL"
        else:
            return "Indeterminate"

    def execute(self):
        for filename in [self.ymlfile] + self.inputfiles:
            if not os.path.isfile(filename):
                print(f"Error: {filename} does not exist!")
                return
        if "verbose" in self.settings and self.settings["verbose"]:
            print("Running test " + self.ymlfile)

        self.setup(self.inputfiles)
        result = self._run()

        print(f"{self.name} {self.result_string(result)}", flush=True)
        if result is False:
            raise RuntimeError(
                "The result expression evaluated to False. The output of the tools likely does not match."
            )
        return result

    def execute_in_sandbox(self):
        runpath = self.name
        if not os.path.exists(runpath):
            os.mkdir(runpath)
        cwd = os.getcwd()
        os.chdir(runpath)
        self.execute()
        os.chdir(cwd)
        if os.listdir(runpath) == []:
            os.rmdir(runpath)

    def add_command_line_options(self, tool_label, options):
        tool = self.find_tool(tool_label)
        tool.args = tool.args + options
