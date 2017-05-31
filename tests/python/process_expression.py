#!/usr/bin/env python

#~ Copyright 2012-2017 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

#--------------------------------------------------
# expression                  | #children  | pcrl |
#--------------------------------------------------
# Action                      |     0      |  y   |
# Delta                       |     0      |  y   |
# Tau                         |     0      |  y   |
# ProcessInstance             |     0      |  y   |
# Sum                         |     1      |  y   |
# IfThen                      |     1      |  y   |
# IfThenElse                  |     2      |  y   |
# Choice                      |     2      |  y   |
# Seq                         |     2      |  y   |
# Block                       |     1      |  n   |
# Hide                        |     1      |  n   |
# Rename                      |     1      |  n   |
# Comm                        |     1      |  n   |
# Allow                       |     1      |  n   |
# Sync                        |     2      |  n   |
# Merge                       |     2      |  n   |
# LeftMerge                   |     2      |  n   |
#--------------------------------------------------
# unsupported                                     |
#--------------------------------------------------
# process_instance_assignment |     0      |  y   |
# At                          |     1      |  y   |
# binit                       |     2      |  y   |

import re
from data_expression import *
import random_data_expression

# example: 'b: Bool'
def parse_variable(text):
    text = text.strip()
    m = re.match('([^,:]+)\s*\:(.+)', text)
    result = Variable(m.group(1).strip(), m.group(2).strip())
    return result

# example: 'b: Bool, m: Nat'
def parse_variables(text):
    import string
    variables = filter(None, list(map(str.strip, text.split(','))))
    return map(parse_variable, variables)

# example: 'P(m: Nat, b: Bool)'
class ProcessIdentifier(object):
    def __init__(self, text):
        m = re.search(r'(\w*)(\(.*\))?', text)
        self.name = m.group(1)
        if m.group(2):
            vartext = m.group(2)[1:-1]
            self.variables = parse_variables(vartext)
        else:
            self.variables = []

    def __str__(self):
        if self.variables:
            return '{}({})'.format(self.name, ', '.join(map(str, self.variables)))
        else:
            return self.name

class ProcessExpression(object):
    pass

class Action(ProcessExpression):
    def __init__(self, a):
        self.a = a

    def __str__(self):
        return self.a

class Delta(ProcessExpression):
    def __str__(self):
        return 'delta'

class Tau(ProcessExpression):
    def __str__(self):
        return 'tau'

class ProcessInstance(ProcessExpression):
    def __init__(self, identifier, parameters = []):
        assert isinstance(identifier, ProcessIdentifier)
        self.identifier = identifier
        self.parameters = parameters

    def __str__(self):
        if self.parameters:
            return '{}({})'.format(self.identifier.name, ', '.join(self.parameters))
        else:
            return self.identifier.name

class Sum(ProcessExpression):
    def __init__(self, d, x):
        assert isinstance(d, Variable)
        self.d = d
        self.x = x

    def __str__(self):
        d = self.d
        x = self.x
        return 'sum {}: {}. ({})'.format(d, d.type, x)

class IfThen(ProcessExpression):
    def __init__(self, c, x):
        self.x = x
        self.c = c

    def __str__(self):
        x = self.x
        c = self.c
        return '({}) -> ({})'.format(c, x)

class IfThenElse(ProcessExpression):
    def __init__(self, c, x, y):
        self.c = c
        self.x = x
        self.y = y

    def __str__(self):
        c = self.c
        x = self.x
        y = self.y
        return '({}) -> ({}) <> ({})'.format(c, x, y)

class BinaryOperator(ProcessExpression):
    def __init__(self, op, x, y):
        self.op = op
        self.x = x
        self.y = y

    def __str__(self):
        x = self.x
        y = self.y
        op = self.op
        return '({}) {} ({})'.format(x, op, y)

class Choice(BinaryOperator):
    def __init__(self, x, y):
        super(Choice, self).__init__('+', x, y)

class Seq(BinaryOperator):
    def __init__(self, x, y):
        super(Seq, self).__init__('.', x, y)

class BoundedInit(BinaryOperator):
    def __init__(self, x, y):
        super(BoundedInit, self).__init__('<<', x, y)

class LeftMerge(BinaryOperator):
    def __init__(self, x, y):
        super(LeftMerge, self).__init__('||_', x, y)

class Merge(BinaryOperator):
    def __init__(self, x, y):
        super(Merge, self).__init__('||', x, y)

class Sync(BinaryOperator):
    def __init__(self, x, y):
        super(Sync, self).__init__('|', x, y)

class At(ProcessExpression):
    def __init__(self, x, t):
        self.x = x
        self.t = t

    def __str__(self):
        x = self.x
        t = self.t
        return '({}) @ ({})'.format(x, t)

class Allow(ProcessExpression):
    def __init__(self, V, x):
        self.V = V
        self.x = x

    def __str__(self):
        V = self.V
        x = self.x
        return 'allow({{{}}}, {})'.format(', '.join(map(str, V)), x)

class Block(ProcessExpression):
    def __init__(self, B, x):
        self.B = B
        self.x = x

    def __str__(self):
        B = self.B
        x = self.x
        return 'block({{{}}}, {})'.format(', '.join(B), x)

class Comm(ProcessExpression):
    def __init__(self, C, x):
        self.C = C
        self.x = x

    def __str__(self):
        C = self.C
        x = self.x
        return 'comm({{{}}}, {})'.format(', '.join(C), x)

class Hide(ProcessExpression):
    def __init__(self, I, x):
        self.I = I
        self.x = x

    def __str__(self):
        I = self.I
        x = self.x
        return 'hide({{{}}}, {})'.format(', '.join(I), x)

class Rename(ProcessExpression):
    def __init__(self, R, x):
        self.R = R
        self.x = x

    def __str__(self):
        x = self.x
        R = self.R
        return 'rename({{{}}}, {})'.format(', '.join(R), x)
