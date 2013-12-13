#!/usr/bin/env python

#~ Copyright 2007 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
import sys
from optparse import OptionParser
from mcrl2_parser import *
from mcrl2_utility import *
from path import *

#---------------------------------------------------------------#
#            generate_function_symbol_constructors
#---------------------------------------------------------------#
#
def generate_function_symbol_constructors(rules, filename, skip_list):
    CODE = '''// %(name)s
inline
const atermpp::function_symbol& function_symbol_%(name)s()
{
  static atermpp::function_symbol function_symbol_%(name)s = atermpp::function_symbol("%(name)s", %(arity)d);
  return function_symbol_%(name)s;
}

'''

    names = {}
    calls = {}
    decls = {}

    functions = find_functions(rules)

    for f in functions:
        name = f.name()
        names[name] = f.arity()
        calls[name] = f.default_call()
        decls[name] = f.default_declaration()

    text = ''

    name_keys = names.keys()
    name_keys.sort()
    for name in name_keys:
        if name in skip_list:
            continue
        arity = names[name]
        text = text + CODE % {
            'name'  : name,
            'name'  : name,
            'name'  : name,
            'arity' : arity,
            'name'  : name,
            'name'  : name,
            'name'  : name,
            'name'  : name,
            'name'  : name,
            'name'  : name,
            'name'  : name
        }
        comma = ''
        if calls[name] != "":
            comma = ', '
    text = text.strip() + '\n'
    return insert_text_in_file(filename, text, 'generated constructors')

CHECK_RULE = '''template <typename Term>
bool check_rule_%(name)s(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
%(body)s
#else
  return true;
#endif // MCRL2_NO_SOUNDNESS_CHECKS
}

'''

CHECK_TERM = '''// %(name)s(%(arguments)s)
template <typename Term>
bool %(check_name)s(Term t)
{
#ifndef MCRL2_NO_SOUNDNESS_CHECKS
%(body)s
#endif // MCRL2_NO_SOUNDNESS_CHECKS
  return true;
}

'''
CHECK_TERM_TYPE = '''  // check the type of the term
  atermpp::aterm term(t);
  if (!term.type_is_appl())
  {
    return false;
  }
  const atermpp::aterm_appl& a = atermpp::aterm_cast<atermpp::aterm_appl>(term);
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

#---------------------------------------------------------------#
#                      generate_soundness_check_functions
#---------------------------------------------------------------#
# generates C++ code for checking if terms are in the right format
#
def generate_soundness_check_functions(rules, filename, skip_list):
    text  = '' # function definitions
    ptext = '' # function declarations (prototypes)

    functions = find_functions(rules)

    for rule in rules:
        name = rule.name()
        if name in skip_list:
            continue
        rhs_functions = rule.functions()
        body = '  return ' + '\n         || '.join(map(lambda x: x.check_name() + '(t)', rhs_functions)) + ';'
        text = text + CHECK_RULE % {
            'name'      : name,
            'body'      : body
        }
        ptext = ptext + 'template <typename Term> bool check_rule_%s(Term t);\n' % rule.name()

    for f in functions:
        name = f.name()
        if name in skip_list:
            continue
        arguments = ', '.join(map(lambda x: x.full_name(), f.arguments))
        arity = len(f.arguments)

        body = CHECK_TERM_TYPE % {
            'name' : name
        }
        body = body + CHECK_TERM_CHILDREN % {
            'arity' : len(f.arguments)
        }
        if arity > 0:
            body = body + '#ifndef LPS_NO_RECURSIVE_SOUNDNESS_CHECKS\n'
            for i in range(arity):
                arg = f.arguments[i]
                if arg.repetitions == '':
                    body = body + '  if (!check_term_argument(a[%d], %s<atermpp::aterm>))\n'    % (i, arg.check_name())
                elif arg.repetitions == '*':
                    body = body + '  if (!check_list_argument(a[%d], %s<atermpp::aterm>, 0))\n' % (i, arg.check_name())
                elif arg.repetitions == '+':
                    body = body + '  if (!check_list_argument(a[%d], %s<atermpp::aterm>, 1))\n' % (i, arg.check_name())
                body = body + '  {\n'
                body = body + '    mCRL2log(log::debug, "soundness_checks") << "%s" << std::endl;\n'                % (arg.check_name())
                body = body + '    return false;\n'
                body = body + '  }\n'
            body = body + '#endif // LPS_NO_RECURSIVE_SOUNDNESS_CHECKS\n'

        text = text + CHECK_TERM % {
            'name'       : name,
            'arguments'  : arguments,
            'check_name' : f.check_name(),
            'body'       : body
        }
        ptext = ptext + 'template <typename Term> bool %s(Term t);\n' % f.check_name()

    text = string.strip(ptext + '\n' + text)
    text = text + '\n'
    return insert_text_in_file(filename, text, 'generated code')

CONSTRUCTOR_FUNCTIONS = '''// %(name)s
inline
const atermpp::aterm_appl& construct%(name)s()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_%(name)s()%(arguments)s);
  return t;
}

'''

CONSTRUCTOR_RULE = '''// %(name)s
inline
const atermpp::aterm_appl& construct%(name)s()
{
  return construct%(fname)s();
}

'''

#---------------------------------------------------------------#
#                      generate_constructor_functions
#---------------------------------------------------------------#
# generates C++ code for constructor functions
#
def generate_constructor_functions(rules, filename, function_symbol_declaration_filename, function_symbol_definition_filename, skip_list):
    text  = ''
    ptext = '' # function declarations (prototypes)
    ftext = '' # function symbol declarations
    dtext = '' # function symbol definitions

    functions = find_functions(rules)

    for f in functions:
        name  = f.name()
        if name in skip_list:
            continue
        ptext = ptext + 'const atermpp::aterm_appl& construct%s();\n' % f.name()
        arity = f.arity()
        args = []
        for x in f.arguments:
            if x.repetitions == '':
                args.append('construct%s()' % x.name())
            elif x.repetitions == '*':
                args.append('constructList()')
            else:
                args.append('constructList(construct%s())' % x.name())

        if len(args) > 0:
            arguments = ', ' + ', '.join(args)
        else:
            arguments = ''
        text = text + CONSTRUCTOR_FUNCTIONS % {
            'name'       : name,
            'name'       : name,
            'arity'      : arity,
            'name'       : name,
            'arguments'  : arguments,
            'name'       : name,
            'name'       : name
        }

        ftext = ftext + '  static atermpp::function_symbol %s;\n' % name
        dtext = dtext + '  atermpp::function_symbol core::detail::function_symbols::%s = core::detail::function_symbol_%s();\n' % (name, name)

    function_names = map(lambda x: x.name(), functions)
    for rule in rules:
        if not rule.name() in function_names:
            name = rule.name()
            if name in skip_list:
                continue
            for f in rule.rhs:
                if f.phase == None or not f.phase.startswith('-') or not f.phase.startswith('.'):
                    fname = f.name()
                    break
            ptext = ptext + 'const atermpp::aterm_appl& construct%s();\n' % name
            text = text + CONSTRUCTOR_RULE % {
                'name'       : name,
                'name'       : name,
                'fname'      : fname
            }

    text = ptext + '\n' + text
    result = insert_text_in_file(filename, text, 'generated code')
    result = result and insert_text_in_file(function_symbol_declaration_filename, ftext, 'generated function symbol declarations')
    result = result and insert_text_in_file(function_symbol_definition_filename, dtext, 'generated function symbol definitions')
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
    functions = map(lambda x: function_map[x], function_map.keys())
    for f in functions:
        for arg in f.arguments:
            for e in arg.expressions:
                if not e.is_rule():
                    function_map[e.name()] = e

    return map(lambda x: function_map[x], function_map.keys())

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
        lines  = string.split(paragraph, '\n')
        clines = [] # comment lines
        glines = [] # grammar lines
        for line in lines:
            if re.match('\s*//.*', line):
                clines.append(line)
            else:
                glines.append(line)
        comment = string.join(clines, '\n')

        parser = EBNFParser(Mcrl2Actions())
        try:
            newrules = parser(string.join(glines, '\n'))
            for rule in newrules:
                rule.comment = comment
            rules = rules + newrules
        except tpg.SyntacticError, e:
            print "------------------------------------------------------"
            print 'grammar: ', string.join(glines, '\n')
            print e
        except tpg.LexicalError, e:
            print "------------------------------------------------------"
            print 'grammar: ', string.join(glines, '\n')
            print e
    return rules

#---------------------------------------------------------------#
#                          postprocess_libstruct
#---------------------------------------------------------------#
def postprocess_libstruct(filename):
    src = '''inline
aterm_appl gsMakeProcess\(aterm_appl ProcVarId_0, aterm_list DataExpr_1\)
\{
  return aterm_appl\(gsAFunProcess\(\), \(aterm\) ProcVarId_0, \(aterm\) DataExpr_1\);
\}
'''
    dest = '''inline
aterm_appl gsMakeProcess(aterm_appl ProcVarId_0, aterm_list DataExpr_1)
{
  // Check whether lengths of process type and its arguments match.
  // Could be replaced by at test for equal types.

  assert(ATgetLength((aterm_list)ATgetArgument(ProcVarId_0,1))==ATgetLength(DataExpr_1));
  return aterm_appl(gsAFunProcess(), (aterm) ProcVarId_0, (aterm) DataExpr_1);
}
'''
    text = path(filename).text()
    text = re.sub(re.compile(src, re.M), dest, text)
    path(filename).write_text(text)

#---------------------------------------------------------------#
#                          main
#---------------------------------------------------------------#
def main():
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    parser.add_option("-s", "--soundness-checks", action="store_true", help="generate soundness check functions from internal mcrl2 format")
    parser.add_option("-l", "--libstruct", action="store_true", help="generate libstruct functions from internal mcrl2 format")
    parser.add_option("-c", "--constructors", action="store_true", help="generate constructor functions from internal mcrl2 format")
    (options, args) = parser.parse_args()

    result = True
    filename = '../../../doc/specs/mcrl2.internal.txt'
    rules = parse_ebnf(filename)

    if not options.soundness_checks and not options.libstruct and not options.constructors:
        options.soundness_checks = True
        options.libstruct = True
        options.constructors = True

    # elements in this list are skipped during generation
    skip_list = ['DataAppl']

    if options.soundness_checks:
        filename = '../include/mcrl2/core/detail/soundness_checks.h'
        result = generate_soundness_check_functions(rules, filename, skip_list) and result

    if options.libstruct:
        filename = '../include/mcrl2/core/detail/function_symbols.h'
        result = generate_function_symbol_constructors(rules, filename, skip_list) and result

    if options.constructors:
        filename = '../include/mcrl2/core/detail/constructors.h'
        function_symbol_declaration_filename = '../include/mcrl2/core/detail/function_symbols.h'
        function_symbol_definition_filename = '../source/core.cpp'
        result = generate_constructor_functions(rules, filename, function_symbol_declaration_filename, function_symbol_definition_filename, skip_list) and result

    return result

if __name__ == "__main__":
    result = main()
    sys.exit(not result) # 0 result indicates successful execution
