#!/usr/bin/env python

# ~ Copyright 2012-2017 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from dataclasses import dataclass

@dataclass(frozen=True)
class DataExpression:
    """Base class for all data expressions."""
    pass

@dataclass(frozen=True)
class Variable(DataExpression):
    """Represents a named variable with an associated type."""
    name: str
    type: str = "Bool"

    def __str__(self) -> str:
        return self.name

@dataclass(frozen=True)
class Integer(DataExpression):
    """Represents an integer value."""
    value: str
    type: str = "Int"

    def __str__(self) -> str:
        return self.value

@dataclass(frozen=True)
class Boolean(DataExpression):
    """Represents a boolean value."""
    value: str
    type: str = "Bool"

    def __str__(self) -> str:
        return self.value
