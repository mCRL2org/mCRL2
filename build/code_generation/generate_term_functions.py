#!/usr/bin/env python3

#~ Copyright 2007 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
import sys
from mcrl2_parser import *
from mcrl2_utility import *

MCRL2_ROOT = '../../'

#---------------------------------------------------------------#
#            generate_function_symbol_constructors
#---------------------------------------------------------------#
#
def generate_function_symbol_constructors(rules, declaration_filename, definition_filename, skip_list):
    CODE = '''// %(name)s
inline
const atermpp::function_symbol& function_symbol_%(name)s()
{
  static const atermpp::global_function_symbol function_symbol_%(name)s("%(name)s", %(arity)d);
  return function_symbol_%(name)s;
}

'''

    ctext = '' # constructors
    dtext = '' # definitions
    vtext = '' # variables
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

        dtext = dtext + '  const atermpp::function_symbol core::detail::function_symbols::%s = core::detail::function_symbol_%s();\n' % (name, name)
        vtext = vtext + '  static const atermpp::function_symbol %s;\n' % name

    name_keys = list(names.keys())
    name_keys = sorted(name_keys)
    for name in name_keys:
        if name in skip_list:
            continue
        arity = names[name]
        ctext = ctext + CODE % {
            'name'  : name,
            'arity' : arity,
        }
        comma = ''
        if calls[name] != "":
            comma = ', '

    result = insert_text_in_file(declaration_filename, ctext, 'generated constructors')
    result = result and insert_text_in_file(definition_filename, dtext, 'generated function symbol definitions')
    result = result and insert_text_in_file(declaration_filename, vtext, 'generated variables')
    return result

#---------------------------------------------------------------#
#                      generate_default_values
#---------------------------------------------------------------#
#
def generate_default_values(rules, declaration_filename, definition_filename, skip_list):
    TERM_FUNCTION = '''// %(name)s
inline
const atermpp::aterm_appl& default_value_%(name)s()
{
  static const atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_%(name)s()%(arguments)s);
  return t;
}

'''

    RULE_FUNCTION = '''// %(name)s
inline
const atermpp::aterm_appl& default_value_%(name)s()
{
  return default_value_%(fname)s();
}

'''
    ptext = '' # constructor prototypes
    ctext = '' # constructors
    dtext = '' # definitions
    vtext = '' # variables

    functions = find_functions(rules)

    for f in functions:
        name  = f.name()
        if name in skip_list:
            continue
        ptext = ptext + 'const atermpp::aterm_appl& default_value_%s();\n' % f.name()
        arity = f.arity()
        args = []
        for x in f.arguments:
            if x.repetitions == '':
                args.append('default_value_%s()' % x.name())
            elif x.repetitions == '*':
                args.append('default_value_List()')
            else:
                args.append('default_value_List(default_value_%s())' % x.name())

        if len(args) > 0:
            arguments = ', ' + ', '.join(args)
        else:
            arguments = ''
        ctext = ctext + TERM_FUNCTION % {
            'name'       : name,
            'arity'      : arity,
            'arguments'  : arguments,
        }
        dtext = dtext + '  const atermpp::aterm_appl core::detail::default_values::%s = core::detail::default_value_%s();\n' % (name, name)
        vtext = vtext + '  static const atermpp::aterm_appl %s;\n' % name

    function_names = [x.name() for x in functions]
    for rule in rules:
        if not rule.name() in function_names:
            name = rule.name()
            if name in skip_list:
                continue
            for f in rule.rhs:
                if f.phase == None or not f.phase.startswith('-') or not f.phase.startswith('.'):
                    fname = f.name()
                    break
            ptext = ptext + 'const atermpp::aterm_appl& default_value_%s();\n' % name
            ctext = ctext + RULE_FUNCTION % {
                'name'       : name,
                'fname'      : fname
            }
            dtext = dtext + '  const atermpp::aterm_appl core::detail::default_values::%s = core::detail::default_value_%s();\n' % (name, name)
            vtext = vtext + '  static const atermpp::aterm_appl %s;\n' % name

    ctext = ptext + '\n' + ctext
    result = insert_text_in_file(declaration_filename, ctext, 'generated constructors')
    result = result and insert_text_in_file(definition_filename, dtext, 'generated default value definitions')
    result = result and insert_text_in_file(declaration_filename, vtext, 'generated variables')
    return result

#---------------------------------------------------------------#
#                          find_functions
#---------------------------------------------------------------#
# find all functions that appear in the rhs of a rule
def find_functions(rules):
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

#---------------------------------------------------------------#
#                      generate_soundness_check_functions
#---------------------------------------------------------------#
# generates C++ code for checking if terms are in the right format
#
def generate_soundness_check_functions(rules, filename, skip_list):
    CHECK_RULE = '''template <typename Term>
bool check_rule_%(name)s(const Term& t)
{
  utilities::mcrl2_unused(t);
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
%(body)s
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

'''

    CHECK_TERM = '''// %(name)s(%(arguments)s)
template <typename Term>
bool %(check_name)s(const Term& t)
{
  utilities::mcrl2_unused(t);
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
%(body)s
#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

'''
    CHECK_TERM_TYPE = '''  // check the type of the term
  const atermpp::aterm& term(t);
  if (!term.type_is_appl())
  {
    return false;
  }
  const atermpp::aterm_appl& a = atermpp::down_cast<atermpp::aterm_appl>(term);
  if (a.function() != core::detail::function_symbols::%(name)s)
  {
    return false;
  }

'''

    CHECK_TERM_CHILDREN = '''  // check the children
  if (a.size() != %(arity)d)
  {
    return false;
  }
'''

    text  = '' # function definitions
    ptext = '' # function declarations (prototypes)

    functions = find_functions(rules)

    for rule in rules:
        name = rule.name()
        if name in skip_list:
            continue
        rhs_functions = rule.functions()
        body = '  return ' + '\n         || '.join([x.check_name() + '(t)' for x in rhs_functions]) + ';'
        text = text + CHECK_RULE % {
            'name'      : name,
            'body'      : body
        }
        ptext = ptext + 'template <typename Term> bool check_rule_%s(const Term& t);\n' % rule.name()

    for f in functions:
        name = f.name()
        if name in skip_list:
            continue
        arguments = ', '.join([x.full_name() for x in f.arguments])
        arity = len(f.arguments)

        body = CHECK_TERM_TYPE % {
            'name' : name
        }
        body = body + CHECK_TERM_CHILDREN % {
            'arity' : len(f.arguments)
        }
        if arity > 0:
            body = body + '#ifndef MCRL2_NO_RECURSIVE_SOUNDNESS_CHECKS\n'
            for i in range(arity):
                arg = f.arguments[i]
                if arg.repetitions == '':
                    body = body + '  if (!check_term_argument(a[%d], %s<atermpp::aterm>))\n'    % (i, arg.check_name())
                elif arg.repetitions == '*':
                    body = body + '  if (!check_list_argument(a[%d], %s<atermpp::aterm>, 0))\n' % (i, arg.check_name())
                elif arg.repetitions == '+':
                    body = body + '  if (!check_list_argument(a[%d], %s<atermpp::aterm>, 1))\n' % (i, arg.check_name())
                body = body + '  {\n'
                body = body + '    mCRL2log(log::debug) << "%s" << std::endl;\n'                % (arg.check_name())
                body = body + '    return false;\n'
                body = body + '  }\n'
            body = body + '#endif // MCRL2_NO_RECURSIVE_SOUNDNESS_CHECKS\n'

        text = text + CHECK_TERM % {
            'name'       : name,
            'arguments'  : arguments,
            'check_name' : f.check_name(),
            'body'       : body
        }
        ptext = ptext + 'template <typename Term> bool %s(const Term& t);\n' % f.check_name()

    text = ptext + '\n' + text.strip()
    text = text + '\n'
    return insert_text_in_file(filename, text, 'generated code')

#---------------------------------------------------------------#
#                          parse_ebnf
#---------------------------------------------------------------#
def parse_ebnf(filename):
    rules = []

    paragraphs = read_paragraphs(filename)
    for paragraph in paragraphs:
        #--- skip special paragraphs
        if re.match('// Date', paragraph):
            continue
        if re.match('//Specification', paragraph):
            continue
        if re.match('//Expressions', paragraph):
            continue

        #--- handle other paragraphs
        lines  = paragraph.split('\n')
        clines = [] # comment lines
        glines = [] # grammar lines
        for line in lines:
            if re.match('\s*//.*', line):
                clines.append(line)
            else:
                glines.append(line)
        comment = '\n'.join(clines)

        parser = EBNFParser(Mcrl2Actions())
        try:
            newrules = parser('\n'.join(glines))
            for rule in newrules:
                rule.comment = comment
            rules = rules + newrules
        except tpg.SyntacticError as e:
            print("------------------------------------------------------")
            print(('grammar: ', '\n'.join(glines)))
            print(e)
        except tpg.LexicalError as e:
            print("------------------------------------------------------")
            print(('grammar: ', '\n'.join(glines)))
            print(e)
    return rules

#---------------------------------------------------------------#
#                          main
#---------------------------------------------------------------#
def main():
    result = True
    filename = MCRL2_ROOT + 'doc/specs/mcrl2.internal.txt'
    rules = parse_ebnf(filename)

    # elements in this list are skipped during generation
    skip_list = ['DataAppl']

    result = generate_soundness_check_functions(rules, MCRL2_ROOT + 'libraries/core/include/mcrl2/core/detail/soundness_checks.h', skip_list) and result

    declaration_filename = MCRL2_ROOT + 'libraries/core/include/mcrl2/core/detail/function_symbols.h'
    definition_filename = MCRL2_ROOT + 'libraries/core/source/core.cpp'
    result = generate_function_symbol_constructors(rules, declaration_filename, definition_filename, skip_list) and result

    declaration_filename = MCRL2_ROOT + 'libraries/core/include/mcrl2/core/detail/default_values.h'
    definition_filename = MCRL2_ROOT + 'libraries/core/source/core.cpp'
    result = generate_default_values(rules, declaration_filename, definition_filename, skip_list) and result

    return result

if __name__ == "__main__":
    result = main()
    sys.exit(not result) # 0 result indicates successful execution
