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

import copy
import random
import re
import random_data_expression
from process_expression import *
from data_expression import *

#---------------------------------------------------------------------------#
#           process library classes
#---------------------------------------------------------------------------#

class ProcessEquation:
    def __init__(self, lhs, rhs):
        self.lhs = lhs
        self.rhs = rhs

    def __str__(self):
        lhs = self.lhs
        rhs = self.rhs
        return '{} = {};'.format(lhs, rhs)

class ProcessSpecification:
    def __init__(self, actions, equations, init):
        self.actions = actions
        self.equations = equations
        self.init = init

    def __str__(self):
        actions = self.actions
        equations = self.equations
        init = self.init

        actspec = 'act\n  {};\n'.format(','.join(actions))
        procspec = 'proc\n  {}\n'.format('\n  '.join(map(str, equations)))
        initspec = 'init\n  {};'.format(str(init))
        return '{}\n{}\n{}'.format(actspec, procspec, initspec)

#---------------------------------------------------------------------------#
#           utility functions
#---------------------------------------------------------------------------#

def remove_postfix(name):
    return re.sub('\d+$', '', name)

# Sum variables have the format 'bN: Bool', with N in [1, 2, ...]
def make_variable(forbidden_variables):
    V = [x.name for x in forbidden_variables]
    for i in range(1, len(V) + 2):
        name = 'b{}'.format(i)
        if not name in V:
            return Variable(name, 'Bool')

def default_value(v):
    assert isinstance(v, Variable)
    if v.type == 'Bool':
        return random.choice([Boolean('true'), Boolean('false')])
    elif v.type == 'Int':
        return random.choice([Integer('0'), Integer('1'), Integer('2')])
    raise RuntimeError('default_value: only Bool and Int are supported! ' + str(v.type))

def make_multi_action1(actions, size):
    result = []
    for _ in range(size):
        result.append(random.choice(actions))
    return MultiAction(sorted(result))

# returns the size of a process expression
def expression_size(x):
    if isinstance(x, MultiAction):
        return 1
    result = 0
    if isinstance(x, ProcessExpression):
        result = result + 1
    for key, value in vars(x).items():
        if isinstance(value, ProcessExpression):
            result = result + expression_size(value)
    return result

# returns the generator functions that are capable of producing a random process expression
# that satisfies the given constraints
def select_generators(generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    result = []

    if size >= 1:
        result = result + [make_delta, make_tau]
        if len(actions) > 0:
            result = result + [make_action, make_multi_action]
        if len(process_identifiers) > 0 and not is_guarded:
            result = result + [make_process_instance]

    if size >= 2:
        result = result + [make_sum, make_if_then]

    if size >= 3:
        result = result + [make_if_then_else, make_choice, make_seq]

    r = []
    for x in result:
        r = r + [x] * generators[x]
    return r

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

#---------------------------------------------------------------------------#
#           generators for process expressions
#---------------------------------------------------------------------------#

# Generates a random process expression
# - actions: the set of action names that may be used (can be empty)
# - process_identifiers: the set of process identifiers that may be used (can be empty)
# - is_guarded: if True, the returned process expression must be guarded
# - is_pcrl: if True, the returned process expression must be a pCRL expression
# - size: the maximal size of the returned process expression, where size is defined
#   as 1 + the sum of the sizes of the children
def make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    generators = select_generators(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size)
    generator = random.choice(generators)
    result = generator(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size)
    if expression_size(result) > size:
        raise RuntimeError('The generated expression has the wrong size! ' + str(result))
    assert 1 <= expression_size(result) <= size
    return result

def make_two_process_expressions(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    assert size >= 2
    n = random.randint(1, size - 1)
    x = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, n)
    y = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size - expression_size(x))
    return x, y

def make_action(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    a = random.choice(actions)
    return Action(a)

def make_multi_action(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    n = random.randint(2, 4)
    result = []
    for _ in range(n):
        result.append(random.choice(actions))
    return MultiAction(sorted(result))

def make_delta(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    return Delta()

def make_tau(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    return Tau()

def make_process_instance(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    P = random.choice(process_identifiers)
    parameters = []
    for v in P.variables:
        if v.type == 'Bool':
            parameters.append(random_data_expression.make_boolean_data_expression(variables))
        elif v.type == 'Int':
            parameters.append(random_data_expression.make_integer_data_expression(variables))
        else:
            raise RuntimeError('unknown type {}'.format(v.type))
    return ProcessInstance(P, parameters)

def make_sum(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    d = make_variable(variables)
    x = make_process_expression(process_expression_generators, actions, process_identifiers, variables + [d], is_pcrl, is_guarded, size - 1)
    return Sum(d, x)

def make_if_then(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    c = 'true'
    x = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size - 1)
    return IfThen(c, x)

def make_if_then_else(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    c = 'true'
    x, y = make_two_process_expressions(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size - 1)
    return IfThenElse(c, x, y)

def make_choice(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    x, y = make_two_process_expressions(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size - 1)
    return Choice(x, y)

def make_seq(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    x = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size - 2)
    y = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, False, size - expression_size(x) - 1)
    return Seq(x, y)

def make_sync(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    x = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size - 2)
    y = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, False, size - expression_size(x) - 1)
    return Sync(x, y)

def make_left_merge(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    x = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size - 2)
    y = make_process_expression(process_expression_generators, actions, process_identifiers, variables, is_pcrl, False, size - expression_size(x) - 1)
    return LeftMerge(x, y)

def make_bounded_init(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size):
    x, y = make_two_process_expressions(process_expression_generators, actions, process_identifiers, variables, is_pcrl, is_guarded, size - 1)
    return BoundedInit(x, y)

def make_allow_set(actions, size):
    result = set([])
    for _ in range(size):
        n = random.randint(1, 2)
        alpha = make_multi_action1(actions, n)
        result.add(alpha)
    return list(result)

def make_allow(actions, x):
    V = make_allow_set(actions, 5)
    return Allow(V, x)

def make_block_set(actions, size):
    return random.sample(actions, size)

def make_block(actions, x):
    B = make_block_set(actions, 1)
    return Block(B, x)

def make_comm_set(actions, size):
    result = []
    A = set(actions)
    for _ in range(size):
        if len(A) < 2:
            break
        a = random.choice(list(A))
        b = random.choice(list(A))
        A.remove(a)
        if a != b:
            A.remove(b)
        c = random.choice(list(A))
        result.append('{} | {} -> {}'.format(a, b, c))
    return result

def make_comm(actions, x):
    C = make_comm_set(actions, 1)
    return Comm(C, x)

def make_hide_set(actions, size):
    if size > len(actions):
        return actions
    return random.sample(actions, size)

def make_hide(actions, x):
    I = make_hide_set(actions, 1)
    return Hide(I, x)

def make_rename_set(actions, size):
    result = []
    A = copy.deepcopy(actions)
    for _ in range(size):
        if len(A) < 2:
            break
        a, b = random.sample(A, 2)
        A.remove(a)
        A.remove(b)
        result.append('{} -> {}'.format(a, b))
    return result

def make_rename(actions, x):
    R = make_rename_set(actions, 1)
    return Rename(R, x)

#---------------------------------------------------------------------------#
#                       make_process_specification
#---------------------------------------------------------------------------#

# Generates a random process expression by wrapping parallel operators around elements of process_expressions
# - size: the number of parallel operators that are used
def make_parallel_process_expression(actions, process_expressions, size, parallel_operator_generators = [make_block, make_hide, make_rename, make_comm, make_allow]):
    # 1) join the process expressions using merge / left_merge / sync
    V = copy.deepcopy(process_expressions)
    x = None
    while len(V) > 1 or size > 0:
        if random.choice([True, False]) and len(V) > 1:
            p, q = random.sample(V, 2)
            V.remove(p)
            V.remove(q)
            V.append(Merge(p, q))
        elif size > 0:
            p = random.choice(V)
            V.remove(p)
            f = random.choice(parallel_operator_generators)
            V.append(f(actions, p))
            size = size - 1
    return V[0]

# Generates a random process expression by wrapping parallel operators around elements of process_expressions
# - size: the number of parallel operators that are used
def make_parallel_process_expression_old(actions, process_expressions, size, parallel_operator_generators = [make_block, make_hide, make_rename, make_comm, make_allow]):
    V = copy.deepcopy(process_expressions)
    x = None
    while len(V) > 0:
        if x == None:
            p, q = random.sample(V, 2)
            V.remove(p)
            V.remove(q)
            x = Merge(p, q)
        else:
            p = random.choice(V)
            V.remove(p)
            x = Merge(x, p)

    # 2) wrap size parallel operators around x
    for _ in range(size):
        f = random.choice(parallel_operator_generators)
        x = f(actions, x)

    return x

# Generators for random process expressions, and their frequency.
default_process_expression_generators = {
    make_action          : 8,
    make_delta           : 1,
    make_tau             : 1,
    make_process_instance: 2,
    make_sum             : 2,
    make_if_then         : 2,
    make_if_then_else    : 2,
    make_choice          : 5,
    make_seq             : 5,
    make_multi_action    : 1,
}

default_parallel_operator_generators = [make_block, make_hide, make_rename, make_comm, make_allow]

# Generates a random process specification
# - actions: the set of action names that may be used (can be empty)
# - process_identifiers: the set of process identifiers that may be used (can be empty)
# - size: the maximal size of process expressions in the specifation
# - process_expression_generators: a mapping containing generators for process expressions
# - parallel_process_expression_generators: generators for wrapping process expressions inside parallel operators
# - init: the initial state of the result. If it is equal to None, it will be generated.
# - generate_process_parameters: determines if the process identifiers have parameters
def make_process_specification(parallel_operator_generators = default_parallel_operator_generators,
                               process_expression_generators = default_process_expression_generators,
                               actions = ['a', 'b', 'c', 'd'],
                               process_identifiers = ['P', 'Q', 'R'],
                               size = 13,
                               init = None,
                               generate_process_parameters = False
                              ):

    # create process identifiers for the equations
    process_identifiers = list(map(ProcessIdentifier, process_identifiers))
    if generate_process_parameters:
        V = parse_variables('c1: Bool, c2: Bool, c3: Bool, i1: Int, i2: Int, i3: Int')
        for i, P in enumerate(process_identifiers):
            if not P.variables:
                n = random.randint(0, 3)
                process_identifiers[i].variables = random.sample(V, n)

    is_guarded = True
    variables = []
    is_pcrl = True
    equations = []
    for P in process_identifiers:
        x = make_process_expression(process_expression_generators, actions, process_identifiers, variables + P.variables, is_pcrl, is_guarded, size)
        equations.append(ProcessEquation(P, x))
    n = random.randint(0, 5)
    if not init:
        process_instances = [ProcessInstance(x, map(default_value, x.variables)) for x in process_identifiers]
        init = make_parallel_process_expression(actions, process_instances, n, parallel_operator_generators)
    return ProcessSpecification(list(set(actions)), equations, init)

if __name__ == '__main__':
    procspec = make_process_specification(generate_process_parameters = True)
    print procspec
