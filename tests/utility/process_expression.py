#!/usr/bin/env python

# ~ Copyright 2012-2017 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

# --------------------------------------------------
# expression                  | #children  | pcrl |
# --------------------------------------------------
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
# --------------------------------------------------
# unsupported                                     |
# --------------------------------------------------
# process_instance_assignment |     0      |  y   |
# At                          |     1      |  y   |
# binit                       |     2      |  y   |

import re
from .data_expression import DataExpression, Variable


# example: 'b: Bool'
def parse_variable(text: str) -> Variable:
    text = text.strip()
    m = re.match(r"([^,:]+)\s*\:(.+)", text)
    assert m is not None
    result = Variable(m.group(1).strip(), m.group(2).strip())
    return result


# example: 'b: Bool, m: Nat'
def parse_variables(text: str) -> list[Variable]:
    variables = map(str.strip, text.split(","))
    return list(map(parse_variable, variables))


# example: 'P(m: Nat, b: Bool)'
class ProcessIdentifier(object):
    name: str
    variables: list[Variable]
    
    def __init__(self, text: str) -> None:
        m = re.search(r"(\w*)(\(.*\))?", text)
        assert m is not None
        self.name = m.group(1)
        if m.group(2):
            vartext = m.group(2)[1:-1]
            self.variables = parse_variables(vartext)
        else:
            self.variables = []

    def __str__(self) -> str:
        if self.variables:
            return f"{self.name}({', '.join([f'{x}: {x.type}' for x in self.variables])})"
        else:
            return self.name


class ProcessExpression(object):
    pass


class Action(ProcessExpression):
    a: str
    
    def __init__(self, a: str) -> None:
        self.a = a

    def __str__(self) -> str:
        return self.a


class MultiAction(ProcessExpression):
    actions: list[str]
    
    def __init__(self, actions: list[str]) -> None:
        self.actions = actions

    def __eq__(self, other: object) -> bool:
        if type(other) is type(self):
            return self.__dict__ == other.__dict__
        return False

    def __ne__(self, other: object) -> bool:
        return not self.__eq__(other)

    def __str__(self) -> str:
        if len(self.actions) == 0:
            return "Tau"
        return " | ".join(self.actions)

    def __hash__(self) -> int:
        return hash(str(self))


class Delta(ProcessExpression):
    def __str__(self) -> str:
        return "delta"


class Tau(ProcessExpression):
    def __str__(self) -> str:
        return "tau"


class ProcessInstance(ProcessExpression):
    identifier: ProcessIdentifier
    parameters: list[DataExpression]
    
    def __init__(self, identifier: ProcessIdentifier, parameters: list[DataExpression]) -> None:
        self.identifier = identifier
        self.parameters = parameters

    def __str__(self) -> str:
        if self.parameters:
            return f"{self.identifier.name}({', '.join(map(str, self.parameters))})"
        else:
            return self.identifier.name


class Sum(ProcessExpression):
    d: Variable
    x: ProcessExpression
    
    def __init__(self, d: Variable, x: ProcessExpression) -> None:
        assert isinstance(d, Variable)
        self.d = d
        self.x = x

    def __str__(self) -> str:
        return f"sum {self.d}: {self.d.type}. ({self.x})"


class IfThen(ProcessExpression):
    c: DataExpression
    x: ProcessExpression
    
    def __init__(self, c: DataExpression, x: ProcessExpression) -> None:
        self.x = x
        self.c = c

    def __str__(self) -> str:
        return f"({self.c}) -> ({self.x})"


class IfThenElse(ProcessExpression):
    c: DataExpression
    x: ProcessExpression
    y: ProcessExpression
    
    def __init__(self, c: DataExpression, x: ProcessExpression, y: ProcessExpression) -> None:
        self.c = c
        self.x = x
        self.y = y

    def __str__(self) -> str:
        return f"({self.c}) -> ({self.x}) <> ({self.y})"


class BinaryOperator(ProcessExpression):
    op: str
    x: ProcessExpression
    y: ProcessExpression
    
    def __init__(self, op: str, x: ProcessExpression, y: ProcessExpression) -> None:
        self.op = op
        self.x = x
        self.y = y

    def __str__(self) -> str:
        x = self.x
        y = self.y
        op = self.op
        return f"({x}) {op} ({y})"


class Choice(BinaryOperator):
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super(Choice, self).__init__("+", x, y)


class Seq(BinaryOperator):
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super(Seq, self).__init__(".", x, y)


class BoundedInit(BinaryOperator):
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super(BoundedInit, self).__init__("<<", x, y)


class LeftMerge(BinaryOperator):
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super(LeftMerge, self).__init__("||_", x, y)


class Merge(BinaryOperator):
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super(Merge, self).__init__("||", x, y)


class Sync(BinaryOperator):
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super(Sync, self).__init__("|", x, y)


class At(ProcessExpression):
    x: ProcessExpression
    t: DataExpression
    
    def __init__(self, x: ProcessExpression, t: DataExpression) -> None:
        self.x = x
        self.t = t

    def __str__(self) -> str:
        x = self.x
        t = self.t
        return f"({x}) @ ({t})"


class Allow(ProcessExpression):
    V: list[MultiAction]
    x: ProcessExpression
    
    def __init__(self, V: list[MultiAction], x: ProcessExpression) -> None:
        self.V = V
        self.x = x

    def __str__(self) -> str:
        V = self.V
        x = self.x
        return f"allow({{{', '.join(map(str, V))}}}, {x})"


class Block(ProcessExpression):
    B: list[str]
    x: ProcessExpression
    
    def __init__(self, B: list[str], x: ProcessExpression) -> None:
        self.B = B
        self.x = x

    def __str__(self) -> str:
        B = self.B
        x = self.x
        return f"block({{{', '.join(B)}}}, {x})"


class Comm(ProcessExpression):
    C: list[str]
    x: ProcessExpression
    
    def __init__(self, C: list[str], x: ProcessExpression) -> None:
        self.C = C
        self.x = x

    def __str__(self) -> str:
        C = self.C
        x = self.x
        return f"comm({{{', '.join(C)}}}, {x})"


class Hide(ProcessExpression):
    I: list[str]
    x: ProcessExpression
    
    def __init__(self, I: list[str], x: ProcessExpression) -> None:
        self.I = I
        self.x = x

    def __str__(self) -> str:
        I = self.I
        x = self.x
        return f"hide({{{', '.join(I)}}}, {x})"


class Rename(ProcessExpression):
    R: list[str]
    x: ProcessExpression
    
    def __init__(self, R: list[str], x: ProcessExpression) -> None:
        self.R = R
        self.x = x

    def __str__(self) -> str:
        x = self.x
        R = self.R
        return f"rename({{{', '.join(R)}}}, {x})"


class StochasticOperator(ProcessExpression):
    v: Variable
    dist: DataExpression
    x: ProcessExpression
    
    def __init__(self, v: Variable, dist: DataExpression, x: ProcessExpression) -> None:
        self.v = v
        self.dist = dist
        self.x = x

    def __str__(self) -> str:
        dist = self.dist
        v = self.v
        x = self.x
        return f"dist {v}: {v.type}[{dist}].({x})"
