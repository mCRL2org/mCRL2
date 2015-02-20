#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import copy
import os
import os.path
from testing import run_yml_test, Test

class TestCommand(object):
    def __init__(self, name):
        self.name = name

    def execute(self, runpath = '.'):
        raise NotImplemented

    def print_commands(self, runpath = '.'):
        raise NotImplemented

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

class YmlTest(TestCommand):
    def __init__(self, name, ymlfile, inputfiles, settings = dict()):
        super(YmlTest, self).__init__(name)
        self.ymlfile = ymlfile
        self.inputfiles = inputfiles
        self.settings = copy.deepcopy(settings)

    def execute(self, runpath = '.'):
        run_yml_test(self.name, self.ymlfile, self.inputfiles, self.settings)

    def print_commands(self, runpath = '.'):
        test = Test(self.ymlfile, self.settings)
        test.print_commands(runpath)

    def set_command_line_options(self, tool_label, options):
        self.settings.update({'tools': {tool_label: {'args': options}}})
