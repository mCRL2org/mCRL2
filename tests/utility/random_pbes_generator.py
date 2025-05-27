#!/usr/bin/env python

# ~ Copyright 2010-2025 Wieger Wesselink, Maurice Laveaux.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import random
from typing import List, Any, Union, Optional
from abc import ABC, abstractmethod

from typeguard import typechecked

from .text_utility import write_text
from .pbes import QuantifierType, UnaryOperatorType, BinaryOperatorType, FixpointType

PREDICATE_INTEGERS = ["m", "n"]
PREDICATE_BOOLEANS = ["b", "c"]
QUANTIFIER_INTEGERS = ["t", "u", "v", "w"]
INTEGERS = PREDICATE_INTEGERS + QUANTIFIER_INTEGERS
BOOLEANS = PREDICATE_BOOLEANS

# As a convention we use that k, m, n are always natural numbers and
# b, c, d are always booleans.


class Generated(ABC):
    """Base class for generated terms."""

    @typechecked
    @abstractmethod
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        """Finalizes the generated term."""
        pass


class Boolean(Generated):
    def __init__(self, value=None):
        self.value = value

    def __repr__(self):
        if self.value is None:
            return "<BOOL>"
        return f"{self.value}"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        if self.value is None:
            self.value = make_boolean(freevars, add_val)


class Natural(Generated):
    def __init__(self, value=None):
        self.value = value

    def __repr__(self):
        if self.value is None:
            return "<NAT>"
        return f"{self.value}"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        if self.value is None:
            self.value = make_natural(freevars)

@typechecked
def add_type(var: Union[Boolean, Natural, str]) -> Optional[str]:
    """Adds a type ':Bool' or ':Nat' to the name of a variable"""
    if var in BOOLEANS:
        return f"{var}:Bool"
    elif var in INTEGERS:
        return f"{var}:Nat"
    return None


class PropositionalVariable(Generated):
    def __init__(self, name: str, args: List[Union[Boolean, Natural]]) -> None:
        self.name = name
        self.args = args
        self.prefix = ""  # sometimes we need to add a '!' to make a PBES monotonic

    @typechecked
    def __repr__(self) -> str:
        if len(self.args) == 0:
            return f"{self.prefix}{self.name}"
        return f"{self.prefix}{self.name}({', '.join(map(str, self.args))})"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        if negated:
            self.prefix = "!"
        for a in self.args:
            a.finish(freevars, negated, False)


class PredicateVariable(Generated):
    @typechecked
    def __init__(self, name: str, args: List[str]) -> None:
        self.name = name
        self.args = args

    @typechecked
    def __repr__(self) -> str:
        if len(self.args) == 0:
            return self.name
        args = map(add_type, self.args)
        return f"{self.name}({', '.join(map(str, args))})"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        pass


class Equation(Generated):
    @typechecked
    def __init__(self, sigma: FixpointType, var: PredicateVariable, formula: Any) -> None:
        self.sigma = sigma
        self.var = var
        self.formula = formula

    @typechecked
    def __repr__(self) -> str:
        return f"{self.sigma.value} {self.var} = {self.formula};"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        freevars = self.var.args
        self.formula.finish(freevars, negated, add_val)


class PBES(Generated):
    @typechecked
    def __init__(self, equations: List[Equation], init: PropositionalVariable) -> None:
        self.equations = equations
        self.init = init

    @typechecked
    def __repr__(self) -> str:
        # Python 3.10 does not allow backslash inside f-string interpolation.
        newline = "\n"
        return f"pbes\n{newline.join(map(str, self.equations))}\n\ninit {self.init};"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        for e in self.equations:
            e.finish()


class UnaryOperator(Generated):
    @typechecked
    def __init__(self, op: UnaryOperatorType, x: Any) -> None:
        self.op = op
        self.x = x

    @typechecked
    def __repr__(self) -> str:
        x = self.x
        op = self.op
        return f"{op.value}({x})"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        if self.op == UnaryOperatorType.NOT:
            negated = not negated
        self.x.finish(freevars, negated, add_val)


class BinaryOperator(Generated):
    @typechecked
    def __init__(self, op: BinaryOperatorType, x: Any, y: Any) -> None:
        self.op = op
        self.x = x
        self.y = y

    @typechecked
    def __repr__(self) -> str:
        x = self.x
        y = self.y
        op = self.op
        return f"({x}) {op.value} ({y})"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        if self.op == BinaryOperatorType.IMPLIES:
            self.x.finish(freevars, not negated, add_val)
        else:
            self.x.finish(freevars, negated, add_val)
        self.y.finish(freevars, negated, add_val)


class QuantifierDepthExceededError(BaseException):
    """Exception raised when the quantifier nesting depth is exceeded."""

    def __init__(self, message):
        super().__init__(message)
        self.message = message


class Quantifier(Generated):
    def __init__(self, quantor: QuantifierType, x, y):
        self.quantor = quantor
        self.x = x  # the bound variable
        self.y = y  # the formula

    @typechecked
    def __repr__(self) -> str:
        x = self.x
        y = self.y
        quantor = self.quantor
        return f"{quantor.value} {add_type(x)}.({y})"

    @typechecked
    def finish(
        self, freevars: List[str]|None = None, negated: bool = False, add_val: bool = True
    ) -> None:
        qvar = []
        for q in QUANTIFIER_INTEGERS:
            if q not in freevars:
                qvar.append(q)
        if len(qvar) == 0:
            raise QuantifierDepthExceededError(
                "warning: Quantifier nesting depth exceeded"
            )
        var, dummy = pick_element(qvar)
        self.x = var
        if self.quantor == QuantifierType.EXISTS:
            self.y = or_(Boolean(make_val(f"{var} < 3")), self.y)
        else:
            self.y = and_(Boolean(make_val(f"{var} < 3")), self.y)
        self.y.finish(freevars + [self.x], negated, add_val)


def not_(x):
    return UnaryOperator(UnaryOperatorType.NOT, x)


def and_(x, y):
    return BinaryOperator(BinaryOperatorType.AND, x, y)


def or_(x, y):
    return BinaryOperator(BinaryOperatorType.OR, x, y)


def implies(x, y):
    return BinaryOperator(BinaryOperatorType.IMPLIES, x, y)


def forall(x):
    var = Natural()
    phi = x
    return Quantifier(QuantifierType.FORALL, var, phi)


def exists(x):
    var = Natural()
    phi = x
    return Quantifier(QuantifierType.EXISTS, var, phi)


def equal_to(x, y):
    return BinaryOperator(BinaryOperatorType.EQUAL, x, y)


def not_equal_to(x, y):
    return BinaryOperator(BinaryOperatorType.NOT_EQUAL, x, y)


OPERATORS = [not_, and_, or_, implies, forall, exists]
"""The global list of operators used in the PBES generator."""


def is_boolean_constant(x):
    """Check if x is a Boolean with value 'false' or 'true'."""
    return isinstance(x, Boolean) and x.value in ["false", "true"]


def is_natural_constant(x):
    """Check if x is a Natural with value '0' or '1'."""
    return isinstance(x, Natural) and x.value in ["0", "1"]


def is_unary(op):
    """Check if op is a unary operator."""
    return op in [not_, forall, exists]


def pick_element(s):
    """
    Pick a random element x from a set s.
    
    Returns:
        tuple: (x, s - {x}) where x is the selected element and s - {x} is the
               remaining set without x.
    """
    n = random.randint(0, len(s) - 1)
    x = s[n]
    s = s[:n] + s[n + 1 :]
    return x, s


def pick_elements(s, n):
    """
    Randomly pick n elements from a set s.
    
    Returns:
        list: A sequence with the selected elements.
    """
    result = []
    for _ in range(n):
        x, s = pick_element(s)
        result.append(x)
    return result


def make_val(s):
    """Wrap s inside val (with 100% probability)."""
    return f"val({s})"


def make_predvar(n, use_integers=True, size=random.randint(0, 2)):
    name = f"X{n}"
    arguments = []
    variables = (
        PREDICATE_INTEGERS + PREDICATE_BOOLEANS if use_integers else PREDICATE_BOOLEANS
    )
    for _ in range(size):
        v, variables = pick_element(variables)
        arguments.append(v)
    return PredicateVariable(name, arguments)


def make_predvars(n, use_integers):
    """ Generates n random predicate variables with 0, 1 or 2 parameters """
    return [make_predvar(i, use_integers, random.randint(0, 2)) for i in range(n)]



def make_atoms(freevars, add_val=True):
    """ Creates elementary random boolean terms, with free variables from the set freevars. """
    naturals = set(freevars).intersection(set(INTEGERS))
    booleans = set(freevars).intersection(set(BOOLEANS))
    result = []
    for m in naturals:
        result.append(f"{m} > 0")
        result.append(f"{m} > 1")
        result.append(f"{m} < 2")
        result.append(f"{m} < 3")
        for n in naturals - {m}:
            result.append(f"{m} == {n}")
    for b in booleans:
        result.append(b)
    result.append("true")
    result.append("false")
    if add_val:
        result = [make_val(f"{x}") for x in result]
    return result


def make_boolean(freevars, add_val=True):
    atoms = make_atoms(freevars, add_val)
    x, dummy = pick_element(atoms)
    return x


def make_natural(freevars):
    naturals = set(freevars).intersection(set(INTEGERS))
    result = []
    result.append("0")
    result.append("1")
    for m in naturals:
        result.append(f"{m} + 1")
    x, dummy = pick_element(result)
    return x


def make_predvar_instantiations(predvars):
    """ returns instantiations of predicate variables """
    result = []
    for X in predvars:
        args = []
        for a in X.args:
            if a in BOOLEANS:
                args.append(Boolean())
            elif a in INTEGERS:
                args.append(Natural())
        result.append(PropositionalVariable(X.name, args))
    return result


def make_terms(predvars, m, n):
    """ Creates m boolean terms, and n propositional variable instantiations. """
    result = []
    for _ in range(m):
        result.append(Boolean())
    inst = make_predvar_instantiations(predvars)
    result = result + pick_elements(inst, n)
    return result


def join_terms(terms):
    op = OPERATORS[random.randint(0, len(OPERATORS) - 1)]
    if is_unary(op):
        x, terms = pick_element(terms)
        z = op(x)
    else:
        x, terms = pick_element(terms)
        y, terms = pick_element(terms)
        z = op(x, y)
    terms.append(z)
    return terms


def make_pbes(
    equation_count,
    atom_count=5,
    propvar_count=3,
    use_quantifiers=True,
    use_integers=True,
):
    global OPERATORS
    if use_quantifiers:
        OPERATORS = [not_, and_, or_, implies, not_, and_, or_, implies, forall, exists]
    else:
        OPERATORS = [not_, and_, or_, implies]

    retries = 1000
    while retries > 0:
        retries -= 1

        if retries == 0:
            # Remove the quantifiers
            print("Limit reached, removing quantifiers")
            OPERATORS = [not_, and_, or_, implies]

        try:
            predvars = make_predvars(equation_count, use_integers)
            equations = []
            for i in range(equation_count):
                terms = make_terms(predvars, atom_count, propvar_count)
                while len(terms) > 1:
                    terms = join_terms(terms)
                sigma, _ = pick_element([FixpointType.MU, FixpointType.NU])
                equations.append(Equation(sigma, predvars[i], terms[0]))

            X = predvars[0]
            args = []
            for a in X.args:
                if a in BOOLEANS:
                    args.append("true")
                elif a in INTEGERS:
                    args.append("0")
            init = PropositionalVariable(X.name, args)
            p = PBES(equations, init)
            p.finish()
            return p
        except QuantifierDepthExceededError:
            pass


def main():
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument(
        "destination", metavar="DIR", type=str, help="the output directory"
    )
    cmdline_parser.add_argument(
        "--equation-count",
        metavar="VALUE",
        type=int,
        action="store",
        help="the number of equations",
        default=4,
    )
    cmdline_parser.add_argument(
        "--atom-count",
        metavar="VALUE",
        type=int,
        action="store",
        help="the number of atoms",
        default="5",
    )
    cmdline_parser.add_argument(
        "--propvar-count",
        metavar="VALUE",
        type=int,
        action="store",
        help="the number of atoms",
        default="3",
    )
    cmdline_parser.add_argument(
        "--no-use-quantifiers",
        help="disable the generation of quantifiers",
        action="store_true",
        default=False,
    )
    cmdline_parser.add_argument(
        "--no-use-integers",
        help="disable the generation of integers",
        action="store_true",
        default=False,
    )
    cmdline_parser.add_argument(
        "-r",
        "--repetitions",
        dest="repetitions",
        type=int,
        metavar="N",
        default=10,
        help="generate N instances",
    )
    args = cmdline_parser.parse_args()

    for i in range(args.repetitions):
        file = os.path.join(args.destination, f"{i}.txt")
        pbes = make_pbes(
            args.equation_count,
            args.atom_count,
            args.propvar_count,
            not args.no_use_quantifiers,
            not args.no_use_integers,
        )
        print(pbes)
        write_text(file, str(pbes))


if __name__ == "__main__":
    main()
