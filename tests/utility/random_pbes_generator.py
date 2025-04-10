#!/usr/bin/env python

# ~ Copyright 2010 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import random
from typing import List, Tuple, Union

from typeguard import typechecked

from .text_utility import write_text
from .pbes import PropositionalVariable, PredicateVariable, Equation, PBES, UnaryOperator, BinaryOperator, Quantifier
from .data_expression import DataExpression, Boolean, Integer

PREDICATE_INTEGERS = ["m", "n"]
PREDICATE_BOOLEANS = ["b", "c"]
QUANTIFIER_INTEGERS = ["t", "u", "v", "w"]
INTEGERS = PREDICATE_INTEGERS + QUANTIFIER_INTEGERS
BOOLEANS = PREDICATE_BOOLEANS

# As a convention we use that k, m, n are always natural numbers and
# b, c, d are always booleans.

@typechecked
def not_(x: Union[DataExpression, PropositionalVariable]) -> UnaryOperator:
    """
    Creates a negation operator.
    """
    return UnaryOperator("!", x)


@typechecked
def and_(x: Union[DataExpression, PropositionalVariable], y: Union[DataExpression, PropositionalVariable]) -> BinaryOperator:
    """
    Creates a conjunction operator.
    """
    return BinaryOperator("&&", x, y)


@typechecked
def or_(x: Union[DataExpression, PropositionalVariable], y: Union[DataExpression, PropositionalVariable]) -> BinaryOperator:
    """
    Creates a disjunction operator.
    """
    return BinaryOperator("||", x, y)


@typechecked
def implies(x: Union[DataExpression, PropositionalVariable], y: Union[DataExpression, PropositionalVariable]) -> BinaryOperator:
    """
    Creates an implication operator.
    """
    return BinaryOperator("=>", x, y)


@typechecked
def forall(x: Union[DataExpression, PropositionalVariable]) -> Quantifier:
    """
    Creates a universal quantifier.
    """
    var = "Natural"
    phi = x
    return Quantifier("forall", var, phi)


@typechecked
def exists(x: Union[DataExpression, PropositionalVariable]) -> Quantifier:
    """
    Creates an existential quantifier.
    """
    var = "Natural"
    phi = x
    return Quantifier("exists", var, phi)


@typechecked
def equal_to(x: Union[DataExpression, PropositionalVariable], y: Union[DataExpression, PropositionalVariable]) -> BinaryOperator:
    """
    Creates an equality operator.
    """
    return BinaryOperator("==", x, y)


@typechecked
def not_equal_to(x: Union[DataExpression, PropositionalVariable], y: Union[DataExpression, PropositionalVariable]) -> BinaryOperator:
    """
    Creates an inequality operator.
    """
    return BinaryOperator("!=", x, y)


operators = [not_, and_, or_, implies, forall, exists]


@typechecked
def is_boolean_constant(x: Union[DataExpression, PropositionalVariable]) -> bool:
    """
    Checks if a term is a boolean constant ('true' or 'false').
    """
    return isinstance(x, DataExpression) and x.value in ["false", "true"]


@typechecked
def is_natural_constant(x: Union[DataExpression, PropositionalVariable]) -> bool:
    """
    Checks if a term is a natural constant ('0' or '1').
    """
    return isinstance(x, DataExpression) and x.value in ["0", "1"]


@typechecked
def is_unary(op: Union[UnaryOperator, BinaryOperator]) -> bool:
    """
    Checks if an operator is unary.
    """
    return op in [not_, forall, exists]


@typechecked
def pick_element(s: List[str]) -> Tuple[str, List[str]]:
    """
    Picks a random element from a list and removes it.
    """
    n = random.randint(0, len(s) - 1)
    x = s[n]
    s = s[:n] + s[n + 1 :]
    return x, s


@typechecked
def pick_elements(s: List[str], n: int) -> List[str]:
    """
    Picks multiple random elements from a list.
    """
    result = []
    for _ in range(n):
        x, s = pick_element(s)
        result.append(x)
    return result


@typechecked
def make_val(s: str) -> str:
    """
    Wraps a string in a 'val()' expression.
    """
    return f"val({s})"


@typechecked
def make_predvar(n: int, use_integers: bool = True, size: int = random.randint(0, 2)) -> PredicateVariable:
    """
    Creates a predicate variable with random arguments.
    """
    name = f"X{n}"
    arguments = []
    variables = (
        PREDICATE_INTEGERS + PREDICATE_BOOLEANS if use_integers else PREDICATE_BOOLEANS
    )
    for _ in range(size):
        v, variables = pick_element(variables)
        arguments.append(v)
    return PredicateVariable(name, arguments)


@typechecked
def make_predvars(n: int, use_integers: bool) -> List[PredicateVariable]:
    """
    Creates a list of predicate variables.
    """
    return [make_predvar(i, use_integers, random.randint(0, 2)) for i in range(n)]


@typechecked
def make_atoms(freevars: List[str], add_val: bool = True) -> List[str]:
    """
    Creates a list of atomic formulas based on free variables.
    """
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


@typechecked
def make_boolean(freevars: List[str], add_val: bool = True) -> str:
    """
    Creates a random boolean formula.
    """
    atoms = make_atoms(freevars, add_val)
    x, _ = pick_element(atoms)
    return x


@typechecked
def make_natural(freevars: List[str], add_val: bool = True) -> str:
    """
    Creates a random natural number formula.
    """
    naturals = set(freevars).intersection(set(INTEGERS))
    result = ["0", "1"]
    for m in naturals:
        result.append(f"{m} + 1")
    x, _ = pick_element(result)
    return x


@typechecked
def make_predvar_instantiations(predvars: List[PredicateVariable]) -> List[PropositionalVariable]:
    """
    Instantiates predicate variables as propositional variables.
    """
    result = []
    for X in predvars:
        args = []
        for a in X.args:
            if a in BOOLEANS:
                args.append(Boolean("true"))
            elif a in INTEGERS:
                args.append(Integer("0"))
        result.append(PropositionalVariable(X.name, args))
    return result


@typechecked
def make_terms(predvars: List[PredicateVariable], m: int, n: int) -> List[Union[DataExpression, PropositionalVariable]]:
    """
    Creates a list of terms for a PBES equation.
    """
    result = [Boolean("true") for _ in range(m)]
    inst = make_predvar_instantiations(predvars)
    result += pick_elements(inst, n)
    return result


@typechecked
def join_terms(terms: List[Union[DataExpression, PropositionalVariable]]) -> List[Union[DataExpression, PropositionalVariable]]:
    """
    Combines terms using random operators.
    """
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


@typechecked
def make_pbes(
    equation_count: int,
    atom_count: int = 5,
    propvar_count: int = 3,
    use_quantifiers: bool = True,
    use_integers: bool = True,
) -> PBES:
    """
    Generates a random PBES.
    """
    global operators
    if use_quantifiers:
        operators = [not_, and_, or_, implies, not_, and_, or_, implies, forall, exists]
    else:
        operators = [not_, and_, or_, implies]
    while True:
        try:
            predvars = make_predvars(equation_count, use_integers)
            equations = []
            for i in range(equation_count):
                terms = make_terms(predvars, atom_count, propvar_count)
                while len(terms) > 1:
                    terms = join_terms(terms)
                sigma, _ = pick_element(["mu", "nu"])
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
            return p
        except Exception:
            pass


@typechecked
def main() -> None:
    """
    Main function to generate random PBES instances and write them to files.
    """
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
        write_text(file, str(pbes))


if __name__ == "__main__":
    main()
