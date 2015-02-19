#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from testing import run_yml_test, Test

class TestCommand(object):
    def __init__(self, name):
        self.name = name

    def execute(self):
        raise NotImplemented

    def print_commands(self, runpath):
        raise NotImplemented


class YmlTest(TestCommand):
    def __init__(self, name, ymlfile, inputfiles, settings = dict()):
        super(YmlTest, self).__init__(name)
        self.ymlfile = ymlfile
        self.inputfiles = inputfiles
        self.settings = settings

    def execute(self):
        run_yml_test(self.name, self.ymlfile, self.inputfiles, self.settings)

    def print_commands(self, runpath):
        test = Test(self.ymlfile, self.settings)
        test.print_commands(runpath)
