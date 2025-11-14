#!/usr/bin/env python

# ~ Copyright 2018-2025 Wieger Wesselink, Maurice Laveaux.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import random
import argparse
import sys

from enum import Enum
from dataclasses import dataclass
from typing import List

from .data_expression import DataExpression, Variable as DataVariable
from .random_data_expression import (
    make_boolean_data_expression,
    make_integer_data_expression,
)


class BinaryOperator(Enum):
    """Binary operators."""

    AND = "&&"
    OR = "||"


class FixedPointOperator(Enum):
    """Fixed point operators."""

    MU = "mu"
    NU = "nu"


class ActionType(Enum):
    NAME = "name"
    SEQUENCE = "sequence"
    ALTERNATIVE = "alternative"
    REPETITION = "repetition"


class FormulaType(Enum):
    CONSTANT = "constant"
    BINARY = "binary"
    MODAL = "modal"
    FIXEDPOINT = "fixedpoint"


@dataclass(frozen=True)
class Formula:
    """Base class for modal formulas."""

    def __str__(self) -> str:
        raise NotImplementedError("Subclasses should implement this method.")


@dataclass(frozen=True)
class TrueFormula(Formula):
    """Represents the 'true' constant."""

    def __str__(self) -> str:
        return "true"


@dataclass(frozen=True)
class FalseFormula(Formula):
    """Represents the 'false' constant."""

    def __str__(self) -> str:
        return "false"


@dataclass(frozen=True)
class Variable:
    """Represents a variable in a formula."""

    name: str
    parameters: tuple[DataVariable] = None
    initial: tuple[DataExpression] = None

    def __str__(self) -> str:
        if not self.parameters:
            return self.name

        bound_vars = ", ".join(
            f"{var.parameter()} = {expr}"
            for (var, expr) in zip(self.parameters, self.initial)
        )
        return f"{self.name}({bound_vars})"


@dataclass(frozen=True)
class VariableInstanceFormula(Formula):
    """Represents a variable instance with concrete argument values."""

    variable: Variable
    arguments: List[DataExpression]  # List of expressions mapped to bound variables

    def __str__(self) -> str:
        if not self.arguments:
            return self.variable.name

        args = ", ".join(str(arg) for arg in self.arguments)
        return f"{self.variable.name}({args})"


@dataclass(frozen=True)
class BinaryFormula(Formula):
    """Base class for binary operators."""

    left: Formula
    right: Formula
    operator: BinaryOperator

    def __str__(self) -> str:
        return f"({self.left} {self.operator.value} {self.right})"


@dataclass(frozen=True)
class ActionFormula:
    """Base class for action formulas."""

    def __str__(self) -> str:
        raise NotImplementedError("Subclasses should implement this method.")


@dataclass(frozen=True)
class ActionName(ActionFormula):
    """Represents a basic action name."""

    name: str

    def __str__(self) -> str:
        return self.name


@dataclass(frozen=True)
class SequenceAction(ActionFormula):
    """Represents sequential composition of actions."""

    left: ActionFormula
    right: ActionFormula

    def __str__(self) -> str:
        return f"({self.left}.{self.right})"


@dataclass(frozen=True)
class AlternativeAction(ActionFormula):
    """Represents alternative composition of actions."""

    left: ActionFormula
    right: ActionFormula

    def __str__(self) -> str:
        return f"({self.left}+{self.right})"


@dataclass(frozen=True)
class RepetitionAction(ActionFormula):
    """Represents Kleene star repetition of an action."""

    action: ActionFormula

    def __str__(self) -> str:
        return f"({self.action})*"


@dataclass(frozen=True)
class ModalFormula(Formula):
    """Base class for modal operators."""

    action: ActionFormula
    subformula: Formula

    def __str__(self) -> str:
        raise NotImplementedError("Subclasses should implement this method.")


@dataclass(frozen=True)
class BoxFormula(ModalFormula):
    """Represents the box (necessity) modal operator."""

    def __str__(self) -> str:
        return f"[{self.action}]{self.subformula}"


@dataclass(frozen=True)
class DiamondFormula(ModalFormula):
    """Represents the diamond (possibility) modal operator."""

    def __str__(self) -> str:
        return f"<{self.action}>{self.subformula}"


@dataclass(frozen=True)
class FixedPointFormula(Formula):
    """Base class for fixed point operators."""

    variable: Variable
    subformula: Formula
    operator: FixedPointOperator

    def __str__(self) -> str:
        return f"{self.operator.value} {self.variable}.({self.subformula})"


def generate_random_action_formula(action_names, max_depth=3):
    """
    Generate a random action formula.

    Parameters:
    - action_names: List of action names to use
    - max_depth: Maximum depth of the action formula tree

    Returns:
    - An ActionFormula object
    """
    if not action_names:
        raise ValueError("At least one action name must be provided")

    # Base case for recursion - at max depth use simple action names
    if max_depth <= 0 or random.random() < 0.4:
        action_name = random.choice(action_names)
        return ActionName(action_name)

    action_type = random.choice(list(ActionType))

    if action_type == ActionType.NAME:
        action_name = random.choice(action_names)
        return ActionName(action_name)

    elif action_type == ActionType.SEQUENCE:
        left = generate_random_action_formula(action_names, max_depth - 1)
        right = generate_random_action_formula(action_names, max_depth - 1)
        return SequenceAction(left, right)

    elif action_type == ActionType.ALTERNATIVE:
        left = generate_random_action_formula(action_names, max_depth - 1)
        right = generate_random_action_formula(action_names, max_depth - 1)
        return AlternativeAction(left, right)

    else:  # action_type == ActionType.REPETITION
        action = generate_random_action_formula(action_names, max_depth - 1)
        return RepetitionAction(action)


class RandomStateFormulaGenerator:
    """Generate a random state formula."""

    fixed_point_counter: int = 0

    def generate(
        self,
        action_names,
        max_depth=5,
        max_fixedpoints=2,
        existing_variables=None,
    ):
        """
        Generate a random state formula.

        Parameters:
        - action_names: List of action names to use
        - max_depth: Maximum depth of the formula tree
        - max_fixedpoints: Maximum number of fixedpoint formulas to introduce
        - existing_variables: Set of variable names already in scope

        Returns:
        - A Formula object
        """
        if existing_variables is None:
            existing_variables = set()

        # Base case for recursion
        if max_depth <= 0:
            if not existing_variables or random.random() < 0.3:
                return random.choice([TrueFormula(), FalseFormula()])
            else:
                var_name = random.choice(list(existing_variables))
                expressions = []
                if var_name.parameters:
                    for arg in var_name.parameters:
                        if arg.type == "Nat":
                            expressions.append(
                                make_integer_data_expression(list(var_name.parameters))
                            )
                        elif arg.type == "Bool":
                            expressions.append(
                                make_boolean_data_expression(list(var_name.parameters))
                            )
                        else:
                            raise ValueError(f"Unknown type: {arg.type}")

                return VariableInstanceFormula(var_name, expressions)

        formula_types = [FormulaType.CONSTANT, FormulaType.BINARY]

        if max_depth > 1:
            formula_types.append(FormulaType.MODAL)
            if self.fixed_point_counter < max_fixedpoints:
                formula_types.append(FormulaType.FIXEDPOINT)

        formula_type = random.choice(formula_types)

        if formula_type == FormulaType.CONSTANT:
            return random.choice([TrueFormula(), FalseFormula()])

        elif formula_type == FormulaType.BINARY:
            operator = random.choice(list(BinaryOperator))
            left = self.generate(
                action_names,
                max_depth - 1,
                max_fixedpoints,
                existing_variables,
            )
            right = self.generate(
                action_names,
                max_depth - 1,
                max_fixedpoints,
                existing_variables,
            )
            return BinaryFormula(left, right, operator)

        elif formula_type == FormulaType.MODAL:
            action = generate_random_action_formula(action_names, max_depth - 1)
            subformula = self.generate(
                action_names,
                max_depth - 1,
                max_fixedpoints,
                existing_variables,
            )
            return (
                BoxFormula(action, subformula)
                if random.random() < 0.5
                else DiamondFormula(action, subformula)
            )

        else:  # formula_type == FormulaType.FIXEDPOINT
            var_name = f"X{self.fixed_point_counter}"
            bound_vars = []
            initial = []
            if random.random() < 0.3:
                num_bound_vars = random.randint(1, 3)
                for i in range(num_bound_vars):
                    datatype = random.choice(["Nat", "Bool"])
                    bound_vars.append(DataVariable(f"v{i}", datatype))

                    if datatype == "Nat":
                        initial.append(make_integer_data_expression([]))
                    elif datatype == "Bool":
                        initial.append(make_boolean_data_expression([]))

            variable = Variable(var_name, tuple(bound_vars), tuple(initial))

            self.fixed_point_counter = self.fixed_point_counter + 1
            new_existing_variables = set(existing_variables)
            new_existing_variables.add(variable)

            subformula = self.generate(
                action_names,
                max_depth - 1,
                max_fixedpoints,
                new_existing_variables,
            )

            operator = random.choice(list(FixedPointOperator))
            return FixedPointFormula(variable, subformula, operator)


FORMULAS = """nu X.<a || b || c || d>X && [a]false
([true*.a]mu X.[!d]X) && ([true*.a]mu X.[!c]X) && ([true*.a]mu X.[!b]X)
[true*](([a](nu X. mu Y. ([b]X && [!b]Y))))
[true*]([a](nu X. mu Y. ([b]X && [!b]Y)))
[true*]([a](nu X. mu Y. ([c]X && [!c]Y)))
<true*>(<a>(nu X. mu Y. (<c>X || <!d && !b>Y)))
[true*] [a.(!a && !c)*.c.(!a)*.c]false
[true*](mu Y. ([!a]Y && <true>true))
([true*] nu X. mu Y. nu Z. ([a]X && ([a]false || [!a]Y) && [!a]Z))
[true*] nu X. mu Y. nu Z. ([a]X && ([a]false || [!a]Y) && [!a]Z)
[true*]<true>true
<true*.a>true
nu X. mu Y. (<a>X || <!a>Y)
nu X. mu Y. (<a>X || <!b>Y)
nu X. (([!a]X && [b]false))
[true*.(a + b)]mu X.[!c]X
[true*]mu X.[a]X
[!a*.b]false && [!c*.b]false && [!d*.b]false && [true*.c.!a*.b]false
[true*] [a.(!a && !b)*.b.(!a)*.b]false
[true*](<a>true => <b>true)"""


def make_modal_formula() -> str:
    """ " Generate a random modal formula with actions a, b and c."""
    if random.random() < 0.5:
        # Return a random formula from the predefined set
        return random.choice(FORMULAS.splitlines())

    return RandomStateFormulaGenerator().generate(
        action_names=["a", "b", "c"],
        max_depth=5,
        max_fixedpoints=3,
    )


def main():
    parser = argparse.ArgumentParser(description="Generate a random state formula.")
    parser.add_argument(
        "-a",
        "--actions",
        type=str,
        default="a,b,c",
        help='Comma-separated list of action names (default: "a,b,c")',
    )
    parser.add_argument(
        "-d",
        "--depth",
        type=int,
        default=5,
        help="Maximum depth of the formula tree (default: 5)",
    )
    parser.add_argument(
        "-f",
        "--fixedpoints",
        type=int,
        default=2,
        help="Maximum number of fixed point formulas (default: 2)",
    )
    parser.add_argument(
        "-s", "--seed", type=float, help="Random seed for reproducibility"
    )

    args = parser.parse_args()

    # Set random seed if provided
    if args.seed is not None:
        random.seed(args.seed)

    # Parse action names
    action_names = args.actions.split(",")

    # Generate the formula
    formula = RandomStateFormulaGenerator().generate(
        action_names=action_names,
        max_depth=args.depth,
        max_fixedpoints=args.fixedpoints,
    )

    # Print the formula
    print(formula)

    return 0


if __name__ == "__main__":
    sys.exit(main())
