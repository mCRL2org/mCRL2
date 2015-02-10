#!/usr/bin/env python

#~ Copyright 2012 Wieger Wesselink.
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

def remove_postfix(name):
    return re.sub('\d+$', '', name)

# Sum variables have the format 'bN: Bool', with N in [1, 2, ...]
def make_variable(forbidden_variables):
    V = [x.name for x in forbidden_variables]
    for i in range(1, len(V) + 2):
        name = 'b{}'.format(i)
        if not name in V:
            return Variable(name, 'Bool')

class Variable:
    def __init__(self, name, type = 'Bool'):
        self.name = name
        self.type = type

    def __str__(self):
        return '%s: %s' % (self.name, self.type)

class MultiAction:
    def __init__(self, actions):
        self.actions = actions

    def __eq__(self, other):
        if type(other) is type(self):
            return self.__dict__ == other.__dict__
        return False

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        if len(self.actions) == 0:
            return 'Tau'
        return ' | '.join(self.actions)

    def __hash__(self):
        return hash(str(self))

# creates random booleans
def make_booleans(free_variables):
    integers = set([x.name for x in free_variables if x.type == 'Int'])
    booleans = set([x.name for x in free_variables if x.type == 'Bool'])
    result = []
    for m in integers:
        result.append('{} > 0'.format(m))
        result.append('{} > 1'.format(m))
        result.append('{} < 2'.format(m))
        result.append('{} < 3'.format(m))
        for n in integers - set([m]):
            result.append('{} == {}'.format(m, n))
    for b in booleans:
        result.append(b)
    result.append('true')
    result.append('false')
    return result

# creates random integers
def make_integers(free_variables):
    integers = set([x.name for x in free_variables if x.type == 'Int'])
    result = []
    for m in integers:
        result.append(m)
        for n in integers - set([m]) | set(['1', '2']):
            result.append('%s + %s' % (m, n))
            result.append('%s - %s' % (m, n))
    result.append('0')
    result.append('1')
    return result

def default_value(v):
    assert isinstance(v, Variable)
    if v.type == 'Bool':
        return random.choice(['true', 'false'])
    elif v.type == 'Int':
        return random.choice(['0', '1', '2'])
    raise RuntimeError('default_value: only Bool and Int are supported! ' + str(v.type))

# returns a random data expression for variable v
def make_data_expression(v, free_variables):
    assert isinstance(v, Variable)
    if v.type == 'Bool':
        return random.choice(make_booleans(free_variables))
    elif v.type == 'Int':
        return random.choice(make_integers(free_variables))
    raise RuntimeError('make_data_expression: only Bool and Int are supported!')

def make_multi_action(actions, size):
    result = []
    for _ in range(size):
        result.append(random.choice(actions))
    return MultiAction(sorted(result))

# returns the size of a process expression
def expression_size(x):
    result = 0
    if isinstance(x, ProcessExpression):
        result = result + 1
    for key, value in vars(x).items():
        if isinstance(value, ProcessExpression):
            result = result + expression_size(value)
    return result

# collects process expressions of a given type that appear in x and its children
def collect_process_expressions(x, type, result):
    if isinstance(x, type):
        result.append(x)
    for key, value in vars(x).items():
        if isinstance(value, ProcessExpression):
            collect_process_expressions(value, type, result)

# returns all sum variables that appear in the process expression x
def find_sum_variables(x):
    result = []
    collect_process_expressions(x, Sum, result)
    return [x.d for x in result]

# returns all action names that appear in the process expression x
def find_actions(x):
    result = []
    collect_process_expressions(x, Action, result)
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

# example: 'b: Bool'
def parse_variable(text):
    text = text.strip()
    m = re.match('([^,:]+)\s*\:(.+)', text)
    result = Variable(m.group(1).strip(), m.group(2).strip())
    return result

# example: 'b: Bool, m: Nat'
def parse_variables(text):
    import string
    variables = filter(None, map(string.strip, text.split(',')))
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
        raise RuntimeError('The generated expression has the wrong size! ' + str(result))
    assert 1 <= expression_size(result) <= size
    return result

# returns 2 random process expressions
def make_process_expressions(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    assert size >= 2
    n = random.randint(1, size - 1)
    x = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, n)
    y = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - expression_size(x))
    return x, y

class Action(ProcessExpression):
    def __init__(self, a):
        self.a = a

    def __str__(self):
        a = self.a
        return '%s' % a

# generate a random Action
def make_action(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    a = random.choice(actions)
    return Action(a)

class Delta(ProcessExpression):
    def __str__(self):
        return 'delta'

# generate a random delta
def make_delta(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    return Delta()

class Tau(ProcessExpression):
    def __str__(self):
        return 'tau'

# generate a random tau
def make_tau(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    return Tau()

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

# generate a random process instance
def make_process_instance(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    P = random.choice(process_identifiers)
    parameters = [make_data_expression(x, free_variables) for x in P.variables]
    return ProcessInstance(P, parameters)

class Sum(ProcessExpression):
    def __init__(self, d, x):
        self.d = d
        self.x = x

    def __str__(self):
        d = self.d
        x = self.x
        return 'sum %s . (%s)' % (d, x)

# generate a random sum
def make_sum(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    d = make_variable(free_variables)
    x = make_process_expression(generator_map, actions, process_identifiers, free_variables + [d], is_pcrl, is_guarded, size - 1)
    return Sum(d, x)

class IfThen(ProcessExpression):
    def __init__(self, c, x):
        self.x = x
        self.c = c

    def __str__(self):
        x = self.x
        c = self.c
        return '(%s) -> (%s)' % (c, x)

# generate a random if_then
def make_if_then(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    c = 'true'
    x = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 1)
    return IfThen(c, x)

class IfThenElse(ProcessExpression):
    def __init__(self, c, x, y):
        self.c = c
        self.x = x
        self.y = y

    def __str__(self):
        c = self.c
        x = self.x
        y = self.y
        return '(%s) -> (%s) <> (%s)' % (c, x, y)

# generate a random if_then_else
def make_if_then_else(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    c = 'true'
    x, y = make_process_expressions(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 1)
    return IfThenElse(c, x, y)

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

# generate a random choice
def make_choice(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    x, y = make_process_expressions(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 1)
    return Choice(x, y)

class Seq(BinaryOperator):
    def __init__(self, x, y):
        super(Seq, self).__init__('.', x, y)

# generate a random seq
def make_seq(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    x = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 2)
    y = make_process_expression(generator_map, actions, process_identifiers, free_variables, is_pcrl, False, size - expression_size(x) - 1)
    return Seq(x, y)

class BoundedInit(BinaryOperator):
    def __init__(self, x, y):
        super(BoundedInit, self).__init__('<<', x, y)

# generate a random bounded_init
def make_bounded_init(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size):
    x, y = make_process_expressions(generator_map, actions, process_identifiers, free_variables, is_pcrl, is_guarded, size - 1)
    return BoundedInit(x, y)

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
        return '(%s) @ (%s)' % (x, t)

class Allow(ProcessExpression):
    def __init__(self, V, x):
        self.V = V
        self.x = x

    def __str__(self):
        V = self.V
        x = self.x
        return 'allow({%s}, %s)' % (', '.join(map(str, V)), x)

def make_allow_set(actions, size):
    result = set([])
    for _ in range(size):
        n = random.randint(1, 2)
        alpha = make_multi_action(actions, n)
        result.add(alpha)
    return list(result)

# generate a random allow
def make_allow(actions, x):
    V = make_allow_set(actions, 5)
    return Allow(V, x)

class Block(ProcessExpression):
    def __init__(self, B, x):
        self.B = B
        self.x = x

    def __str__(self):
        B = self.B
        x = self.x
        return 'block({%s}, %s)' % (', '.join(B), x)

def make_block_set(actions, size):
    return random.sample(actions, size)

# generate a random block
def make_block(actions, x):
    B = make_block_set(actions, 1)
    return Block(B, x)

class Comm(ProcessExpression):
    def __init__(self, C, x):
        self.C = C
        self.x = x

    def __str__(self):
        C = self.C
        x = self.x
        return 'comm({%s}, %s)' % (', '.join(C), x)

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
        result.append('%s | %s -> %s' % (a, b, c))
    return result

# generate a random comm
def make_comm(actions, x):
    C = make_comm_set(actions, 1)
    return Comm(C, x)

class Hide(ProcessExpression):
    def __init__(self, I, x):
        self.I = I
        self.x = x

    def __str__(self):
        I = self.I
        x = self.x
        return 'hide({%s}, %s)' % (', '.join(I), x)

# generate a random hide set
def make_hide_set(actions, size):
    if size > len(actions):
        return actions
    return random.sample(actions, size)

# generate a random hide
def make_hide(actions, x):
    I = make_hide_set(actions, 1)
    return Hide(I, x)

class Rename(ProcessExpression):
    def __init__(self, R, x):
        self.R = R
        self.x = x

    def __str__(self):
        x = self.x
        R = self.R
        return 'rename({%s}, %s)' % (', '.join(R), x)

def make_rename_set(actions, size):
    result = []
    A = copy.deepcopy(actions)
    for _ in range(size):
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
    return Rename(R, x)

class ProcessEquation:
    def __init__(self, lhs, rhs):
        self.lhs = lhs
        self.rhs = rhs

    def __str__(self):
        lhs = self.lhs
        rhs = self.rhs
        return '%s = %s;' % (lhs, rhs)

class ProcessSpecification:
    def __init__(self, actions, equations, init):
        self.actions = actions
        self.equations = equations
        self.init = init

    def __str__(self):
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
            x = Merge(p, q)
        else:
            p = random.choice(V)
            V.remove(p)
            x = Merge(x, p)

    # 2) wrap size parallel operators around x
    for _ in range(size):
        f = random.choice(parallel_operators)
        x = f(actions, x)

    return x

# generate a random process specification
def make_process_specification(generator_map, actions, process_identifiers, size, parallel_operators = [make_block, make_hide, make_rename, make_comm, make_allow], \
                               init = None, generate_process_parameters = False):

    # create process identifiers for the equations
    process_identifiers = map(ProcessIdentifier, process_identifiers)
    if generate_process_parameters:
        V = parse_variables('c1: Bool, c2: Bool, c3: Bool, i1: Int, i2: Int, i3: Int')
        for i, P in enumerate(process_identifiers):
            if not P.variables:
                n = random.randint(0, 3)
                process_identifiers[i].variables = random.sample(V, n)

    is_guarded = True
    free_variables = []
    is_pcrl = True
    equations = []
    for P in process_identifiers:
        x = make_process_expression(generator_map, actions, process_identifiers, free_variables + P.variables, is_pcrl, is_guarded, size)
        equations.append(ProcessEquation(P, x))
    n = random.randint(0, 3)
    if not init:
        process_instances = [ProcessInstance(x, map(default_value, x.variables)) for x in process_identifiers]
        init = make_parallel_expression(actions, process_instances, n, parallel_operators)
    return ProcessSpecification(list(set(actions)), equations, init)

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
