#!/usr/bin/env python

#~ Copyright 2012 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

#--------------------------------------------------
# expression                  | #children  | pcrl |
#--------------------------------------------------
# action                      |     0      |  y   |
# delta                       |     0      |  y   |
# tau                         |     0      |  y   |
# process_instance            |     0      |  y   |
# sum                         |     1      |  y   |
# if_then                     |     1      |  y   |
# if_then_else                |     2      |  y   |
# choice                      |     2      |  y   |
# seq                         |     2      |  y   |
# block                       |     1      |  n   |
# hide                        |     1      |  n   |
# rename                      |     1      |  n   |
# comm                        |     1      |  n   |
# allow                       |     1      |  n   |
# sync                        |     2      |  n   |
# merge                       |     2      |  n   |
# left_merge                  |     2      |  n   |
#--------------------------------------------------
# unsupported                                     |
#--------------------------------------------------
# process_instance_assignment |     0      |  y   |
# at                          |     1      |  y   |
# binit                       |     2      |  y   |

import copy
import random

ACTIONS = ['a', 'b', 'c', 'd']
PROCESS_IDENTIFIERS = ['P', 'Q', 'R']

# sum variables have the format 'bN: Bool', with N in [1, 2, ...]
def make_variable(forbidden_variables):
    V = [int(x.name[1:]) for x in forbidden_variables]
    for i in range(1, len(V) + 2):
        if not i in V:
            return variable('b%d' % i, 'Bool')

class variable:
    def __init__(self, name, type = 'Bool'):
        self.name = name
        self.type = type

    def __repr__(self):
        return '%s: %s' % (self.name, self.type)

class multi_action:
    def __init__(self, actions):
        self.actions = actions

    def __eq__(self, other):
        if type(other) is type(self):
            return self.__dict__ == other.__dict__
        return False

    def __ne__(self, other):
        return not self.__eq__(other)

    def __repr__(self):
        if len(self.actions) == 0:
            return 'tau'
        return ' | '.join(self.actions)

    def __hash__(self):
        return hash(str(self))

def make_multi_action(actions, size):
    result = []
    for i in range(size):
        result.append(random.choice(actions))
    return multi_action(sorted(result))

# returns the size of a process expression
def expression_size(x):
    result = 0
    if isinstance(x, process_expression):
        result = result + 1
    for key, value in vars(x).items():
        if isinstance(value, process_expression):
            result = result + expression_size(value)
    return result

# collects process expressions of a given type that appear in x and its children
def collect_process_expressions(x, type, result):
    if isinstance(x, type):
        result.append(x)
    for key, value in vars(x).items():
        if isinstance(value, process_expression):
            collect_process_expressions(value, type, result)

# returns all sum variables that appear in the process expression x
def find_sum_variables(x):
    result = []
    collect_process_expressions(x, sum, result)
    return [x.d for x in result]

# returns all action names that appear in the process expression x
def find_actions(x):
    result = []
    collect_process_expressions(x, action, result)
    return [x.a for x in result]

# returns the generator functions that are capable of producing a random process expression
# that satisfies the given constraints
def select_generators(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    result = []

    if size >= 1:
        result = result + [make_delta, make_tau]
        if len(actions) > 0:
            result = result + [make_action]
        if len(process_identifiers) > 0 and not is_guarded:
            result = result + [make_process_instance]

    if size >= 2:
        result = result + [make_sum, make_if_then]

    if size >= 3:
        result = result + [make_if_then_else, make_choice, make_seq]

    r = []
    for x in result:
        r = r + [x] * generator_map[x]
    return r

class process_expression(object):
    pass

# generate a random process expression
# - actions: the set of action names that may be used (can be empty)
# - process_identifiers: the set of process identifiers that may be used (can be empty)
# - is_guarded: if True, the returned process expression must be guarded
# - is_pcrl: if True, the returned process expression must be a pCRL expression
# - size: the maximal size of the returned process expression, where size is defined
#   as 1 + the sum of the sizes of the children
def make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    generators = select_generators(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size)
    generator = random.choice(generators)
    result = generator(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size)
    if expression_size(result) > size:
        print 'foutje:', result, expression_size(result), size
    assert 1 <= expression_size(result) <= size
    return result

# returns 2 random process expressions
def make_process_expressions(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    assert size >= 2
    n = random.randint(1, size - 1)
    x = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, n)
    y = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - expression_size(x))
    return x, y

class action(process_expression):
    def __init__(self, a):
        self.a = a

    def __repr__(self):
        a = self.a
        return '%s' % a

# generate a random action
def make_action(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    a = random.choice(actions)
    return action(a)

class delta(process_expression):
    def __repr__(self):
        return 'delta'

# generate a random delta
def make_delta(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    return delta()

class tau(process_expression):
    def __repr__(self):
        return 'tau'

# generate a random tau
def make_tau(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    return tau()

class process_instance(process_expression):
    def __init__(self, P):
        self.P = P

    def __repr__(self):
        P = self.P
        return '%s' % P

# generate a random process instance
def make_process_instance(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    P = random.choice(process_identifiers)
    return process_instance(P)

class sum(process_expression):
    def __init__(self, d, x):
        self.d = d
        self.x = x

    def __repr__(self):
        d = self.d
        x = self.x
        return 'sum %s . (%s)' % (d, x)

# generate a random sum
def make_sum(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    d = make_variable(free_variables)
    x = make_process_expression(generator_map, actions, process_identifiers, free_variables + [d], is_pcrl, is_guarded, size - 1)
    return sum(d, x)

class if_then(process_expression):
    def __init__(self, c, x):
        self.x = x
        self.c = c

    def __repr__(self):
        x = self.x
        c = self.c
        return '(%s) -> (%s)' % (c, x)

# generate a random if_then
def make_if_then(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    c = 'true'
    x = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 1)
    return if_then(c, x)

class if_then_else(process_expression):
    def __init__(self, c, x, y):
        self.c = c
        self.x = x
        self.y = y

    def __repr__(self):
        c = self.c
        x = self.x
        y = self.y
        return '(%s) -> (%s) <> (%s)' % (c, x, y)

# generate a random if_then_else
def make_if_then_else(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    c = 'true'
    x, y = make_process_expressions(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 1)
    return if_then_else(c, x, y)

class binary_operator(process_expression):
    def __init__(self, op, x, y):
        self.op = op
        self.x = x
        self.y = y

    def __repr__(self):
        x = self.x
        y = self.y
        op = self.op
        return '(%s) %s (%s)' % (x, op, y)

class choice(binary_operator):
    def __init__(self, x, y):
        super(choice, self).__init__('+', x, y)

# generate a random choice
def make_choice(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    x, y = make_process_expressions(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 1)
    return choice(x, y)

class seq(binary_operator):
    def __init__(self, x, y):
        super(seq, self).__init__('.', x, y)

# generate a random seq
def make_seq(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    x = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 2)
    y = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, False, size - expression_size(x) - 1)
    return seq(x, y)

class bounded_init(binary_operator):
    def __init__(self, x, y):
        super(bounded_init, self).__init__('<<', x, y)

# generate a random bounded_init
def make_bounded_init(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    x, y = make_process_expressions(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 1)
    return bounded_init(x, y)

class left_merge(binary_operator):
    def __init__(self, x, y):
        super(left_merge, self).__init__('||_', x, y)

class merge(binary_operator):
    def __init__(self, x, y):
        super(merge, self).__init__('||', x, y)

class sync(binary_operator):
    def __init__(self, x, y):
        super(sync, self).__init__('|', x, y)

class at(process_expression):
    def __init__(self, x, t):
        self.x = x
        self.t = t

    def __repr__(self):
        x = self.x
        t = self.t
        return '(%s) @ (%s)' % (x, t)

class allow(process_expression):
    def __init__(self, V, x):
        self.V = V
        self.x = x

    def __repr__(self):
        V = self.V
        x = self.x
        return 'allow({%s}, %s)' % (', '.join(map(str, V)), x)

def make_allow_set(actions, size):
    result = set([])
    for i in range(size):
        n = random.randint(1, 2)
        alpha = make_multi_action(actions, n)
        result.add(alpha)
    return list(result)

# generate a random allow
def make_allow(actions, x):
    V = make_allow_set(actions, 5)
    return allow(V, x)

class block(process_expression):
    def __init__(self, B, x):
        self.B = B
        self.x = x

    def __repr__(self):
        B = self.B
        x = self.x
        return 'block({%s}, %s)' % (', '.join(B), x)

def make_block_set(actions, size):
    return random.sample(actions, size)

# generate a random block
def make_block(actions, x):
    B = make_block_set(actions, 1)
    return block(B, x)

class comm(process_expression):
    def __init__(self, C, x):
        self.C = C
        self.x = x

    def __repr__(self):
        C = self.C
        x = self.x
        return 'comm({%s}, %s)' % (', '.join(C), x)

def make_comm_set(actions, size):
    result = []
    A = set(actions)
    for i in range(size):
        if len(A) < 2:
            break
        a = random.choice(list(A))
        b = random.choice(list(A))
        A.remove(a)
        if a != b:
            A.remove(b)
        c = random.choice(list(A))
        result.append('%s | %s -> %s' % (a, b, c))
    return result

# generate a random comm
def make_comm(actions, x):
    C = make_comm_set(actions, 1)
    return comm(C, x)

class hide(process_expression):
    def __init__(self, I, x):
        self.I = I
        self.x = x

    def __repr__(self):
        I = self.I
        x = self.x
        return 'hide({%s}, %s)' % (', '.join(I), x)

# generate a random comm
def make_hide_set(actions, size):
    if size > len(actions):
        return actions
    return random.sample(actions, size)

# generate a random hide
def make_hide(actions, x):
    I = make_hide_set(actions, 1)
    return hide(I, x)

class rename(process_expression):
    def __init__(self, R, x):
        self.R = R
        self.x = x

    def __repr__(self):
        x = self.x
        R = self.R
        return 'rename({%s}, %s)' % (', '.join(R), x)

def make_rename_set(actions, size):
    result = []
    A = copy.deepcopy(actions)
    for i in range(size):
        if len(A) < 2:
            break
        a, b = random.sample(A, 2)
        A.remove(a)
        A.remove(b)
        result.append('%s -> %s' % (a, b))
    return result

# generate a random rename
def make_rename(actions, x):
    R = make_rename_set(actions, 1)
    return rename(R, x)

class process_equation:
    def __init__(self, lhs, rhs):
        self.lhs = lhs
        self.rhs = rhs

    def __repr__(self):
        lhs = self.lhs
        rhs = self.rhs
        return '%s = %s;' % (lhs, rhs)

class process_specification:
    def __init__(self, actions, equations, init):
        self.actions = actions
        self.equations = equations
        self.init = init

    def __repr__(self):
        actions = self.actions
        equations = self.equations
        init = self.init

        actspec = 'act\n  %s;\n' % ','.join(actions)
        procspec = 'proc\n  %s\n' % '\n  '.join(map(str, equations))
        initspec = 'init\n  %s;' % str(init)
        return '%s\n%s\n%s' % (actspec, procspec, initspec)

def make_parallel_expression(actions, process_expressions, size, parallel_operators = [make_block, make_hide, make_rename, make_comm, make_allow]):
    # 1) join the process expressions using merge / left_merge / sync
    V = copy.deepcopy(process_expressions)
    x = None
    while len(V) > 0:
        if x == None:
            p, q = random.sample(V, 2)
            V.remove(p)
            V.remove(q)
            x = merge(p, q)
        else:
            p = random.choice(V)
            V.remove(p)
            x = merge(x, p)

    # 2) wrap size parallel operators around x
    for i in range(size):
        f = random.choice(parallel_operators)
        x = f(actions, x)

    return x

# generate a random process specification
def make_process_specification(generator_map, actions, process_identifiers, size, parallel_operators = [make_block, make_hide, make_rename, make_comm, make_allow]):
    is_guarded = True
    free_variables = []
    is_pcrl = True
    equations = []
    for P in process_identifiers:
        x = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size)
        equations.append(process_equation(P, x))
    n = random.randint(0, 3)
    init = make_parallel_expression(actions, process_identifiers, n, parallel_operators)
    return process_specification(actions, equations, init)

generator_map = {
    make_action          : 8,
    make_delta           : 1,
    make_tau             : 1,
    make_process_instance: 2,
    make_sum             : 2,
    make_if_then         : 2,
    make_if_then_else    : 2,
    make_choice          : 5,
    make_seq             : 5,
}
