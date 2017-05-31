#!/usr/bin/env python

#~ Copyright 2012-2017 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

class DataExpression(object):
    def __init__(self):
        pass

class Variable(DataExpression):
    def __init__(self, name, type = 'Bool'):
        self.name = name
        self.type = type

    def __str__(self):
        return self.name

class Integer(DataExpression):
    def __init__(self, value):
        self.value = value
        self.type = 'Int'

    def __str__(self):
        return self.value

class Boolean(DataExpression):
    def __init__(self, value):
        self.value
        self.type = 'Bool'

    def __str__(self):
        return self.value
