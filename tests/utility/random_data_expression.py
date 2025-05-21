#!/usr/bin/env python

#~ Copyright 2012-2025 Wieger Wesselink, Maurice Laveaux.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import random
from typing import List

from typeguard import typechecked

from .data_expression import Boolean, Integer, Variable, Sort

@typechecked
def make_boolean_data_expression(variables: List[Variable]) -> Boolean:
    """ Simple generator for boolean data expressions. """
    integers = [x for x in variables if x.type == Sort.INT]
    booleans = [x for x in variables if x.type == Sort.BOOL]
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

@typechecked
def make_integer_data_expression(variables: List[Variable]) -> Integer:
    """ Simple generator for integer data expressions. """
    integers = [x for x in variables if x.type == Sort.INT]
    result = integers[:]
    for m in integers:
        for n in integers + [Integer('1'), Integer('2')]:
            result.append(Integer(f'{m} + {n}'))
            result.append(Integer(f'{m} - {n}'))
    result.append(Integer('0'))
    result.append(Integer('1'))
    return random.choice(result)
