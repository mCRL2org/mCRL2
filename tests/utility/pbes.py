#!/usr/bin/env python

# ~ Copyright 2010-2025 Wieger Wesselink, Maurice Laveaux.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from dataclasses import dataclass
from typing import List, Union
from typeguard import typechecked
from enum import Enum

from .data_expression import Variable, DataExpression

class QuantifierType(Enum):
    FORALL = "forall"
    EXISTS = "exists"

class UnaryOperatorType(Enum):
    NOT = "!"

class BinaryOperatorType(Enum):
    AND = "&&"
    OR = "||"
    IMPLIES = "=>"
    EQUAL = "=="
    NOT_EQUAL = "!="

class FixpointType(Enum):
    """
    Represents two fixpoint types in PBES equations.
    """
    MU = "mu"  # Least fixpoint
    NU = "nu"  # Greatest fixpoint

@typechecked
@dataclass(frozen=True)
class PropositionalVariable:
    """
    Represents a propositional variable with optional arguments.
    """
    name: str
    args: List[DataExpression]
    prefix: str = ""

    def __repr__(self) -> str:
        if len(self.args) == 0:
            return f"{self.prefix}{self.name}"
        return f"{self.prefix}{self.name}({', '.join(map(str, self.args))})"


@typechecked
@dataclass(frozen=True)
class PredicateVariable:
    """
    Represents a predicate variable with typed arguments.
    """
    name: str
    args: List[Variable]

    def __repr__(self) -> str:
        if len(self.args) == 0:
            return self.name
        return f"{self.name}({', '.join(map(str, self.args))})"


@typechecked
@dataclass(frozen=True)
class Equation:
    """
    Represents an equation in a PBES.
    """
    sigma: FixpointType
    var: PredicateVariable
    formula: Union[DataExpression, PropositionalVariable]

    def __repr__(self) -> str:
        return f"{self.sigma.value} {self.var} = {self.formula};"


@typechecked
@dataclass(frozen=True)
class PBES:
    """
    Represents a Parameterized Boolean Equation System (PBES).
    """
    equations: List[Equation]
    init: PropositionalVariable

    def __repr__(self) -> str:
        equations_str = "\n".join(map(str, self.equations))
        return f"pbes\n{equations_str}\n\ninit {self.init};"


@typechecked
@dataclass(frozen=True)
class UnaryOperator:
    """
    Represents a unary operator applied to a term.
    """
    op: UnaryOperatorType
    x: Union[DataExpression, PropositionalVariable]

    def __repr__(self) -> str:
        return f"{self.op.value}({self.x})"


@typechecked
@dataclass(frozen=True)
class BinaryOperator:
    """
    Represents a binary operator applied to two terms.
    """
    op: BinaryOperatorType
    x: Union[DataExpression, PropositionalVariable]
    y: Union[DataExpression, PropositionalVariable]

    def __repr__(self) -> str:
        return f"({self.x}) {self.op.value} ({self.y})"

@typechecked
@dataclass(frozen=True)
class Quantifier:
    """
    Represents a quantifier (forall or exists) applied to a formula.
    """
    quantifier: QuantifierType
    x: str  # the bound variable
    y: Union[DataExpression, PropositionalVariable]  # the formula

    def __repr__(self) -> str:
        return f"{self.quantifier.value} {self.x}.({self.y})"
