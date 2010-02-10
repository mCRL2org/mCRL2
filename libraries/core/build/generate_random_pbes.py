#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import random
from path import *

# As a convention we use that k, m, n are always natural numbers and
# b, c, d are always booleans.

# Adds a type ':Bool' or ':Nat' to the name of a variable
def add_type(var):
    if var in ['b', 'c', 'd']:
        return '%s:Bool' % var
    elif var in ['k', 'm', 'n']:
        return '%s:Nat' % var
    print 'VAR', var
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

class pbes:
    def __init__(self, equations, init):
        self.equations = equations
        self.init = init

    def __repr__(self):
        return 'pbes\n%s\n\ninit %s;' % ('\n'.join(map(str, self.equations)), self.init)

    def finish(self):
        for e in self.equations:
            e.finish()

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
        if self.x in freevars:
            self.y.finish(freevars, negated)
        else:
            self.y.finish(freevars + [self.x], negated)

def not_(x):
    return unary_operator('!', x)

def and_(x, y):
    return binary_operator('&&', x, y)

def or_(x, y):
    return binary_operator('||', x, y)

def implies(x, y):
    return binary_operator('=>', x, y)

def forall(x):
    var, dummy = pick_element(['k', 'm', 'n'])
    phi = or_(bool('val(%s < 3)' % var), x)
    return quantifier('forall', var, phi)

def exists(x):
    var, dummy = pick_element(['k', 'm', 'n'])
    phi = or_(bool('val(%s < 3)' % var), x)
    return quantifier('exists', var, phi)

def equal_to(x, y):
    return binary_operator('==', x, y)

def not_equal_to(x, y):
    return binary_operator('!=', x, y)

#operators = [not_, forall, exists, and_, or_, implies, equal_to, not_equal_to]
operators = [not_, and_, or_, implies, forall, exists]

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
    variables = ['b', 'c', 'm', 'n']
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
    naturals = set(freevars).intersection(set(['k', 'm', 'n']))
    booleans = set(freevars).intersection(set(['b', 'c', 'd']))
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
    naturals = set(freevars).intersection(set(['k', 'm', 'n']))
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
            if a in ['b', 'c', 'd']:
                args.append(bool())
            elif a in ['k', 'm', 'n']:
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

def make_pbes(n, atom_count = 5, propvar_count = 3):
    predvars = make_predvars(n)
    equations = []
    for i in range(n):
        terms = make_terms(predvars, atom_count, propvar_count)
        while len(terms) > 1:
            terms = join_terms(terms)
        sigma, dummy = pick_element(['mu', 'nu'])
        equations.append(equation(sigma, predvars[i], terms[0]))
    X = predvars[0]
    args = []
    for a in X.args:
        if a in ['b', 'c', 'd']:
            args.append('true')
        elif a in ['k', 'm', 'n']:
            args.append('0')
    init = propvar(X.name, args)
    p = pbes(equations, init)
    p.finish()
    return p

print make_pbes(4, 5, 2)