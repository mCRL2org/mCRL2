#!/usr/bin/env python

# ~ Copyright 2010 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import random
from typing import List, Any, Union, Optional

from typeguard import typechecked

from .text_utility import write_text

PREDICATE_INTEGERS = ["m", "n"]
PREDICATE_BOOLEANS = ["b", "c"]
QUANTIFIER_INTEGERS = ["t", "u", "v", "w"]
INTEGERS = PREDICATE_INTEGERS + QUANTIFIER_INTEGERS
BOOLEANS = PREDICATE_BOOLEANS

# As a convention we use that k, m, n are always natural numbers and
# b, c, d are always booleans.


# Adds a type ':Bool' or ':Nat' to the name of a variable
@typechecked
def add_type(var: str) -> Optional[str]:
    if var in BOOLEANS:
        return f"{var}:Bool"
    elif var in INTEGERS:
        return f"{var}:Nat"
    return None


class Boolean:
    def __init__(self, value=None):
        self.value = value

    def __repr__(self):
        if self.value is None:
            return "<BOOL>"
        else:
            return f"{self.value}"

    def finish(self, freevars, negated, add_val=True):
        if self.value is None:
            self.value = make_boolean(freevars, add_val)


class Natural:
    def __init__(self, value=None):
        self.value = value

    def __repr__(self):
        if self.value is None:
            return "<NAT>"
        else:
            return f"{self.value}"

    def finish(self, freevars, negated, add_val=True):
        if self.value is None:
            self.value = make_natural(freevars, add_val)


class PropositionalVariable:
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
    def finish(self, freevars: List[str], negated: bool) -> None:
        if negated:
            self.prefix = "!"
        add_val = False
        for a in self.args:
            a.finish(freevars, negated, add_val)


class PredicateVariable:
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
    def finish(self, freevars: List[str], negated: bool) -> None:
        pass


class Equation:
    @typechecked
    def __init__(self, sigma: str, var: PredicateVariable, formula: Any) -> None:
        self.sigma = sigma
        self.var = var
        self.formula = formula

    @typechecked
    def __repr__(self) -> str:
        return f"{self.sigma} {self.var} = {self.formula};"

    @typechecked
    def finish(self) -> None:
        freevars = self.var.args
        negated = False
        self.formula.finish(freevars, negated)


class PBES:
    @typechecked
    def __init__(self, equations: List[Equation], init: PropositionalVariable) -> None:
        self.equations = equations
        self.init = init

    @typechecked
    def __repr__(self) -> str:
        return f"pbes\n{'\n'.join(map(str, self.equations))}\n\ninit {self.init};"

    @typechecked
    def finish(self) -> None:
        for e in self.equations:
            e.finish()


class UnaryOperator:
    @typechecked
    def __init__(self, op: str, x: Any) -> None:
        self.op = op
        self.x = x

    @typechecked
    def __repr__(self) -> str:
        x = self.x
        op = self.op
        return f"{op}({x})"

    @typechecked
    def finish(self, freevars: List[str], negated: bool) -> None:
        if self.op == "!":
            negated = not negated
        self.x.finish(freevars, negated)


class BinaryOperator:
    @typechecked
    def __init__(self, op: str, x: Any, y: Any) -> None:
        self.op = op
        self.x = x
        self.y = y

    @typechecked
    def __repr__(self) -> str:
        x = self.x
        y = self.y
        op = self.op
        return f"({x}) {op} ({y})"

    @typechecked
    def finish(self, freevars: List[str], negated: bool) -> None:
        if self.op == "=>":
            self.x.finish(freevars, not negated)
        else:
            self.x.finish(freevars, negated)
        self.y.finish(freevars, negated)


class Quantifier:
    def __init__(self, quantor, x, y):
        self.quantor = quantor
        self.x = x  # the bound variable
        self.y = y  # the formula

    def __repr__(self):
        x = self.x
        y = self.y
        quantor = self.quantor
        return f"{quantor} {add_type(x)}.({y})"

    def finish(self, freevars, negated):
        qvar = []
        for q in QUANTIFIER_INTEGERS:
            if not q in freevars:
                qvar.append(q)
        if len(qvar) == 0:
            raise RuntimeError("warning: Quantifier nesting depth exceeded")
        var, dummy = pick_element(qvar)
        self.x = var
        if self.quantor == "exists":
            self.y = or_(Boolean(make_val(f"{var} < 3")), self.y)
        else:
            self.y = and_(Boolean(make_val(f"{var} < 3")), self.y)
        self.y.finish(freevars + [self.x], negated)


def not_(x):
    return UnaryOperator("!", x)


def and_(x, y):
    return BinaryOperator("&&", x, y)


def or_(x, y):
    return BinaryOperator("||", x, y)


def implies(x, y):
    return BinaryOperator("=>", x, y)


def forall(x):
    var = Natural()
    phi = x
    return Quantifier("forall", var, phi)


def exists(x):
    var = Natural()
    phi = x
    return Quantifier("exists", var, phi)


def equal_to(x, y):
    return BinaryOperator("==", x, y)


def not_equal_to(x, y):
    return BinaryOperator("!=", x, y)


# operators = [not_, forall, exists, and_, or_, implies, equal_to, not_equal_to]
operators = [not_, and_, or_, implies, forall, exists]


def is_boolean_constant(x):
    return isinstance(x, Boolean) and x.value in ["false", "true"]


def is_natural_constant(x):
    return isinstance(x, Natural) and x.value in ["0", "1"]


def is_unary(op):
    return op in [not_, forall, exists]


# pick a random element x from a set s
# returns x, (s - {x})
def pick_element(s):
    n = random.randint(0, len(s) - 1)
    x = s[n]
    s = s[:n] + s[n + 1 :]
    return x, s


# randomly pick n elements from a set s
# returns a sequence with the selected elements
def pick_elements(s, n):
    result = []
    for _ in range(n):
        x, s = pick_element(s)
        result.append(x)
    return result


# with a 100% probability wrap s inside val
def make_val(s):
    return f"val({s})"

    # n = random.randint(0, 1)
    # if n == 0:
    #    return f'val({s})'
    # else:
    #    return s


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


# Generates n random predicate variables with 0, 1 or 2 parameters
def make_predvars(n, use_integers):
    return [make_predvar(i, use_integers, random.randint(0, 2)) for i in range(n)]


# Creates elementary random boolean terms, with free variables
# from the set freevars.
def make_atoms(freevars, add_val=True):
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


def make_natural(freevars, add_val=True):
    naturals = set(freevars).intersection(set(INTEGERS))
    result = []
    result.append("0")
    result.append("1")
    for m in naturals:
        result.append(f"{m} + 1")
    x, dummy = pick_element(result)
    return x


# returns instantiations of predicate variables
def make_predvar_instantiations(predvars):
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


# Creates m boolean terms, and n propositional variable instantiations.
def make_terms(predvars, m, n):
    result = []
    for i in range(m):
        result.append(Boolean())
    inst = make_predvar_instantiations(predvars)
    result = result + pick_elements(inst, n)
    return result


def join_terms(terms):
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


def make_pbes(
    equation_count,
    atom_count=5,
    propvar_count=3,
    use_quantifiers=True,
    use_integers=True,
):
    global operators
    if use_quantifiers:
        operators = [not_, and_, or_, implies, not_, and_, or_, implies, forall, exists]
    else:
        operators = [not_, and_, or_, implies]

    predvars = make_predvars(equation_count, use_integers)
    equations = []
    for i in range(equation_count):
        terms = make_terms(predvars, atom_count, propvar_count)
        while len(terms) > 1:
            terms = join_terms(terms)
        sigma, dummy = pick_element(["mu", "nu"])
        while True:
            try:
                equations.append(Equation(sigma, predvars[i], terms[0]))
                break
            except RuntimeError as e:
                print(e)
                pass

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
