#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
import string
import tpg
from types import *

#--------------------------------------------------------#
#                  read_text
#--------------------------------------------------------#
# returns the contents of the file 'filename' as a string

def read_text(filename):
    try:
        f = open(filename, 'r')
    except IOError, e:
        print 'Unable to open file ' + filename + ' ', e
        sys.exit(0)

    text = f.read()
    f.close()
    return text

#--------------------------------------------------------#
#                  read_paragraphs
#--------------------------------------------------------#
# returns the contents of the file 'filename' as a list of paragraphs

def read_paragraphs(file):
    text       = read_text(file)
    paragraphs = re.split('\n\s*\n', text)
    return paragraphs

#---------------------------------------------------------------#
#                          Rule
#---------------------------------------------------------------#
# lhs: String
# rhs: List<Function>
# phase: String (or None)
class Rule:
    def __init__(self, lhs, rhs):
        self.lhs = lhs
        self.rhs = rhs

    def name(self):
        return self.lhs

    # returns all functions in self.rhs
    def functions(self):
        result = []
        for f in self.rhs:
            result.append(f)
        return result

    def terminals(self, non_terminal_list):
        result = []
        for f in self.rhs:
            if f.name() not in non_terminal_list:
                result.append(f)
        return result

    def non_terminals(self, non_terminal_list):
        result = []
        for f in self.rhs:
            if f.name() in non_terminal_list:
                result.append(f)
        return result

#---------------------------------------------------------------#
#                          Function
#---------------------------------------------------------------#
# name: String
# arguments: List<Argument>
class Function:
    def __init__(self, name, arguments, phase):
        self.name_ = name
        self.arguments = arguments
        self.phase = phase

    def full_name(self):
        return self.name_

    def name(self):
        if self.name_[0] == '<':
            return self.name_[1:-1]
        else:
            return self.name_

    def check_name(self):
        if self.is_rule():
            return 'check_rule_' + self.name()
        else:
            return 'check_term_' + self.name()

    def is_rule(self):
        return self.name_[0] == '<'

    def arity(self):
        return len(self.arguments)

    def parameters(self):
        result = []
        for i in range(len(self.arguments)):
            arg = self.arguments[i]
            result.append('%s_%d' % (arg.name(), i))
        return result

    def types(self):
        return map(Argument.type, self.arguments)

    def default_call(self):
        params = self.parameters()
        t = []
        for i in range(len(params)):
            t.append('%s' % (params[i]))
        return string.join(t, ', ')

    def default_declaration(self):
        params = self.parameters()
        types  = self.types()
        t = []
        for i in range(len(params)):
            t.append('const %s& %s' % (types[i], params[i]))
        return string.join(t, ', ')

#---------------------------------------------------------------#
#                          Argument
#---------------------------------------------------------------#
# expressions: List<Function>
# repetitions: String    ('+*?')
class Argument:
    def __init__(self, expressions, repetitions, lbl):
        self.expressions = expressions
        self.repetitions = repetitions
        self.lbl = lbl

    def type(self):
        if self.repetitions == '*' or self.repetitions == '+':
            return 'aterm_list'
        else:
            return 'aterm_appl'

    def name(self):
        return self.expressions[0].name()

    def check_name(self):
        return self.expressions[0].check_name()

    def full_name(self):
        return self.expressions[0].name() + self.repetitions

    def __str__(self):
        args = []
        for e in self.expressions:
            args.append(e.name())
        return self.repetitions + ' ' + string.join(args, ' ')

# expressions: List<Expression>
# repetitions: String ('+*?')
class Term:
    def __init__(self, expression, repetitions):
        self.expression = expression
        self.repetitions = repetitions


class AttributeRule:
    def __init__(self, lhs, rhs):
        self.lhs = lhs
        self.rhs = rhs


#---------------------------------------------------------------#
#                          Mcrl2Actions
#---------------------------------------------------------------#
class Mcrl2Actions:
    def make_expression(self, terms):
        return terms

    def make_term(self, name, arguments, phase):
        return Function(name, arguments, phase)

    # repetitions can be either '\*', '\?', '\+' or ''
    def make_factor(self, expression, repetitions, label):
        return Argument(expression, repetitions, label)

    def make_rule(self, lhs, rhs):
        return Rule(lhs, rhs)

    def make_attributerule(self, lhs, rhs):
        return AttributeRule(lhs, rhs)

#---------------------------------------------------------------#
#                          EBNFParser
#---------------------------------------------------------------#
# EBNF description by Aad Mathijsen
#
# // This is the description of an EBNF grammar in the same EBNF format.
# // The start symbol is <syntax> and C-style comments may be used anywhere.
# digit      ::= [0-9]
# letter     ::= [a-zA-Z]
# reserved   ::= ":" | "=" | "|" | "*" | "+" | "?" | "(" | ")" | "[" | "]" | "-"
# other      ::= "`" | "_" | "~" | "!" | "@" | "#" | "$" | "%" | "^" | "&"
#              | "\" | "{" | "}" | ";" | "," | "." | "/" | "<" | ">"
# identifier ::= letter ( letter | digit )?
# character  ::= digit | letter | special | other
# string     ::= "'" ( character | '"' )+ "'"
#              | '"' ( character | "'" )+ '"'
# range      ::= "~"? "[" ( character | digit "-" digit | letter "-" letter )+ "]"
#
# syntax     ::= ( rule )*
# rule       ::= identifier ":" expression
# expression ::= term ( "|" term )*
# term       ::= ( factor ) *
# factor     ::= ( identifier | string | range | "(" expression ")" ) ( "*" | "+" | "?" )?

class EBNFParser(tpg.Parser):
    r"""
        separator option: '\$(\{.*?\}|\S*?\s+-?[0-9]*)' ;
        separator space: '\s+' ;
        separator comments: '//.*' ;


        token tkn: '"(?:[^"\\]|\\.)*"' ;
        token literal: '\'.*?\'' ;
        token identifier: '!?[a-zA-Z_]\w*(\.unique)?' ;
        token attributerule: '\{.*?\}' ;

        START/rules ->
                                          $ rules = [] $
             (
               rule/r                     $ rules.append(r) $
             )*
             ;

        rule/r ->                         
                                          
             identifier/i
             ':'
             expression/e
             '\;'
                                          $ r = self.actions.make_rule(i, e) $
             ;

        expression/e ->
                                          $ terms = [] $
            term/t                        $ terms.append(t) $
            (
              '\|'
              term/t                      $ terms.append(t) $
            )*
                                          $ e = self.actions.make_expression(terms) $
            ;

        term/t ->
                                          $ factors = []; p = None ; name = '' $
            factor/f                      $ factors.append(f) $
            (
              factor/f                    $ factors.append(f) $
            )*
                                          $ t = self.actions.make_term(name, factors, p) $
            ;

        factor/f ->
                                          $ r = '' ; e = None; l = None $
            (
              identifier/e 
              | literal/e        
              | tkn/e
              | '\(' expression/e '\)'
              | '\[' identifier/l '\]'
            )
            (
                '\*'/r
              | '\+'/r
              | '\?'/r
            )?
                                          $ f = self.actions.make_factor(e, r, l) $
            ;

    """

    def __init__(self, actions):
        tpg.Parser.__init__(self)
        self.actions = actions