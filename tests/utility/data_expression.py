#!/usr/bin/env python

# ~ Copyright 2012-2017 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)


class DataExpression(object):
    """Base class for all data expressions."""
    def __init__(self) -> None:
        pass


class Variable(DataExpression):
    """Represents a named variable with an associated type."""
    def __init__(self, name: str, _type: str = "Bool") -> None:
        self.name: str = name
        self.type: str = _type

    def __str__(self) -> str:
        return self.name


class Integer(DataExpression):
    """Represents an integer value."""
    def __init__(self, value: str) -> None:
        self.value: str = value
        self.type: str = "Int"

    def __str__(self) -> str:
        return self.value


class Boolean(DataExpression):
    """Represents a boolean valuen."""
    def __init__(self, value: str) -> None:
        self.value: str = value
        self.type: str = "Bool"

    def __str__(self) -> str:
        return self.value
