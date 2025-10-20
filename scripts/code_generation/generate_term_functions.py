#!/usr/bin/env python3

# ~ Copyright 2007 Wieger Wesselink.
# ~ Distributed under the Boost Software License, Version 1.0.
# ~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

#pylint: disable=line-too-long
#pylint: disable=missing-function-docstring
#pylint: disable=invalid-name

import re
import sys
from typing import List, Any

import tpg
from typeguard import typechecked

from mcrl2_parser import EBNFParser, Mcrl2Actions, read_paragraphs
from mcrl2_utility import insert_text_in_file

MCRL2_ROOT: str = "../../"

@typechecked
def generate_function_symbol_constructors(
    rules: List[Any], declaration_filename: str, definition_filename: str, skip_list: List[str]
) -> bool:
    """Generate function symbol constructors."""

    CODE = """// {name}
inline
const atermpp::function_symbol& function_symbol_{name}()
{{
  static const atermpp::global_function_symbol function_symbol_{name}("{name}", {arity});
  return function_symbol_{name};
}}

"""

    ctext = ""  # constructors
    dtext = ""  # definitions
    vtext = ""  # variables
    names = {}
    calls = {}
    decls = {}

    functions = find_functions(rules)

    for f in functions:
        name = f.name()
        if name in skip_list:
            continue
        names[name] = f.arity()
        calls[name] = f.default_call()
        decls[name] = f.default_declaration()

        dtext = (
            dtext
            + f"  const atermpp::function_symbol core::detail::function_symbols::{name} = core::detail::function_symbol_{name}();\n"
        )
        vtext = vtext + f"  static const atermpp::function_symbol {name};\n"

    name_keys = list(names.keys())
    name_keys = sorted(name_keys)
    for name in name_keys:
        if name in skip_list:
            continue
        arity = names[name]
        ctext = ctext + CODE.format(name=name, arity=arity)

    result = insert_text_in_file(declaration_filename, ctext, "generated constructors")
    result = result and insert_text_in_file(
        definition_filename, dtext, "generated function symbol definitions"
    )
    result = result and insert_text_in_file(
        declaration_filename, vtext, "generated variables"
    )
    return result


@typechecked
def generate_default_values(
    rules: List[Any], declaration_filename: str, definition_filename: str, skip_list: List[str]
) -> bool:
    """Generate default values."""

    TERM_FUNCTION = """// {name}
inline
const atermpp::aterm& default_value_{name}()
{{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_{name}(){arguments});
  return t;
}}

"""

    RULE_FUNCTION = """// {name}
inline
const atermpp::aterm& default_value_{name}()
{{
  return default_value_{fname}();
}}

"""
    ptext = ""  # constructor prototypes
    ctext = ""  # constructors
    dtext = ""  # definitions
    vtext = ""  # variables

    functions = find_functions(rules)

    for f in functions:
        name = f.name()
        if name in skip_list:
            continue
        ptext = ptext + f"const atermpp::aterm& default_value_{f.name()}();\n"
        arity = f.arity()
        args = []
        for x in f.arguments:
            if x.repetitions == "":
                args.append(f"default_value_{x.name()}()")
            elif x.repetitions == "*":
                args.append("default_value_List()")
            else:
                args.append(f"default_value_List(default_value_{x.name()}())")

        if len(args) > 0:
            arguments = ", " + ", ".join(args)
        else:
            arguments = ""
        ctext = ctext + TERM_FUNCTION.format(
            name=name,
            arity=arity,
            arguments=arguments,
        )
        dtext = (
            dtext
            + f"  const atermpp::aterm core::detail::default_values::{name} = core::detail::default_value_{name}();\n"
        )
        vtext = vtext + f"  static const atermpp::aterm {name};\n"

    function_names = [x.name() for x in functions]
    for rule in rules:
        if rule.name() not in function_names:
            name = rule.name()
            fname = ""
            if name in skip_list:
                continue
            for f in rule.rhs:
                if (
                    f.phase is None
                    or not f.phase.startswith("-")
                    or not f.phase.startswith(".")
                ):
                    fname = f.name()
                    break
            ptext = ptext + f"const atermpp::aterm& default_value_{name}();\n"
            ctext = ctext + RULE_FUNCTION.format(name=name, fname=fname)
            dtext = (
                dtext
                + f"  const atermpp::aterm core::detail::default_values::{name} = core::detail::default_value_{name}();\n"
            )
            vtext = vtext + f"  static const atermpp::aterm {name};\n"

    ctext = ptext + "\n" + ctext
    result = insert_text_in_file(declaration_filename, ctext, "generated constructors")
    result = result and insert_text_in_file(
        definition_filename, dtext, "generated default value definitions"
    )
    result = result and insert_text_in_file(
        declaration_filename, vtext, "generated variables"
    )
    return result


# ---------------------------------------------------------------#
#                          find_functions
# ---------------------------------------------------------------#
# find all functions that appear in the rhs of a rule
@typechecked
def find_functions(rules: List[Any]) -> List[Any]:
    """Find all functions that appear in the rhs of a rule."""

    function_map = {}
    for rule in rules:
        for f in rule.functions():
            if not f.is_rule():
                function_map[f.name()] = f

    # do a recursion step to find additional functions (no longer necessary?)
    functions = [function_map[x] for x in list(function_map.keys())]
    for f in functions:
        for arg in f.arguments:
            for e in arg.expressions:
                if not e.is_rule():
                    function_map[e.name()] = e

    return [function_map[x] for x in list(function_map.keys())]


# ---------------------------------------------------------------#
#                      generate_soundness_check_functions
# ---------------------------------------------------------------#
# generates C++ code for checking if terms are in the right format
#
@typechecked
def generate_soundness_check_functions(rules: List[Any], filename: str, skip_list: List[str]) -> bool:
    """
    Generate C++ code for checking if terms are in the right format.
    """

    CHECK_RULE = """template <atermpp::IsATerm Term>
bool check_rule_{name}([[maybe_unused]] const Term& t)
{{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
{body}
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}}

"""

    CHECK_TERM = """// {name}({arguments})
template <atermpp::IsATerm Term>
bool {check_name}([[maybe_unused]] const Term& t)
{{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
{body}
#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}}

"""
    CHECK_TERM_TYPE = """  // check the type of the term
  const atermpp::aterm& term(t);
  if (!term.type_is_appl())
  {{
    return false;
  }}
  if (term.function() != core::detail::function_symbols::{name})
  {{
    return false;
  }}

"""

    CHECK_TERM_CHILDREN = """  // check the children
  if (term.size() != {arity})
  {{
    return false;
  }}
"""

    text = ""  # function definitions
    ptext = ""  # function declarations (prototypes)

    functions = find_functions(rules)

    for rule in rules:
        name = rule.name()
        if name in skip_list:
            continue
        rhs_functions = rule.functions()
        body = (
            "  return "
            + "\n         || ".join([x.check_name() + "(t)" for x in rhs_functions])
            + ";"
        )
        text = text + CHECK_RULE.format(name=name, body=body)
        ptext = (
            ptext
            + f"template <atermpp::IsATerm Term> bool check_rule_{rule.name()}(const Term& t);\n"
        )

    for f in functions:
        name = f.name()
        if name in skip_list:
            continue
        arguments = ", ".join([x.full_name() for x in f.arguments])
        arity = len(f.arguments)

        body = CHECK_TERM_TYPE.format(name=name)
        body = body + CHECK_TERM_CHILDREN.format(arity=len(f.arguments))
        if arity > 0:
            body = body + "#ifndef MCRL2_NO_RECURSIVE_SOUNDNESS_CHECKS\n"
            for i in range(arity):
                arg = f.arguments[i]
                if arg.repetitions == "":
                    body = (
                        body
                        + f"  if (!check_term_argument(term[{i}], {arg.check_name()}<atermpp::aterm>))\n"
                    )
                elif arg.repetitions == "*":
                    body = (
                        body
                        + f"  if (!check_list_argument(term[{i}], {arg.check_name()}<atermpp::aterm>, 0))\n"
                    )
                elif arg.repetitions == "+":
                    body = (
                        body
                        + f"  if (!check_list_argument(term[{i}], {arg.check_name()}<atermpp::aterm>, 1))\n"
                    )
                body = body + "  {\n"
                body = body + f'    mCRL2log(log::debug) << "{arg.check_name()}" << std::endl;\n'
                body = body + "    return false;\n"
                body = body + "  }\n"
            body = body + "#endif // MCRL2_NO_RECURSIVE_SOUNDNESS_CHECKS\n"

        text = text + CHECK_TERM.format(
            name=name,
            arguments=arguments,
            check_name=f.check_name(),
            body=body,
        )
        ptext = (
            ptext
            + f"template <atermpp::IsATerm Term> bool {f.check_name()}(const Term& t);\n"
        )

    text = ptext + "\n" + text.strip()
    text = text + "\n"
    return insert_text_in_file(filename, text, "generated code")


# ---------------------------------------------------------------#
#                          parse_ebnf
# ---------------------------------------------------------------#
@typechecked
def parse_ebnf(filename: str) -> List[Any]:
    """Parse EBNF grammar file."""

    rules: List[Any] = []

    paragraphs: List[str] = read_paragraphs(filename)
    for paragraph in paragraphs:
        # --- skip special paragraphs
        if re.match("// Date", paragraph):
            continue
        if re.match("//Specification", paragraph):
            continue
        if re.match("//Expressions", paragraph):
            continue

        # --- handle other paragraphs
        lines: List[str] = paragraph.split("\n")
        clines: List[str] = []  # comment lines
        glines: List[str] = []  # grammar lines
        for line in lines:
            if re.match(r"\s*//.*", line):
                clines.append(line)
            else:
                glines.append(line)
        comment: str = "\n".join(clines)

        parser: EBNFParser = EBNFParser(Mcrl2Actions())
        try:
            newrules: List[Any] = parser("\n".join(glines))
            for rule in newrules:
                rule.comment = comment
            rules = rules + newrules
        except tpg.SyntacticError as e:
            print("------------------------------------------------------")
            print(("grammar: ", "\n".join(glines)))
            print(e)
        except tpg.LexicalError as e:
            print("------------------------------------------------------")
            print(("grammar: ", "\n".join(glines)))
            print(e)
    return rules


# ---------------------------------------------------------------#
#                          main
# ---------------------------------------------------------------#
@typechecked
def main() -> int:
    """Main function to generate term functions."""

    result: bool = True
    filename: str = MCRL2_ROOT + "scripts/code_generation/mcrl2.internal.txt"
    rules: List[Any] = parse_ebnf(filename)

    # elements in this list are skipped during generation
    skip_list: List[str] = ["DataAppl"]

    result = (
        generate_soundness_check_functions(
            rules,
            MCRL2_ROOT + "libraries/core/include/mcrl2/core/detail/soundness_checks.h",
            skip_list,
        )
        and result
    )

    declaration_filename = (
        MCRL2_ROOT + "libraries/core/include/mcrl2/core/detail/function_symbols.h"
    )
    definition_filename = MCRL2_ROOT + "libraries/core/source/core.cpp"
    result = (
        generate_function_symbol_constructors(
            rules, declaration_filename, definition_filename, skip_list
        )
        and result
    )

    declaration_filename = (
        MCRL2_ROOT + "libraries/core/include/mcrl2/core/detail/default_values.h"
    )
    definition_filename = MCRL2_ROOT + "libraries/core/source/core.cpp"
    result = (
        generate_default_values(
            rules, declaration_filename, definition_filename, skip_list
        )
        and result
    )

    return int(not result) # 0 result indicates successful execution


if __name__ == "__main__":
    sys.exit(main())
