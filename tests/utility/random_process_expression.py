#!/usr/bin/env python

# ~ Copyright 2012-2025 Wieger Wesselink, Maurice Laveaux.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import copy
import random
import re
from typing import List, Optional, Union, Dict, Callable

from typeguard import typechecked

import tests.utility.random_data_expression as random_data_expression
from .process_expression import (
    Action,
    MultiAction,
    Delta,
    Tau,
    ProcessInstance,
    Sum,
    IfThen,
    IfThenElse,
    Choice,
    Seq,
    Merge,
    LeftMerge,
    Allow,
    Block,
    Comm,
    Hide,
    Rename,
    StochasticOperator,
    ProcessIdentifier,
    ProcessExpression,
)
from .data_expression import Variable, Boolean, Integer, DataExpression
from .data_expression import Sort


@typechecked
class DistVariableGenerator(object):
    """
    Generate unique dist variables.
    TODO: Check if variable clashes between nested dists are allowed.
    """

    n: int = 0

    @staticmethod
    def generate() -> Variable:
        result = Variable(f"x{DistVariableGenerator.n}", Sort.BOOL)
        DistVariableGenerator.n += 1
        return result


@typechecked
class ProcessEquation:
    """
    Represents a process equation with a left-hand side (lhs) and a right-hand side (rhs).
    """

    def __init__(self, lhs: ProcessIdentifier, rhs: ProcessExpression):
        self.lhs: ProcessIdentifier = lhs
        self.rhs: ProcessExpression = rhs

    def __str__(self) -> str:
        lhs = self.lhs
        rhs = self.rhs
        return f"{lhs} = {rhs};"


@typechecked
class ProcessSpecification:
    """
    Represents a process specification consisting of actions, equations, and an initial state.
    """

    def __init__(self, actions, equations, init):
        self.actions = actions
        self.equations = equations
        self.init = init

    def __str__(self):
        actions = self.actions
        equations = self.equations
        init = self.init

        actspec = f"act\n  {','.join(actions)};\n"
        # Python 3.10 does not support backslash inside f-string interpolation.
        newline = "\n  "
        procspec = f"proc\n  {newline.join(map(str, equations))}\n"
        initspec = f"init\n  {str(init)};"
        return f"{actspec}\n{procspec}\n{initspec}"


@typechecked
def remove_postfix(name: str) -> str:
    """
    Removes the numeric postfix from a name.
    """
    return re.sub(r"\d+$", "", name)


@typechecked
def make_variable(forbidden_variables: List[Variable]) -> Variable:
    """
    Creates a new variable that does not clash with the given forbidden variables.
    """
    V = [x.name for x in forbidden_variables]
    for i in range(1, len(V) + 2):
        name = f"b{i}"
        if name not in V:
            return Variable(name, Sort.BOOL)


@typechecked
def default_value(v: Variable) -> DataExpression:
    """
    Returns a default value for the given variable.
    """
    assert isinstance(v, Variable)
    if v.type == Sort.BOOL:
        return random.choice([Boolean("true"), Boolean("false")])
    elif v.type == Sort.INT:
        return random.choice([Integer("0"), Integer("1"), Integer("2")])
    raise RuntimeError(f"default_value: only Bool and Int are supported! {v.type}")


@typechecked
def make_multi_action1(actions: List[str], size: int) -> MultiAction:
    """
    Creates a multi-action consisting of a random selection of actions.
    """
    result = []
    for _ in range(size):
        result.append(random.choice(actions))
    return MultiAction(sorted(result))


@typechecked
def expression_size(x: Union[ProcessExpression, MultiAction]) -> int:
    """
    Returns the size of a process expression.
    """
    if isinstance(x, MultiAction):
        return 1
    result = 0
    if isinstance(x, ProcessExpression) and not isinstance(x, StochasticOperator):
        result = result + 1
    for _, value in vars(x).items():
        if isinstance(value, ProcessExpression):
            result = result + expression_size(value)
    return result


@typechecked
def select_generators(
    generators: Dict["ProcGenerator", int],
    actions: List[str],
    process_identifiers: List[ProcessIdentifier],
    is_guarded: bool,
    size: int,
) -> List["ProcGenerator"]:
    """
    Returns the generator functions capable of producing a random process expression
    that satisfies the given constraints.
    """
    result = []

    if size >= 1:
        result = result + [DeltaGenerator(), TauGenerator()]
        if len(actions) > 0:
            result = result + [ActionGenerator(), MultiActionGenerator()]
        if len(process_identifiers) > 0 and not is_guarded:
            result = result + [ProcessInstanceGenerator()]

    if size >= 2:
        result = result + [SumGenerator(), IfThenGenerator(), DistGenerator()]

    if size >= 3:
        result = result + [IfThenElseGenerator(), ChoiceGenerator(), SeqGenerator()]

    r = []
    for x in result:
        for k, v in generators.items():
            if type(x) is type (k):
                r = r + [x] * v

    return r


@typechecked
class ProcGenerator:
    """
    Base class for generating process expressions.
    """

    def generate(
        self,
        process_expression_generators: Dict["ProcGenerator", int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> ProcessExpression:
        """
        Generates a process expression based on the specific implementation.
        """
        raise NotImplementedError("Subclasses must implement the generate method.")


@typechecked
class SumGenerator(ProcGenerator):
    """
    Generates a summation process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> Sum:
        d = make_variable(variables)
        x = make_process_expression(
            process_expression_generators,
            actions,
            process_identifiers,
            variables + [d],
            size - 1,
        )
        return Sum(d, x)


@typechecked
class ActionGenerator(ProcGenerator):
    """
    Generates an action process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> Action:
        a = random.choice(actions)
        return Action(a)


@typechecked
class MultiActionGenerator(ProcGenerator):
    """
    Generates a multi-action process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> MultiAction:
        n = random.randint(2, 4)
        result = [random.choice(actions) for _ in range(n)]
        return MultiAction(sorted(result))


@typechecked
class ProcessInstanceGenerator(ProcGenerator):
    """
    Generates a process instance expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> ProcessInstance:
        P = random.choice(process_identifiers)
        parameters = []
        for v in P.variables:
            if v.type == Sort.BOOL:
                parameters.append(
                    random_data_expression.make_boolean_data_expression(variables)
                )
            elif v.type == Sort.INT:
                parameters.append(
                    random_data_expression.make_integer_data_expression(variables)
                )
            else:
                raise RuntimeError(f"unknown type {v.type}")
        return ProcessInstance(P, parameters)


@typechecked
class DeltaGenerator(ProcGenerator):
    """
    Generates a delta process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> Delta:
        return Delta()


@typechecked
class TauGenerator(ProcGenerator):
    """
    Generates a tau process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> Tau:
        return Tau()


@typechecked
class DistGenerator(ProcGenerator):
    """
    Generates a stochastic operator process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> StochasticOperator:
        d = DistVariableGenerator.generate()
        x = make_process_expression(
            process_expression_generators,
            actions,
            process_identifiers,
            variables,
            size - 1,
        )
        distributions = [f"1/2", f"if({d.name},1/4,3/4)"]
        dist = random.choice(distributions)
        return StochasticOperator(d, dist, IfThen(d.name, x))


@typechecked
class IfThenGenerator(ProcGenerator):
    """
    Generates an if-then process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> IfThen:
        c = "true"
        x = make_process_expression(
            process_expression_generators,
            actions,
            process_identifiers,
            variables,
            size - 1,
        )
        return IfThen(c, x)


@typechecked
class IfThenElseGenerator(ProcGenerator):
    """
    Generates an if-then-else process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> IfThenElse:
        c = "true"
        x, y = make_two_process_expressions(
            process_expression_generators,
            actions,
            process_identifiers,
            variables,
            size - 1,
        )
        return IfThenElse(c, x, y)


@typechecked
class ChoiceGenerator(ProcGenerator):
    """
    Generates a choice process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> Choice:
        x, y = make_two_process_expressions(
            process_expression_generators,
            actions,
            process_identifiers,
            variables,
            size - 1,
        )
        return Choice(x, y)


@typechecked
class SeqGenerator(ProcGenerator):
    """
    Generates a sequential process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> Seq:
        x = make_process_expression(
            process_expression_generators,
            actions,
            process_identifiers,
            variables,
            size - 2,
        )
        y = make_process_expression(
            process_expression_generators,
            actions,
            process_identifiers,
            variables,
            size - expression_size(x) - 1,
        )
        return Seq(x, y)


@typechecked
class LeftMergeGenerator(ProcGenerator):
    """
    Generates a left-merge process expression.
    """

    def generate(
        self,
        process_expression_generators: Dict[ProcGenerator, int],
        actions: List[str],
        process_identifiers: List[ProcessIdentifier],
        variables: List[Variable],
        size: int,
    ) -> LeftMerge:
        x = make_process_expression(
            process_expression_generators,
            actions,
            process_identifiers,
            variables,
            size - 2,
        )
        y = make_process_expression(
            process_expression_generators,
            actions,
            process_identifiers,
            variables,
            size - expression_size(x) - 1,
        )
        return LeftMerge(x, y)


@typechecked
def make_hide_set(actions: List[str], size: int) -> List[str]:
    """
    Creates a set of actions to hide.
    """
    if size > len(actions):
        return actions
    return random.sample(actions, size)


@typechecked
def make_hide(actions: List[str], x: ProcessExpression) -> Hide:
    """
    Wraps a process expression with a hide operator.
    """
    I = make_hide_set(actions, 1)
    return Hide(I, x)


@typechecked
def make_rename_set(actions: List[str], size: int) -> List[str]:
    """
    Creates a set of rename mappings.
    """
    result = []
    A = copy.deepcopy(actions)
    for _ in range(size):
        if len(A) < 2:
            break
        a, b = random.sample(A, 2)
        A.remove(a)
        A.remove(b)
        result.append(f"{a} -> {b}")
    return result


@typechecked
def make_rename(actions: List[str], x: ProcessExpression) -> Rename:
    """
    Wraps a process expression with a rename operator.
    """
    R = make_rename_set(actions, 1)
    return Rename(R, x)


@typechecked
def make_comm_set(actions: List[str], size: int) -> List[str]:
    """
    Creates a set of communication mappings.
    """
    result = []
    A = set(actions)
    for _ in range(size):
        if len(A) < 2:
            break
        a = random.choice(list(A))
        b = random.choice(list(A))
        A.remove(a)
        if a != b:
            A.remove(b)
        c = random.choice(list(A))
        result.append(f"{a} | {b} -> {c}")
    return result


@typechecked
def make_comm(actions: List[str], x: ProcessExpression) -> Comm:
    """
    Wraps a process expression with a communication operator.
    """
    C = make_comm_set(actions, 1)
    return Comm(C, x)


@typechecked
def make_allow_set(actions: List[str], size: int) -> List[MultiAction]:
    """
    Creates a set of allowed multi-actions.
    """
    result = set([])
    for _ in range(size):
        n = random.randint(1, 2)
        alpha = make_multi_action1(actions, n)
        result.add(alpha)
    return list(result)


@typechecked
def make_allow(actions: List[str], x: ProcessExpression) -> Allow:
    """
    Wraps a process expression with an allow operator.
    """
    V = make_allow_set(actions, 5)
    return Allow(V, x)


default_process_expression_generators: Dict[ProcGenerator, int] = {
    ActionGenerator(): 8,
    DeltaGenerator(): 1,
    TauGenerator(): 1,
    ProcessInstanceGenerator(): 2,
    SumGenerator(): 2,
    IfThenGenerator(): 2,
    IfThenElseGenerator(): 2,
    ChoiceGenerator(): 5,
    SeqGenerator(): 5,
    MultiActionGenerator(): 1,
    DistGenerator(): 0,
}


@typechecked
def make_block(actions: List[str], x: ProcessExpression) -> Block:
    """
    Wraps a process expression with a block operator.
    """
    B = make_block_set(actions, 1)
    return Block(B, x)


@typechecked
def make_block_set(actions: List[str], size: int) -> List[str]:
    """
    Creates a set of blocked actions.
    """
    return random.sample(actions, size)


@typechecked
def make_process_expression(
    process_expression_generators: Dict[ProcGenerator, int],
    actions: List[str],
    process_identifiers: List[ProcessIdentifier],
    variables: List[Variable],
    size: int,
) -> ProcessExpression:
    """
    Generates a random process expression.
    """
    generators = select_generators(
        process_expression_generators,
        actions,
        process_identifiers,
        size=size,
        is_guarded=True,
    )
    generator = random.choice(generators)
    result = generator.generate(
        process_expression_generators,
        actions,
        process_identifiers,
        variables,
        size,
    )
    if expression_size(result) > size:
        raise RuntimeError(f"The generated expression has the wrong size! {result}")
    assert 1 <= expression_size(result) <= size
    return result


@typechecked
def make_two_process_expressions(
    process_expression_generators: Dict[ProcGenerator, int],
    actions: List[str],
    process_identifiers: List[ProcessIdentifier],
    variables: List[Variable],
    size: int,
) -> tuple[ProcessExpression, ProcessExpression]:
    """
    Generates two random process expressions.
    """
    assert size >= 2
    n = random.randint(1, size - 1)
    x = make_process_expression(
        process_expression_generators,
        actions,
        process_identifiers,
        variables,
        n,
    )
    y = make_process_expression(
        process_expression_generators,
        actions,
        process_identifiers,
        variables,
        size - expression_size(x),
    )
    return x, y


@typechecked
def make_parallel_process_expression(
    actions: List[str],
    process_expressions: List[ProcessExpression],
    size: int,
    parallel_operator_generators: Optional[List[Callable]] = None,
) -> ProcessExpression:
    """
    Generates a random process expression by wrapping parallel operators around elements of process_expressions.
    """
    if parallel_operator_generators is None:
        parallel_operator_generators = [
            make_block,
            make_hide,
            make_rename,
            make_comm,
            make_allow,
        ]
    V = copy.deepcopy(process_expressions)
    x = None
    while len(V) > 1 or size > 0:
        if random.choice([True, False]) and len(V) > 1:
            p, q = random.sample(V, 2)
            V.remove(p)
            V.remove(q)
            V.append(Merge(p, q))
        elif size > 0:
            p = random.choice(V)
            V.remove(p)
            f = random.choice(parallel_operator_generators)
            V.append(f(actions, p))
            size = size - 1
    return V[0]


@typechecked
def make_parallel_process_expression_old(
    actions: List[str],
    process_expressions: List[ProcessExpression],
    size: int,
    parallel_operator_generators: Optional[List[Callable]] = None,
) -> ProcessExpression:
    """
    Generates a random process expression by wrapping parallel operators around elements of process_expressions
    using an older method.
    """
    if parallel_operator_generators is None:
        parallel_operator_generators = [
            make_block,
            make_hide,
            make_rename,
            make_comm,
            make_allow,
        ]
    V = copy.deepcopy(process_expressions)
    x = None
    while len(V) > 0:
        if x is None:
            p, q = random.sample(V, 2)
            V.remove(p)
            V.remove(q)
            x = Merge(p, q)
        else:
            p = random.choice(V)
            V.remove(p)
            x = Merge(x, p)

    for _ in range(size):
        f = random.choice(parallel_operator_generators)
        x = f(actions, x)

    return x


default_parallel_operator_generators: List[Callable] = [
    make_block,
    make_hide,
    make_rename,
    make_comm,
    make_allow,
]


@typechecked
def parse_variable(text: str) -> Variable:
    """
    Parses a string representation of a variable with its type.
    """
    text = text.strip()
    m = re.match(r"([^,:]+)\s*\:(.+)", text)
    assert m is not None
    result = Variable(m.group(1).strip(), Sort(m.group(2).strip()))
    return result


@typechecked
def parse_variables(text: str) -> list[Variable]:
    """
    Parses a comma-separated list of variable declarations.
    """
    variables = map(str.strip, text.split(","))
    return list(map(parse_variable, variables))


@typechecked
def make_process_specification(
    parallel_operator_generators: Optional[List[Callable]] = None,
    process_expression_generators: Optional[Dict[ProcGenerator, int]] = None,
    actions: Optional[List[str]] = None,
    process_identifiers_names: Optional[List[str]] = None,
    size: int = 13,
    init: Optional[ProcessExpression] = None,
    generate_process_parameters: bool = False,
) -> ProcessSpecification:
    """
    Generates a random process specification.
    """
    if parallel_operator_generators is None:
        parallel_operator_generators = default_parallel_operator_generators
    if process_expression_generators is None:
        process_expression_generators = default_process_expression_generators
    if actions is None:
        actions = ["a", "b", "c", "d"]
    if process_identifiers_names is None:
        process_identifiers_names = ["P", "Q", "R"]
    process_identifiers = list(map(ProcessIdentifier, process_identifiers_names))

    if generate_process_parameters:
        variables = parse_variables(
            "c1: Bool, c2: Bool, c3: Bool, i1: Int, i2: Int, i3: Int"
        )
        for i, process in enumerate(process_identifiers):
            if not process.variables:
                n = random.randint(0, 3)
                process_identifiers[i].variables = random.sample(variables, n)

    variables = []
    equations = []
    for process in process_identifiers:
        x = make_process_expression(
            process_expression_generators,
            actions,
            process_identifiers,
            variables + process.variables,
            size,
        )
        equations.append(ProcessEquation(process, x))
    n = random.randint(0, 5)
    if not init:
        process_instances = [
            ProcessInstance(x, list(map(default_value, x.variables)))
            for x in process_identifiers
        ]
        init = make_parallel_process_expression(
            actions, process_instances, n, parallel_operator_generators
        )
    return ProcessSpecification(list(set(actions)), equations, init)


if __name__ == "__main__":
    procspec = make_process_specification(generate_process_parameters=True)
    print(procspec)
