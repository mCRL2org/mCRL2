#~ Copyright 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
import string
from optparse import OptionParser
from mcrl2_utility import *
from parse_mcrl2_syntax import *
from path import *

PRODUCTION_MAPPING = '''
  ActDecl
  ActFrm action_formulas::action_formula
  Action lps::action
  ActionLabelList lps::action_label_list
  ActionList lps::action_label
  ActionRenameRule
  ActionRenameRuleRHS
  ActionRenameRuleSpec
  ActionRenameSpec
  ActSpec
  BagEnumElt data::detail::data_expression_pair
  BagEnumEltList data::detail::data_expression_pair_list
  BesEqnDecl bes::boolean_equation
  BesEqnSpec bes::boolean_equation_system
  BesExpr bes::boolean_expression
  BesInit bes::boolean_variable
  BesSpec bes::boolean_equation_list
  BesVar bes::boolean_variable
  CommExpr
  CommExprList
  CommExprSet
  ConsSpec
  ConstrDecl
  ConstrDeclList
  DataExpr data::data_expression
  DataExprList data::data_expression_list
  DataExprUnit data::data_expression
  DataSpec data::data_specification
  DataValExpr data::data_expression
  Domain data::sort_expression_list
  EqnDecl data::data_equation
  EqnSpec data::data_equation
  FixedPointOperator pbes_system::fixpoint_symbol
  GlobVarSpec
  IdDecl
  IdsDecl
  IdsDeclList
  Init process::process_expression
  MapSpec
  mCRL2Spec
  mCRL2SpecElt
  mCRL2SpecEltList
  MultAct
  MultActId
  MultActIdList
  MultActIdSet
  PbesEqnDecl
  PbesEqnSpec
  PbesExpr pbes_system::pbes_expression
  PbesInit pbes_system::propositional_variable_instantiation
  PbesSpec pbes_system::pbes<>
  ProcDecl
  ProcExpr process::process_expression
  ProcExprThenElse
  ProcSpec
  ProjDecl
  ProjDeclList
  PropVarDecl pbes_system::propositional_variable
  PropVarInst pbes_system::propositional_variable_instantiation
  RegFrm regular_formulas::regular_formula
  RenExpr
  RenExprList
  RenExprSet
  SortDecl
  SortExpr
  SortExprList
  SortSpec
  StateFrm state_formulas::state_formula
  StateVarDecl
  VarSpec
  WhrExpr
  WhrExprList
'''

PRODUCTION_FUNCTION = '''  RETURNTYPE parse_PRODUCTION(const parse_node& node)
  {
BODY
  }

'''

production_return_types = {}
for line in PRODUCTION_MAPPING.splitlines():
    words = line.split()
    if len(words) == 1:
        production_return_types[words[0]] = 'UNKNOWN'
    elif len(words) == 2:
        production_return_types[words[0]] = words[1]

#---------------------------------------------------------------#
#                          print_alternative
#---------------------------------------------------------------#
def print_alternative(text):
    args = []

    words = text.split()
    for i, word in enumerate(words):
        if word.startswith("'"):
            continue
            function = production_return_types[word]
        args.append('parse_%s(node.child(%d))' % (word, i))
    return 'return UNKNOWN(' + ', '.join(args) + ');'

#---------------------------------------------------------------#
#                          print_production
#---------------------------------------------------------------#
def print_production(lhs, rhs):
    if not lhs in production_return_types:
        production_return_types[lhs] = 'UNKNOWN'

    text = PRODUCTION_FUNCTION
    text = re.sub('RETURNTYPE', production_return_types[lhs], text)
    text = re.sub('PRODUCTION', lhs, text)

    alternatives = [print_alternative(t) for (t, comment, annotation) in rhs]
    if len(alternatives) == 1:
        body = '    ' + alternatives[0]
    else:
        condition = 'CONDITION'
        alternatives = ['if (' + condition + ') { ' + a + ' }' for a in alternatives]
        body = '    ' + '\n    else '.join(alternatives)

    text = re.sub('BODY', body, text)
    print text

#---------------------------------------------------------------#
#                          print_section
#---------------------------------------------------------------#
def print_section(title, productions):
    print '  //', title, '\n'
    for (lhs, rhs) in productions:
        print_production(lhs, rhs)

#---------------------------------------------------------------#
#                          post_process_sections
#---------------------------------------------------------------#
# Generates a new production T ::= T1 | ... | Tn for the pattern (T1 | ... | Tn)
# Generates new productions TList ::= T* for patterns T* and T+ (if TList does not yet exist)
def post_process_sections(sections):

    # make a mapping of all production left hand sides
    production_names = {}
    for (title, productions) in sections:
        for (lhs, rhs) in productions:
            production_names[lhs] = True

    for i, (title, productions) in enumerate(sections):
        new_productions = {}
        for j, (lhs, rhs) in enumerate(productions):
            for k, (text, comment, annotation) in enumerate(rhs):

                # Generate a new production T ::= T1 | ... | Tn for the pattern (T1 | ... | Tn)
                regexp = "\(((?!')([^)]|('\)))*[^'])\)"
                m = re.search(regexp, text)
                while m != None:
                    index = 1
                    new_lhs = lhs + 'Alternative' + str(index)
                    while new_lhs in new_productions:
                        index = index + 1
                        new_lhs = lhs + 'Alternative' + str(index)
                    text = re.sub(regexp, new_lhs, text, 1)
                    new_rhs = map(string.strip, re.split(r'\s*\|\s*', m.group(1)))
                    print 'NEW', new_rhs
                    new_rhs = [(r, '', '') for r in new_rhs]
                    new_productions[new_lhs] = (new_lhs, new_rhs)
                    m = re.search(regexp, text)

                # Generates new productions TList ::= T* for patterns T* and T+ (if TList does not yet exist)
                regexp = r'\b((\w+)\s*[*+])'
                m = re.search(regexp, text)
                while m != None:
                    new_lhs = m.group(2) + 'List'
                    text = re.sub(regexp, new_lhs, text, 1)
                    if not new_lhs in production_names:
                        production_names[new_lhs] = True
                        new_rhs = [(m.group(2) + '*', '', '')]
                        new_productions[new_lhs] = (new_lhs, new_rhs)
                    m = re.search(regexp, text)

                rhs[k] = (text, comment, annotation)

            productions[j] = (lhs, rhs)

        sections[i] = (title, productions + new_productions.values())
    return sections

#---------------------------------------------------------------#
#                          my_print
#---------------------------------------------------------------#
def my_print(sections):
    for (title, productions) in sections:
        for (lhs, rhs) in productions:
            for (text, comment, annotation) in rhs:
                print lhs, '->', text

#---------------------------------------------------------------#
#                          main
#---------------------------------------------------------------#
def main():
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    (options, args) = parser.parse_args()

    filename = '../../../doc/specs/mcrl2-syntax.g'
    sections = parse_mcrl2_syntax(filename)
    sections = post_process_sections(sections)
    for (title, productions) in sections:
        print_section(title, productions)

if __name__ == "__main__":
    main()
