# Author(s): Wieger Wesselink
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# \file generate_modal_formula_classes.py
# \brief This script generates classes for state formulas and action formulas.

from path import *
import re

# /// \brief The or operator for state formulas
# class or_: public state_formula
# {
#   public:
#     /// \brief Constructor.
#     /// \param term A term
#     or_(atermpp::aterm_appl term)
#       : state_formula(term)
#     {
#       assert(core::detail::check_term_StateOr(m_term));
#     }
# 
#     /// \brief Constructor.
#     /// \param left A process expression
#     /// \param d A data expression
#     or_(const state_formula& left, const state_formula& right)
#       : or_(core::detail::gsMakeStateOr(left, right))
#     {}
# };
CLASS_DEFINITION = r'''/// \\brief DESCRIPTION
class CLASSNAME: public SUPERCLASS
{
  public:
    /// \\brief Constructor.
    /// \\param term A term
    CLASSNAME(atermpp::aterm_appl term)
      : SUPERCLASS(term)
    {
      assert(core::detail::check_term_ATERM(m_term));
    }

    /// \\brief Constructor.
    /// \\param left A process expression
    /// \\param d A data expression
    CONSTRUCTOR
      : SUPERCLASS(core::detail::gsMakeATERM(PARAMETERS))
    {}MEMBER_FUNCTIONS
};'''

MEMBER_FUNCTION = '''    TYPE NAME() const
    {
      return atermpp::ARG(*this);
    }'''

STATE_FORMULA_CLASSES = r'''
StateTrue       | true_()                                                                                                         | The value true for state formulas
StateFalse      | false_()                                                                                                        | The value false for state formulas
StateNot        | not_(const state_formula& operand)                                                                              | The not operator for state formulas
StateAnd        | and_(const state_formula& left, const state_formula& right)                                                     | The and operator for state formulas
StateOr         | or_(const state_formula& left, const state_formula& right)                                                      | The or operator for state formulas
StateImp        | imp(const state_formula& left, const state_formula& right)                                                      | The implication operator for state formulas
StateForall     | forall(const data::variable_list& variables, const state_formula& operand)                                      | The universal quantification operator for state formulas
StateExists     | exists(const data::variable_list& variables, const state_formula& operand)                                      | The existential quantification operator for state formulas
StateMust       | must(const regular_formulas::regular_formula& formula, const state_formula& operand)                            | The must operator for state formulas
StateMay        | may(const regular_formulas::regular_formula& formula, const state_formula& operand)                             | The may operator for state formulas
StateYaled      | yaled()                                                                                                         | The yaled operator for state formulas
StateYaledTimed | yaled_timed(const data::data_expression& time_stamp)                                                            | The timed yaled operator for state formulas
StateDelay      | delay()                                                                                                         | The delay operator for state formulas      
StateDelayTimed | delay_timed(const data::data_expression& time_stamp)                                                            | The timed delay operator for state formulas
StateVar        | variable(const core::identifier_string& name, const data::data_expression_list& arguments)                      | The state formula variable
StateNu         | nu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand) | The nu operator for state formulas
StateMu         | mu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand) | The mu operator for state formulas
'''

ACTION_FORMULA_CLASSES = '''
ActTrue   | true_()                                                                     | The value true for action formulas  
ActFalse  | false_()                                                                    | The value false for action formulas 
ActNot    | not_(const action_formula& operand)                                         | The not operator for action formulas
ActAnd    | and_(const action_formula& left, const action_formula& right)               | The and operator for action formulas
ActOr     | or_(const action_formula& left, const action_formula& right)                | The or operator for action formulas 
ActImp    | imp(const action_formula& left, const action_formula& right)                | The implication operator for action formulas               
ActForall | forall(const data::variable_list& variables, const action_formula& operand) | The universal quantification operator for action formulas  
ActExists | exists(const data::variable_list& variables, const action_formula& operand) | The existential quantification operator for action formulas
ActAt     | at(const action_formula& operand, const data::data_expression& time_stamp)  | The at operator for action formulas
'''                                                                                       

def member_function(arg, n):
    p = arg.rpartition(' ')
    type = p[0].strip()
    type = re.sub('^const\s*', '', type)
    type = re.sub('\s*&$', '', type)
    name = p[2].strip()
    arg = 'arg' + str(n)
    if type.endswith('list'):
        arg = 'list_' + arg
    text = MEMBER_FUNCTION
    text = re.sub('TYPE', type, text)
    text = re.sub('NAME', name, text)
    text = re.sub('ARG', arg, text)
    return text
                                                                                          
def parse_classes(text, superclass):
    result = []
    lines = text.rsplit('\n')
    for line in lines:
        words = line.split('|')
        if len(words) != 3:
            continue
        aterm = words[0].strip()
        description = words[2].strip()
        classname = re.sub('\(.*', '', words[1].strip())
        constructor = words[1].strip()
        w = words[1].strip()
        w = re.sub('.*\(', '', w)
        w = re.sub('\).*', '', w)
        args = w.split(',')
        parameters = []
        member_functions = []
        index = 1
        for arg in args:
            if arg.strip() == '':
                continue
            parameters.append(arg.split(' ')[-1])
            member_functions.append(member_function(arg, index))
            index = index + 1
        parameters = ', '.join(parameters)
        mtext = '\n\n'.join(member_functions)
        if mtext != '':
            mtext = '\n\n' + mtext
        ctext = CLASS_DEFINITION
        ctext = re.sub('DESCRIPTION'     , description, ctext)
        ctext = re.sub('CLASSNAME'       , classname  , ctext)
        ctext = re.sub('ATERM'           , aterm      , ctext)
        ctext = re.sub('CONSTRUCTOR'     , constructor, ctext)
        ctext = re.sub('PARAMETERS'      , parameters , ctext)
        ctext = re.sub('SUPERCLASS'      , superclass , ctext)
        ctext = re.sub('MEMBER_FUNCTIONS', mtext, ctext)
        result.append(ctext)
    return result

def make_expression_classes(filename, class_text, class_name):
    classes = parse_classes(class_text, class_name)
    ctext = '\n\n'.join(classes) + '\n'
    text = path(filename).text()
    text = re.compile(r'//--- start generated text ---//.*//--- end generated text ---//', re.S).sub(
                  '//--- start generated text ---//\n' + ctext + '//--- end generated text ---//',
                  text)
    path(filename).write_text(text)   

make_expression_classes('../include/mcrl2/modal_formula/state_formula.h', STATE_FORMULA_CLASSES, 'state_formula')
make_expression_classes('../include/mcrl2/modal_formula/action_formula.h', ACTION_FORMULA_CLASSES, 'action_formula')
