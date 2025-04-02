#!/usr/bin/env python

# ~ Copyright 2010 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import random
from dataclasses import dataclass
from typing import List, Literal, Callable, TypeAlias
from abc import ABC, abstractmethod

# Add type aliases for clarity
FixPoint = Literal["mu", "nu"]
Operator = Literal["!", "&&", "||", "=>"]

# Constants
MU: FixPoint = "mu"
NU: FixPoint = "nu"

@dataclass(frozen=True)
class BesFormula(ABC):
    """Abstract base class for all formula types in a Boolean Equation System."""
    @abstractmethod
    def __repr__(self) -> str:
        pass
    
OperatorFunction = Callable[..., BesFormula]

@dataclass(frozen=True)
class PredicateVariable(BesFormula):
    """Represents a predicate variable in a Boolean Equation System."""
    name: str

    def __repr__(self) -> str:
        return self.name

@dataclass(frozen=True)
class Equation:
    """Represents a single equation in a Boolean Equation System.
    
    Contains a fixpoint operator (sigma), variable and a formula.
    """
    sigma: FixPoint
    var: PredicateVariable
    formula: BesFormula

    def __repr__(self) -> str:
        return f"{self.sigma} {self.var} = {self.formula};"

@dataclass(frozen=True)
class BooleanEquationSystem:
    """Represents a complete Boolean Equation System.
    
    Contains a list of equations and an initial variable.
    """
    equations: List[Equation]
    init: PredicateVariable

    def __repr__(self) -> str:
        return f"pbes\n{'\n'.join(map(str, self.equations))}\n\ninit {self.init};"

@dataclass(frozen=True)
class UnaryOperator(BesFormula):
    """Represents a unary operator (like negation) in a Boolean formula."""
    op: Literal["!"]
    x: BesFormula

    def __repr__(self) -> str:
        return f"{self.op}({self.x})"

@dataclass(frozen=True)
class BinaryOperator(BesFormula):
    """Represents a binary operator (like and, or) in a Boolean formula."""
    op: Literal["&&", "||", "=>"]
    x: BesFormula
    y: BesFormula

    def __repr__(self) -> str:
        return f"({self.x}) {self.op} ({self.y})"

def not_(x: BesFormula) -> UnaryOperator:
    """Creates a negation operator for a given formula."""
    return UnaryOperator("!", x)

def and_(x: BesFormula, y: BesFormula) -> BinaryOperator:
    """Creates a conjunction between two formulas."""
    return BinaryOperator("&&", x, y)

def or_(x: BesFormula, y: BesFormula) -> BinaryOperator:
    """Creates a disjunction between two formulas."""
    return BinaryOperator("||", x, y)

def implies(x: BesFormula, y: BesFormula) -> BinaryOperator:
    """Creates an implication between two formulas."""
    return BinaryOperator("=>", x, y)

# Single operators list definition
operators: List[OperatorFunction] = [and_, or_]

def is_unary(op: OperatorFunction) -> bool:
    """ Checks if the operator is unary. """
    return op is not_

# pick a random element x from a set s
# returns x, (s - {x})
def pick_element(s: List[BesFormula]) -> tuple[BesFormula, List[BesFormula]]:
    """Randomly picks an element from a sequence.
    
    Returns a tuple of (picked element, remaining sequence).
    """
    n = random.randint(0, len(s) - 1)
    return s[n], s[:n] + s[n+1:]

# randomly pick n elements from a set s
# returns a sequence with the selected elements
def pick_elements(s: List[BesFormula], n: int) -> List[BesFormula]:
    """Randomly picks n elements from a sequence.
    
    Returns a list of picked elements.
    """
    result = []
    for _ in range(n):
        x, s = pick_element(s)
        result.append(x)
    return result

# Creates n terms
def make_terms(predvars: List[PredicateVariable], n: int) -> List[PredicateVariable]:
    """Creates n random predicate variable terms from the given set of variables."""
    result = []
    for _ in range(n):
        result.append(predvars[random.randint(0, len(predvars) - 1)])
    return result

def join_terms(terms: List[BesFormula]) -> List[BesFormula]:
    """Combines terms using random operators to create more complex formulas."""
    op = operators[random.randint(0, len(operators) - 1)]
    if is_unary(op):
        x, terms = pick_element(terms)
        z = op(x)
    else:
        x, terms = pick_element(terms)
        y, terms = pick_element(terms)
        z = op(x, y)
    terms.append(z)
    return terms

def make_bes(equation_count: int, term_size: int = 3) -> BooleanEquationSystem:
    """Generates a random Boolean Equation System.
    
    Args:
        equation_count: Number of equations to generate
        term_size: Maximum size of terms in each equation
    """
    predvars = [PredicateVariable(f"X{i}") for i in range(1, equation_count + 1)]
    equations = []
    for i in range(equation_count):
        terms = make_terms(predvars, random.randint(1, term_size))
        while len(terms) > 1:
            terms = join_terms(terms)
        sigma = random.choice([MU, NU])
        equations.append(Equation(sigma, predvars[i], terms[0]))
    init = predvars[0]
    return BooleanEquationSystem(equations, init)

if __name__ == "__main__":
    b = make_bes(4, 5)
    print(b)
