#!/usr/bin/env python

#~ Copyright 2012-2017 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import copy
import random
from data_expression import *

def is_list_of(l, types):
    if not isinstance(l, list):
        return False
    for x in l:
        if not isinstance(x, types):
            return False
    return True

# Simple generator for boolean data expressions.
def make_boolean_data_expression(variables):
    assert is_list_of(variables, Variable)
    integers = set([x.name for x in variables if x.type == 'Int'])
    booleans = set([x.name for x in variables if x.type == 'Bool'])
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
    return random.choice(result)

# Simple generator for integer data expressions.
def make_integer_data_expression(variables):
    assert is_list_of(variables, Variable)
    integers = set([x.name for x in variables if x.type == 'Int'])
    result = []
    for m in integers:
        result.append(m)
        for n in integers - set([m]) | set(['1', '2']):
            result.append(Integer('{} + {}'.format(m, n)))
            result.append(Integer('{} - {}'.format(m, n)))
    result.append(Integer('0'))
    result.append(Integer('1'))
    return random.choice(result)
