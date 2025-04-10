#!/usr/bin/env python

# ~ Copyright 2012-2017 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from dataclasses import dataclass
from enum import Enum

from typeguard import typechecked

class Sort(Enum):
    """ Sorts of data expressions """
    INT = "Int"
    BOOL = "Bool"

@typechecked
@dataclass(frozen=True)
class DataExpression:
    """Base class for all data expressions."""
    pass

@typechecked
@dataclass(frozen=True)
class Variable(DataExpression):
    """Represents a named variable with an associated type."""
    name: str
    type: Sort

    def __str__(self) -> str:
        return self.name
    
    def parameter(self) -> str:
        """ Returns a "d: D" string representation of the variable. """
        return f"{self.name}: {self.type.value}"

@typechecked
@dataclass(frozen=True)
class Integer(DataExpression):
    """Represents an integer value."""
    value: str
    type: Sort = Sort.INT

    def __str__(self) -> str:
        return self.value

@typechecked
@dataclass(frozen=True)
class Boolean(DataExpression):
    """Represents a boolean value."""
    value: str
    type: Sort = Sort.BOOL

    def __str__(self) -> str:
        return self.value
