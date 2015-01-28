#!/usr/bin/env python

#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import random
from path import *
from mcrl2_tools import *

PREDICATE_INTEGERS = ['m', 'n']
PREDICATE_BOOLEANS = ['b', 'c']
QUANTIFIER_INTEGERS = ['t', 'u', 'v', 'w']
INTEGERS = PREDICATE_INTEGERS + QUANTIFIER_INTEGERS
BOOLEANS = PREDICATE_BOOLEANS

# As a convention we use that k, m, n are always natural numbers and
# b, c, d are always booleans.

# Adds a type ':Bool' or ':Nat' to the name of a variable
def add_type(var):
    if var in BOOLEANS:
        return '%s:Bool' % var
    elif var in INTEGERS:
        return '%s:Nat' % var
    return None

class bool:
    def __init__(self, value = None):
        self.value = value

    def __repr__(self):
        if self.value == None:
            return '<BOOL>'
        else:
            return '%s' % self.value

    def finish(self, freevars, negated, add_val = True):
        if self.value == None:
            self.value = make_boolean(freevars, add_val)

    # returns True if it has the value 'true' or 'false'
    def is_minimal(self):
        result = self.value in ['false', 'true']
        print '<M>', result, self, self.__class__
        return result

    def minimize(self, call_back, level):
        if level == 0 and not is_boolean_constant(self):
            value = self.value
            self.value = 'false'
            print '<CHANGE>', value, '->', self.value
            call_back()
            self.value = 'true'
            print '<CHANGE>', value, '->', self.value
            call_back()
            self.value = value

class nat:
    def __init__(self, value = None):
        self.value = value

    def __repr__(self):
        if self.value == None:
            return '<NAT>'
        else:
            return '%s' % self.value

    def finish(self, freevars, negated, add_val = True):
        if self.value == None:
            self.value = make_natural(freevars, add_val)

    # returns True if it has the value '0' or '1'
    def is_minimal(self):
        result = self.value in ['0', '1']
        print '<M>', result, self, self.__class__
        return result

    def minimize(self, call_back, level):
        if level == 0 and not is_natural_constant(self):
            value = self.value
            self.value = '0'
            print '<CHANGE>', value, '->', self.value
            call_back()
            self.value = '1'
            print '<CHANGE>', value, '->', self.value
            call_back()
            self.value = value

class propvar:
    def __init__(self, name, args):
        self.name = name
        self.args = args
        self.prefix = ''   # sometimes we need to add a '!' to make a pbes monotonic

    def __repr__(self):
        if len(self.args) == 0:
            return '%s%s' % (self.prefix, self.name)
        return '%s%s(%s)' % (self.prefix, self.name, ', '.join(map(str, self.args)))

    def finish(self, freevars, negated):
        if negated:
            self.prefix = '!'
        add_val = False
        for a in self.args:
            a.finish(freevars, negated, add_val)

    # returns True if all arguments are minimal
    def is_minimal(self):
        result = True
        for a in self.args:
            if not a.is_minimal():
                result = False
                break
        print '<M>', result, self, self.__class__
        return result

    def minimize(self, call_back, level):
        if level > 0:
            for a in self.args:
                a.minimize(call_back, level - 1)

class predvar:
    def __init__(self, name, args):
        self.name = name
        self.args = args

    def __repr__(self):
        if len(self.args) == 0:
            return self.name
        args = map(add_type, self.args)
        return '%s(%s)' % (self.name, ', '.join(map(str, args)))

    def finish(self, freevars, negated):
        pass

class equation:
    def __init__(self, sigma, var, formula):
        self.sigma = sigma
        self.var = var
        self.formula = formula

    def __repr__(self):
        return '%s %s = %s;' % (self.sigma, self.var, self.formula)

    def finish(self):
        freevars = self.var.args
        negated = False
        self.formula.finish(freevars, negated)

    # returns True if the right hand side is minimal
    def is_minimal(self):
        result = is_boolean_constant(self.formula)
        print '<M>', result, self, self.__class__
        return result

    def minimize(self, call_back, level):
        if level == 0:
            if not is_boolean_constant(self.formula):
                formula = self.formula
                self.formula = bool('true')
                print '<CHANGE>', formula, '->', self.formula
                call_back()
                self.formula = formula
        else:
            self.formula.minimize(call_back, level - 1)

class pbes:
    def __init__(self, equations, init):
        self.equations = equations
        self.init = init

    def __repr__(self):
        return 'pbes\n%s\n\ninit %s;' % ('\n'.join(map(str, self.equations)), self.init)

    def finish(self):
        for e in self.equations:
            e.finish()

    # returns True if all equations are minimal
    def is_minimal(self):
        result = True
        for e in self.equations:
            if not e.is_minimal():
                result = False
                break
        print '<M>', result, self, self.__class__
        return result

    def minimize(self, call_back, level):
        for e in self.equations:
            e.minimize(call_back, level - 1)

class unary_operator:
    def __init__(self, op, x):
        self.op = op
        self.x = x

    def __repr__(self):
        x = self.x
        op = self.op
        return '%s(%s)' % (op, x)

    def finish(self, freevars, negated):
        if self.op == '!':
            negated = not negated
        self.x.finish(freevars, negated)

    # returns True if the argument is minimal
    def is_minimal(self):
        result = self.x.is_minimal() and isinstance(self.x, bool)
        print '<M>', result, self, self.__class__
        return result

    # minimizes the argument
    def minimize(self, call_back, level):
        if level == 0:
            if not is_boolean_constant(self.x):
                x = self.x
                self.x = bool('true')
                print '<CHANGE>', x, '->', self.x
                call_back()
                self.x = bool('false')
                print '<CHANGE>', x, '->', self.x
                call_back()
                self.x = x
        else:
            self.x.minimize(call_back, level - 1)

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

    def finish(self, freevars, negated):
        if self.op == '=>':
            self.x.finish(freevars, not negated)
        else:
            self.x.finish(freevars, negated)
        self.y.finish(freevars, negated)

    # returns True if both arguments are minimal
    def is_minimal(self):
        result = is_boolean_constant(self.x) and is_boolean_constant(self.y)
        print '<M>', result, self, self.__class__
        return result

    # randomly minimizes one of the arguments
    def minimize(self, call_back, level):
        if level == 0:
            if not is_boolean_constant(self.x):
                x = self.x
                self.x = bool('true')
                print '<CHANGE>', x, '->', self.x
                call_back()
                self.x = bool('false')
                print '<CHANGE>', x, '->', self.x
                call_back()
                self.x = x
            if not is_boolean_constant(self.y):
                y = self.y
                self.y = bool('true')
                print '<CHANGE>', y, '->', self.y
                call_back()
                self.y = bool('false')
                print '<CHANGE>', y, '->', self.y
                call_back()
                self.y = y
        else:
            self.x.minimize(call_back, level - 1)
            self.y.minimize(call_back, level - 1)

class quantifier:
    def __init__(self, quantor, x, y):
        self.quantor = quantor
        self.x = x    # the bound variable
        self.y = y    # the formula

    def __repr__(self):
        x = self.x
        y = self.y
        quantor = self.quantor
        return '%s %s.(%s)' % (quantor, add_type(x), y)

    def finish(self, freevars, negated):
        qvar = []
        for q in QUANTIFIER_INTEGERS:
            if not q in freevars:
                qvar.append(q)
        if len(qvar) == 0:
            raise RuntimeError('warning: quantifier nesting depth exceeded')
        var, dummy = pick_element(qvar)
        self.x = var
        if self.quantor == 'exists':
            self.y = or_(bool('val(%s < 3)' % var), self.y)
        else:
            self.y = and_(bool('val(%s < 3)' % var), self.y)
        self.y.finish(freevars + [self.x], negated)

    # returns True if the formula is minimal
    def is_minimal(self):
        result = self.y.is_minimal() and isinstance(self.y, bool)
        print '<M>', result, self, self.__class__
        return result

    # minimizes the formula
    def minimize(self, call_back, level):
        if level == 0:
            if not is_boolean_constant(self.y):
                y = self.y
                self.y = bool('true')
                print '<CHANGE>', y, '->', self.y
                call_back()
                self.y = bool('false')
                print '<CHANGE>', y, '->', self.y
                call_back()
                self.y = y
        else:
            self.y.minimize(call_back, level - 1)

def not_(x):
    return unary_operator('!', x)

def and_(x, y):
    return binary_operator('&&', x, y)

def or_(x, y):
    return binary_operator('||', x, y)

def implies(x, y):
    return binary_operator('=>', x, y)

def forall(x):
    var = nat()
    phi = x
    return quantifier('forall', var, phi)

def exists(x):
    var = nat()
    phi = x
    return quantifier('exists', var, phi)

def equal_to(x, y):
    return binary_operator('==', x, y)

def not_equal_to(x, y):
    return binary_operator('!=', x, y)

#operators = [not_, forall, exists, and_, or_, implies, equal_to, not_equal_to]
operators = [not_, and_, or_, implies, forall, exists]

def is_boolean_constant(x):
    return isinstance(x, bool) and x.value in ['false', 'true']

def is_natural_constant(x):
    return isinstance(x, nat) and x.value in ['0', '1']

def is_unary(op):
    return op in [not_, forall, exists]

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

def make_predvar(n, size = random.randint(0, 2)):
    name = 'X%d' % n
    arguments = []
    variables = PREDICATE_INTEGERS + PREDICATE_BOOLEANS
    for i in range(size):
        v, variables = pick_element(variables)
        arguments.append(v)
    return predvar(name, arguments)

# Generates n random predicate variables with 0, 1 or 2 parameters
def make_predvars(n):
    result = []
    for i in range(n):
        result.append(make_predvar(i, random.randint(0, 2)))
    return result

# Creates elementary random boolean terms, with free variables
# from the set freevars.
def make_atoms(freevars, add_val = True):
    naturals = set(freevars).intersection(set(INTEGERS))
    booleans = set(freevars).intersection(set(BOOLEANS))
    result = []
    for m in naturals:
        result.append('%s > 0' % m)
        result.append('%s > 1' % m)
        result.append('%s < 2' % m)
        result.append('%s < 3' % m)
        for n in naturals - set([m]):
            result.append('%s == %s' % (m, n))
    for b in booleans:
        result.append(b)
    result.append('true')
    result.append('false')
    if add_val:
        result = ['val(%s)' % x for x in result]
    return result

def make_boolean(freevars, add_val = True):
    atoms = make_atoms(freevars, add_val)
    x, dummy = pick_element(atoms)
    return x

def make_natural(freevars, add_val = True):
    naturals = set(freevars).intersection(set(INTEGERS))
    result = []
    result.append('0')   
    result.append('1')
    for m in naturals:
        result.append('%s + 1' % m)
    x, dummy = pick_element(result)
    return x

# returns instantiations of predicate variables
def make_predvar_instantiations(predvars):
    result = []
    for X in predvars:
        args = []
        for a in X.args:
            if a in BOOLEANS:
                args.append(bool())
            elif a in INTEGERS:
                args.append(nat())
        result.append(propvar(X.name, args))
    return result

# Creates m boolean terms, and n propositional variable instantiations.
def make_terms(predvars, m, n):
    result = []
    for i in range(m):
        result.append(bool())
    inst  = make_predvar_instantiations(predvars)
    result = result + pick_elements(inst, n)
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

def make_pbes(equation_count, atom_count = 5, propvar_count = 3, use_quantifiers = True):
    global operators
    if use_quantifiers:
      operators = [not_, and_, or_, implies, not_, and_, or_, implies, forall, exists]
    else:
      operators = [not_, and_, or_, implies]
    while True:
        try:
            predvars = make_predvars(equation_count)
            equations = []
            for i in range(equation_count):
                terms = make_terms(predvars, atom_count, propvar_count)
                while len(terms) > 1:
                    terms = join_terms(terms)
                sigma, dummy = pick_element(['mu', 'nu'])
                equations.append(equation(sigma, predvars[i], terms[0]))
            X = predvars[0]
            args = []
            for a in X.args:
                if a in BOOLEANS:
                    args.append('true')
                elif a in INTEGERS:
                    args.append('0')
            init = propvar(X.name, args)
            p = pbes(equations, init)
            p.finish()
            return p
        except Exception as inst:
            print inst

class CounterExampleMinimizer:
    def __init__(self, pbes, solver, name):
        self.pbes = pbes
        self.solver = solver
        self.name = name
        self.counter = 0

    def save_counter_example(self):
        txtfile = '%s_counter_example_%d.txt' % (self.name, self.counter)
        pbesfile = '%s_counter_example_%d.pbes' % (self.name, self.counter)
        path(txtfile).write_text(str(self.pbes))
        run_txt2pbes(txtfile, pbesfile)
        text = run_pbespp(pbesfile)
        path(txtfile).write_text(text)       
        print '--- %s counter example ---' % self.name
        print text
        self.counter = self.counter + 1

    def callback(self):
        if not self.solver(self.pbes):
            raise Exception('test failed')
    
    def minimize(self):
        self.save_counter_example()
        stop = False
        while not stop:
            stop = True
            for level in range(1, 7):
                print '--- LEVEL %d ---' % level
                try:
                    self.pbes.minimize(self.callback, level)
                except Exception:
                    self.save_counter_example()
                    stop = False
                    continue
