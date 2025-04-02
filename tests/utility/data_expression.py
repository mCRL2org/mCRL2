#!/usr/bin/env python

# ~ Copyright 2012-2017 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from dataclasses import dataclass
from typing import Literal

# Type constants
TypeLiteral = Literal["Int", "Bool"]
INT_TYPE: TypeLiteral = "Int"
BOOL_TYPE: TypeLiteral = "Bool"

@dataclass(frozen=True)
class DataExpression:
    """Base class for all data expressions."""
    pass

@dataclass(frozen=True)
class Variable(DataExpression):
    """Represents a named variable with an associated type."""
    name: str
    type: TypeLiteral = BOOL_TYPE

    def __str__(self) -> str:
        return self.name

@dataclass(frozen=True)
class Integer(DataExpression):
    """Represents an integer value."""
    value: str
    type: TypeLiteral = INT_TYPE

    def __str__(self) -> str:
        return self.value

@dataclass(frozen=True)
class Boolean(DataExpression):
    """Represents a boolean value."""
    value: str
    type: TypeLiteral = BOOL_TYPE

    def __str__(self) -> str:
        return self.value
