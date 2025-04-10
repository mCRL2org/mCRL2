#!/usr/bin/env python

# ~ Copyright 2010 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from dataclasses import dataclass
from typing import List, Union
from typeguard import typechecked

from .data_expression import Variable, DataExpression

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
    sigma: str
    var: PredicateVariable
    formula: Union[DataExpression, PropositionalVariable]

    def __repr__(self) -> str:
        return f"{self.sigma} {self.var} = {self.formula};"


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
    op: str
    x: Union[DataExpression, PropositionalVariable]

    def __repr__(self) -> str:
        return f"{self.op}({self.x})"


@typechecked
@dataclass(frozen=True)
class BinaryOperator:
    """
    Represents a binary operator applied to two terms.
    """
    op: str
    x: Union[DataExpression, PropositionalVariable]
    y: Union[DataExpression, PropositionalVariable]

    def __repr__(self) -> str:
        return f"({self.x}) {self.op} ({self.y})"


@typechecked
@dataclass(frozen=True)
class Quantifier:
    """
    Represents a quantifier (forall or exists) applied to a formula.
    """
    quantifier: str
    x: str  # the bound variable
    y: Union[DataExpression, PropositionalVariable]  # the formula

    def __repr__(self) -> str:
        return f"{self.quantifier} {self.x}.({self.y})"
