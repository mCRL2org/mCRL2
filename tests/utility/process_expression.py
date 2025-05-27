#!/usr/bin/env python

# ~ Copyright 2012-2025 Wieger Wesselink, Maurice Laveaux.
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
from dataclasses import dataclass
from typing import NewType
from enum import Enum

from typeguard import typechecked

from .data_expression import DataExpression, Variable

# Type aliases
ActionName = NewType('ActionName', str)

class ProcessConstant(Enum):
    TAU = "tau"
    DELTA = "delta"

class OperatorSymbol(Enum):
    CHOICE = "+"
    SEQUENCE = "."
    BOUNDED_INIT = "<<"
    LEFT_MERGE = "||_"
    MERGE = "||"
    SYNC = "|"


@typechecked
def parse_variable(text: str) -> Variable:
    """
    Parse a string representation of a variable with its type.
    """
    text = text.strip()
    m = re.match(r"([^,:]+)\s*\:(.+)", text)
    assert m is not None
    result = Variable(m.group(1).strip(), m.group(2).strip())
    return result


@typechecked
def parse_variables(text: str) -> list[Variable]:
    """
    Parse a comma-separated list of variable declarations.
    """
    variables = map(str.strip, text.split(","))
    return list(map(parse_variable, variables))


@typechecked
class ProcessIdentifier(object):
    """
    Represents a process identifier with optional parameters.
    Example: P(m: Nat, b: Bool) represents process P with two parameters.
    """
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
            return f"{self.name}({', '.join(f'{x}: {x.type}' for x in self.variables)})"
        else:
            return self.name


@typechecked
class ProcessExpression(object):
    """Base class for all process expressions in mCRL2."""
    def __init__(self) -> None:
        pass


@typechecked
@dataclass(frozen=True)
class Action(ProcessExpression):
    """Represents a basic action in the process algebra."""
    a: ActionName

    def __str__(self) -> str:
        return self.a


@typechecked
@dataclass(frozen=True)
class MultiAction(ProcessExpression):
    """Represents multiple actions that occur simultaneously."""
    actions: list[ActionName]

    def __str__(self) -> str:
        if len(self.actions) == 0:
            return ProcessConstant.TAU.value
        return f" {OperatorSymbol.SYNC.value} ".join(self.actions)

    def __hash__(self) -> int:
        return hash(str(self))


@typechecked
@dataclass(frozen=True)
class Delta(ProcessExpression):
    """Represents the deadlock process."""
    def __str__(self) -> str:
        return ProcessConstant.DELTA.value


@typechecked
@dataclass(frozen=True)
class Tau(ProcessExpression):
    """Represents the internal (silent) action."""
    def __str__(self) -> str:
        return ProcessConstant.TAU.value


@typechecked
@dataclass(frozen=True)
class ProcessInstance(ProcessExpression):
    """
    Represents a process instantiation with parameters.
    Example: P(1, true) for a process P with two parameters.
    """
    identifier: ProcessIdentifier
    parameters: list[DataExpression]

    def __str__(self) -> str:
        if self.parameters:
            return f"{self.identifier.name}({', '.join(map(str, self.parameters))})"
        
        return str(self.identifier.name)


@typechecked
@dataclass(frozen=True)
class Sum(ProcessExpression):
    """
    Represents a sum operator that introduces a variable binding.
    Example: sum d:D . x
    """
    d: Variable
    x: ProcessExpression

    def __str__(self) -> str:
        return f"sum {self.d}: {self.d.type}. ({self.x})"


@typechecked
@dataclass(frozen=True)
class IfThen(ProcessExpression):
    """
    Represents a conditional process with one branch.
    Example: condition -> process
    """
    c: DataExpression
    x: ProcessExpression

    def __str__(self) -> str:
        return f"({self.c}) -> ({self.x})"


@typechecked
@dataclass(frozen=True)
class IfThenElse(ProcessExpression):
    """
    Represents a conditional process with two branches.
    Example: condition -> process1 <> process2
    """
    c: DataExpression
    x: ProcessExpression
    y: ProcessExpression

    def __str__(self) -> str:
        return f"({self.c}) -> ({self.x}) <> ({self.y})"


@typechecked
@dataclass(frozen=True)
class BinaryOperator(ProcessExpression):
    """Base class for all binary operators in the process algebra."""
    op: OperatorSymbol
    x: ProcessExpression
    y: ProcessExpression

    def __str__(self) -> str:
        return f"({self.x}) {self.op.value} ({self.y})"


@typechecked
@dataclass(frozen=True)
class Choice(BinaryOperator):
    """
    Represents alternative composition (choice) between processes.
    Example: p + q
    """
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super().__init__(OperatorSymbol.CHOICE, x, y)


@typechecked
@dataclass(frozen=True)
class Seq(BinaryOperator):
    """
    Represents sequential composition of processes.
    Example: p . q
    """
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super().__init__(OperatorSymbol.SEQUENCE, x, y)


@typechecked
@dataclass(frozen=True)
class BoundedInit(BinaryOperator):
    """
    Represents bounded initialization between processes.
    Example: p << q
    """
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super().__init__(OperatorSymbol.BOUNDED_INIT, x, y)


@typechecked
@dataclass(frozen=True)
class LeftMerge(BinaryOperator):
    """
    Represents left merge parallel composition.
    Example: p ||_ q
    """
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super().__init__(OperatorSymbol.LEFT_MERGE, x, y)


@typechecked
@dataclass(frozen=True)
class Merge(BinaryOperator):
    """
    Represents parallel composition of processes.
    Example: p || q
    """
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super().__init__(OperatorSymbol.MERGE, x, y)


@typechecked
@dataclass(frozen=True)
class Sync(BinaryOperator):
    """
    Represents synchronization of processes.
    Example: p | q
    """
    def __init__(self, x: ProcessExpression, y: ProcessExpression) -> None:
        super().__init__(OperatorSymbol.SYNC, x, y)


@typechecked
@dataclass(frozen=True)
class At(ProcessExpression):
    """
    Represents the at operator for timed processes.
    Example: p @ t
    """
    x: ProcessExpression
    t: DataExpression

    def __str__(self) -> str:
        return f"({self.x}) @ ({self.t})"


@typechecked
@dataclass(frozen=True)
class Allow(ProcessExpression):
    """
    Represents the allow operator that specifies allowed multi-actions.
    Example: allow({a|b, c}, p)
    """
    V: list[MultiAction]
    x: ProcessExpression

    def __str__(self) -> str:
        return f"allow({{{', '.join(map(str, self.V))}}}, {self.x})"


@typechecked
@dataclass(frozen=True)
class Block(ProcessExpression):
    """
    Represents the block operator that blocks specified actions.
    Example: block({a, b}, p)
    """
    B: list[str]
    x: ProcessExpression

    def __str__(self) -> str:
        return f"block({{{', '.join(self.B)}}}, {self.x})"


@typechecked
@dataclass(frozen=True)
class Comm(ProcessExpression):
    """
    Represents the communication operator for renaming action combinations.
    Example: comm({a|b -> c}, p)
    """
    C: list[str]
    x: ProcessExpression

    def __str__(self) -> str:
        return f"comm({{{', '.join(self.C)}}}, {self.x})"


@typechecked
@dataclass(frozen=True)
class Hide(ProcessExpression):
    """
    Represents the hide operator that hides specified actions.
    Example: hide({a, b}, p)
    """
    I: list[str]
    x: ProcessExpression

    def __str__(self) -> str:
        return f"hide({{{', '.join(self.I)}}}, {self.x})"


@typechecked
@dataclass(frozen=True)
class Rename(ProcessExpression):
    """
    Represents the rename operator for renaming actions.
    Example: rename({a -> b}, p)
    """
    R: list[str]
    x: ProcessExpression

    def __str__(self) -> str:
        return f"rename({{{', '.join(self.R)}}}, {self.x})"


@typechecked
@dataclass(frozen=True)
class StochasticOperator(ProcessExpression):
    """
    Represents a stochastic operator with a distribution.
    Example: dist x:Real[dist]. p
    """
    v: Variable
    dist: DataExpression
    x: ProcessExpression

    def __str__(self) -> str:
        return f"dist {self.v}: {self.v.type}[{self.dist}].({self.x})"
    
@typechecked
@dataclass(frozen=True)
class Literal(ProcessExpression):
    """
    Represents a literal process expression.
    Example: 1
    """
    value: str

    def __str__(self) -> str:
        return self.value
