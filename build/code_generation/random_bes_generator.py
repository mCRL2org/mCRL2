#!/usr/bin/env python

#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import random
from path import *

class predvar:
    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return self.name

class equation:
    def __init__(self, sigma, var, formula):
        self.sigma = sigma
        self.var = var
        self.formula = formula

    def __repr__(self):
        return '%s %s = %s;' % (self.sigma, self.var, self.formula)

class bes:
    def __init__(self, equations, init):
        self.equations = equations
        self.init = init

    def __repr__(self):
        # Note: for the moment we use pbes instead of bes, since there is no parser for BESs
        return 'pbes\n%s\n\ninit %s;' % ('\n'.join(map(str, self.equations)), self.init)

class unary_operator:
    def __init__(self, op, x):
        self.op = op
        self.x = x

    def __repr__(self):
        x = self.x
        op = self.op
        return '%s(%s)' % (op, x)

class binary_operator:
    def __init__(self, op, x, y):
        self.op = op
        self.x = x
        self.y = y

    def __repr__(self):
        x = self.x
        y = self.y
        op = self.op
        return '(%s) %s (%s)' % (x, op, y)

def not_(x):
    return unary_operator('!', x)

def and_(x, y):
    return binary_operator('&&', x, y)

def or_(x, y):
    return binary_operator('||', x, y)

def implies(x, y):
    return binary_operator('=>', x, y)

operators = [implies, not_, and_, or_]
operators = [and_, or_]

def is_unary(op):
    return op in [not_]

# pick a random element x from a set s
# returns x, (s - {x})
def pick_element(s):
    n = random.randint(0, len(s) - 1)
    x = s[n]
    s = s[:n] + s[n+1:]
    return x, s

# randomly pick n elements from a set s
# returns a sequence with the selected elements
def pick_elements(s, n):
    result = []
    for i in range(n):
        x, s = pick_element(s)
        result.append(x)
    return result

# Creates n terms
def make_terms(predvars, n):
    result = []
    for i in range(n):
        result.append(predvars[random.randint(0, len(predvars) - 1)])
    return result

def join_terms(terms):
    op = operators[random.randint(0, len(operators) - 1)]
    if is_unary(op):
        x, terms = pick_element(terms)
        z = op(x)
    else:
        x, terms = pick_element(terms)
        y, terms = pick_element(terms)
        z = op(x, y)
    terms.append(z)
    return terms

def make_bes(equation_count, term_size = 3):
    predvars = [predvar('X%d' % i) for i in range(1, equation_count + 1)]
    equations = []
    for i in range(equation_count):
        terms = make_terms(predvars, random.randint(1, term_size))
        while len(terms) > 1:
            terms = join_terms(terms)
        sigma, dummy = pick_element(['mu', 'nu'])
        equations.append(equation(sigma, predvars[i], terms[0]))
    init = predvars[0]
    return bes(equations, init)

if __name__ == "__main__":
    b = make_bes(4, 5)
    print '%s' % b
