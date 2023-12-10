#!/usr/bin/env python

#~ Copyright 2012-2017 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import random

from .data_expression import Boolean, Integer, Variable

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
    integers = [x for x in variables if x.type == 'Int']
    booleans = [x for x in variables if x.type == 'Bool']
    result = booleans[:]
    for m in integers:
        result.append(Boolean(f'{m} > 0'))
        result.append(Boolean(f'{m} > 1'))
        result.append(Boolean(f'{m} < 2'))
        result.append(Boolean(f'{m} < 3'))
        for n in integers:
            result.append(Boolean(f'{m} == {n}'))
    result.append(Boolean('true'))
    result.append(Boolean('false'))
    return random.choice(result)

# Simple generator for integer data expressions.
def make_integer_data_expression(variables):
    assert is_list_of(variables, Variable)
    integers = [x for x in variables if x.type == 'Int']
    result = integers[:]
    for m in integers:
        for n in integers + [Integer('1'), Integer('2')]:
            result.append(Integer(f'{m} + {n}'))
            result.append(Integer(f'{m} - {n}'))
    result.append(Integer('0'))
    result.append(Integer('1'))
    return random.choice(result)
