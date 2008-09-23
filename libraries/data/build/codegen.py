#!/bin/env python
#===============================================================================
# Copyright (c) 2007 Jason Evans <jasone@canonware.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#===============================================================================
#
# Usage: codegen.py [-v] "<input>"
#                          ^^^^^^^
#                          <input> is a filename
#
#===============================================================================

import sys
import re
import os
import string
from optparse import OptionParser
import Parsing

#
# Translator, used to store global substitutions as a dictionary
#
sorts_table = {}
functions_table = {}
variables_table = {}
recognisers = {}
includes_table = {}
current_sort = "" # current sort, to map include file to sort
# Global string to collect the generated code
outputcode = ""

# MACROS

FILE_HEADER = '''#ifndef MCRL2_DATA_%(uppercasename)s_H
#define MCRL2_DATA_%(uppercasename)s_H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
%(includes)s

namespace mcrl2 {

  namespace data {

    namespace sort_%(name)s {

'''

FILE_FOOTER = '''    } // namespace %(name)s
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_%(uppercasename)s_H
'''

SORT_EXPRESSION_CONSTRUCTORS = '''      // Sort expression %(fullname)s
      inline
      basic_sort %(name)s()
      {
        static basic_sort %(name)s("%(fullname)s");
        return %(name)s;
      }

      // Recogniser for sort expression %(fullname)s
      inline
      bool is_%(name)s(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == %(name)s();
        }
        return false;
      }

'''

FUNCTION_CONSTRUCTOR = '''      // Function symbol %(fullname)s
      inline
      function_symbol %(name)s()
      {
        static function_symbol %(name)s("%(fullname)s", %(sort)s);
        return %(name)s;
      }

'''
      
FUNCTION_RECOGNISER = '''      // Recogniser for %(fullname)s
      inline
      bool is_%(name)s_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == %(name)s();
        }
        return false;
      }

'''

FUNCTION_APPLICATION='''      // Application of %(fullname)s
      inline
      application %(name)s(%(formparams)s)
      {
        %(assertions)s
        return application(%(name)s(),%(actparams)s);
      }

'''

FUNCTION_APPLICATION_RECOGNISER='''      // Recogniser for application of %(fullname)s
      inline
      bool is_%(name)s_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_%(name)s_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

'''

PROJECTION_CASE='''
        if (is_%(name)s_application(e))
        {
          return static_cast<const application&>(e).arguments()[%(index)s];
        }
'''

PROJECTION_FUNCTION='''      // Function for projecting out %(name)s
      inline
      data_expression %(name)s(const data_expression& e)
      {
        %(assertions)s;
        %(code)s
        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

'''


# -------------------------------------------------------#
# generate_sort_expression_constructors
# -------------------------------------------------------#
def generate_sort_expression_constructors(label, sortexpr):
    code = SORT_EXPRESSION_CONSTRUCTORS % {
      'fullname'  : sortexpr.string,
      'name'      : label.label          
    }
    return code

# -------------------------------------------------------#
# generate_function_constructors
# -------------------------------------------------------#
def generate_function_constructors(id, label, sortexpr):
    code = FUNCTION_CONSTRUCTOR % {
      'fullname'  : id.string,
      'name'      : label.label,
      'sort'      : sortexpr.inlinecode
    }
    code += FUNCTION_RECOGNISER % {
      'fullname'  : id.string,
      'name'      : label.label
    }
    arguments = sortexpr.argumentcode.split(", ")
    index = 0
    formalparameters = ""
    actualparameters = ""
    assertions = ""
    for a in arguments:
        if index <> 0:
          formalparameters += ", "
          actualparameters += ", "
        formalparameters += "const data_expression& arg%s" % index
        actualparameters += "arg%s" % index
        assertions += "assert(is_%s(arg%s.sort()));\n        " % (a,index)
        index += 1

    code += FUNCTION_APPLICATION % {
      'fullname'  : id.string,
      'name'      : label.label,
      'formparams': formalparameters,
      'actparams' : actualparameters,
      'assertions': assertions
    }
    code += FUNCTION_APPLICATION_RECOGNISER % {
      'fullname'  : id.string,
      'name'      : label.label
    }

    return code

#
# store_projection_arguments
def store_projection_arguments(id, label, sortexpr):
    global recognisers
    ids = sortexpr.recogniserstring.split(", ")
    index = 0
    for id in ids:
        if id <> '':
            if id in recognisers:
                recognisers[id].append((label.label, index))
            else:
                recognisers[id] = [(label.label, index)]
            index += 1    

#--------------------------------------------------------------#
# generate_projection_functions
#--------------------------------------------------------------#
def generate_projection_functions():
    global recognisers
    code = ''

    for recogniser in recognisers:
        functions = recognisers[recogniser]
        print "recogniser: %s\n function:%s\n" % (recogniser, functions)
        assertions = 'assert('
        cases = ''
        for function in functions:
            name = function[0]
            index = function[1]
            if assertions <> 'assert(':
                assertions += " || "
            assertions += "is_%s_application(e)" % (name)
            cases += PROJECTION_CASE % {
              'name'  : name, 
              'index' : index
            }

        assertions += ")"
        code += PROJECTION_FUNCTION % {
          'name'        : recogniser,
          'assertions'  : assertions,
          'code'        : cases
        }

    return code

def generate_equation_code(vars, condition, lhs, rhs):
    global variables_table
    var_code = ''
    for var in vars:
        if var_code <> '':
            var_code += ", "
        var_code += "variable(\"%s\", %s())" % (var, sorts_table[variables_table[var]])
    if var_code == '':
        eqn_code = "data_equation(variable_list(), %s, %s, %s)" % (condition.inlinecode, lhs.inlinecode, rhs.inlinecode)
    else:
        eqn_code = "data_equation(make_vector(%s), %s, %s, %s)" % (var_code, condition.inlinecode, lhs.inlinecode, rhs.inlinecode)
    return eqn_code
    
def generate_equations_code(sortlabel, equations):
    code = '''      // Give all system defined equations for %s
      inline
      data_equation_list %s_generate_equations_code()
      {
        data_equation_list result;
''' % (sortlabel, sortlabel)
    for equation in equations:
        code += "        result.push_back(%s);\n" % (equation)
    code += '''
        return result;
      }

'''
    return code

#===============================================================================
# Tokens/precedences. 

# Precedences
class PArrow(Parsing.Precedence):
    "%right pArrow"
class PHash(Parsing.Precedence):
    "%left pHash"

# Tokens
class TokenArrow(Parsing.Token):
    "%token arrow [pArrow]"
class TokenHash(Parsing.Token):
    "%token hash [pHash]"
class TokenInclude(Parsing.Token):
    "%token include"
class TokenColon(Parsing.Token):
    "%token colon"
class TokenSemiColon(Parsing.Token):
    "%token semicolon"
class TokenComma(Parsing.Token):
    "%token comma"
class TokenLBrack(Parsing.Token):
    "%token lbrack"
class TokenRBrack(Parsing.Token):
    "%token rbrack"
class TokenLAng(Parsing.Token):
    "%token lang"
class TokenRAng(Parsing.Token):
    "%token rang"
class TokenEquals(Parsing.Token):
    "%token equals"

class TokenSort(Parsing.Token):
    "%token sort"
class TokenCons(Parsing.Token):
    "%token cons"
class TokenMap(Parsing.Token):
    "%token map"
class TokenVar(Parsing.Token):
    "%token var"
class TokenEqn(Parsing.Token):
    "%token eqn"

class TokenID(Parsing.Token):
    "%token id"
    def __init__(self, parser, s):
        Parsing.Token.__init__(self, parser)
        self.string = s
        print "Parsed identifier %s" % s

#===============================================================================
# Nonterminals, with associated productions.  In traditional BNF, the following
# productions would look something like:
# Result ::= Spec
#          | Include Spec
# Includes ::= "#include" ID
#          | Includes "#include" ID
# Spec ::= SortSpec ConsSpec MapSpec EqnSpec
# SortSpec ::= "sort" SortDecls        
# ConsSpec ::= "cons" OpDecls
# MapSpec ::= "map" OpDecls
# VarSpec ::= "var" VarDecls
# EqnSpec ::= "eqn" EqnDecls
# SortDecls ::= SortExpr Label
#            | SortDecls SortExpr Label
# VarDecls ::= ID ":" SortExpr
#            | VarDecls ID ":" SortExpr
# OpDecls ::= ID Label ":" SortExpr
#              | OpDecls ID Label ":" SortExpr
# EqnDecls ::= DataExpr "=" DataExpr
#            | EqnDecls DataExpr "=" DataExpr
# DataExpr ::= DataExprPrimary
#            | DataExpr "(" DataExprs ")"
#            | DataExpr DataExprPrimary DataExpr       # infix operator
# DataExprPrimary ::= ID
#                   | "(" DataExpr ")"
# DataExprs ::= DataExpr
#            | DataExprs "," DataExpr
# SortExpr ::= SortExprPrimary
#            | Domain -> SortExpr
# Domain ::= SortExprPrimary Label
#          | Domain # SortExprPrimary Label
# SortExprPrimary ::= ID
#                  | LPAR SortExpr RPAR
# Label ::= LANG ID RANG

class Result(Parsing.Nonterm):
    "%start"
    def reduceResult(self, spec):
        "%reduce Spec"
        global outputcode # needed in order to modify outputcode
        global sorts_table
        self.code = spec.code
        file_header = FILE_HEADER % {
          'uppercasename' : sorts_table[spec.sort].upper(),
          'name'          : sorts_table[spec.sort],
          'includes'      : ""
        }
        file_footer = FILE_FOOTER % {
          'uppercasename' : sorts_table[spec.sort].upper(),
          'name'          : sorts_table[spec.sort]
        }
        outputcode = outputcode + file_header + spec.code + file_footer

        # Debugging
        self.string = spec.string
        print "Parsed specification without includes"
        print "sorts_table: %s" % (sorts_table)
        print "functions_table: %s" % (functions_table)
        print "recognisers: %s" % (recognisers)


    def reduceIncludesResult(self, includes, spec):
        "%reduce Includes Spec"
        global outputcode # needed in order to modify outputcode
        global sorts_table
        self.code = spec.code
        file_header = FILE_HEADER % {
          'uppercasename' : sorts_table[spec.sort].upper(),
          'name'          : sorts_table[spec.sort],
          'includes'      : includes.code
        }
        file_footer = FILE_FOOTER % {
          'uppercasename' : sorts_table[spec.sort].upper(),
          'name'          : sorts_table[spec.sort]
        }
        outputcode = outputcode + file_header + spec.code + file_footer

        # Debugging
        self.string = includes.string + "\n\n" + spec.string
        print "Parsed specification with include(s):\n%s" % (self.string)
        print "sorts_table: %s" % (sorts_table)
        print "functions_table: %s" % (functions_table)
        print "recognisers: %s" % (recognisers)

class Includes(Parsing.Nonterm):
    "%nonterm"
    def reduceInclude(self, include, id):
        "%reduce include id"
        global sorts_table
        self.code = "#include \"mcrl2/data/%s.h\"\n" % (sorts_table[includes_table[id.string]])

        # Debugging
        self.string = "#include %s" % (id.string)
        print "Parsed include: %s" % (self.string)

    def reduceIncludes(self, includes, include, id):
        "%reduce Includes include id"
        global sorts_table
        self.code = includes.code + "#include \"mcrl2/data/%s.h\"\n" % (sorts_table[includes_table[id.string]])

        # Debugging
        self.string = "%s\n#include %s" % (includes.string, id.string)
        print "Parsed includes: %s" % (self.string)

class Spec(Parsing.Nonterm):
    "%nonterm"
    def reduce(self, sortspec, consspec, mapspec, varspec, eqnspec):
        "%reduce SortSpec ConsSpec MapSpec VarSpec EqnSpec"
        global sorts_table
        print sortspec.sorts
        self.sort = sortspec.sorts[0]
        self.code = sortspec.code + \
                    consspec.code + \
                    mapspec.code + \
                    generate_projection_functions() + \
                    varspec.code + \
                    generate_equations_code(sorts_table[self.sort], eqnspec.equations)

        # Debugging
        self.string = sortspec.string + '\n' + consspec.string + '\n' + mapspec.string + '\n' + varspec.string + '\n' + eqnspec.string
        print "Parsed specification:\n%s\n" % self.string

class SortSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceSortSpec(self, sort, sortdecls):
        "%reduce sort SortDecls"
        self.code = sortdecls.code
        self.sorts = sortdecls.sorts
        
        # Debugging
        self.string = "sort %s;" % (sortdecls.string)
        print "Parsed sort specification: %s" % (self.string)

class ConsSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceConsSpec(self, cons, opdecls):
        "%reduce cons OpDecls"
        print "parsing ConsSpec"
        self.code = opdecls.code
        
        # Debugging
        self.string = "cons %s" % (opdecls.string)
        print "Parsed constructor specification: %s" % (self.string)

class MapSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceMapSpec(self, map, opdecls):
        "%reduce map OpDecls"
        self.code = opdecls.code

        # Debugging
        self.string = "map %s" % (opdecls.string)
        print "Parsed mapping specification %s" % (self.string)

class VarSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceVarSpec(self, var, vardecls):
        "%reduce var VarDecls"
        self.code = vardecls.code
        print "variable declarations: %s\n" % (variables_table)

        # Debugging        
        self.string = "var %s" % (vardecls.string)
        print "Parsed variable specification %s" % (self.string)

class EqnSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceEqnSpec(self, eqn, eqndecls):
        "%reduce eqn EqnDecls"
        self.equations = eqndecls.equations

        # Debugging
        self.string = "eqn %s" % (eqndecls.string)
        print "Parsed equation specification %s" % (self.string)

class SortDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceSortDecl(self, sortexpr, label, semicolon):
        "%reduce id Label semicolon"
        global current_sort
        global sorts_table
        sorts_table[sortexpr.string] = label.label # Register sort for usage in substitution
        self.code = generate_sort_expression_constructors(label, sortexpr)
        self.sorts = [sortexpr.string]
        current_sort = sortexpr.string

        self.string = "%s %s" % (sortexpr.string, label.label)
        print "Parsed single sort declaration %s" % (self.string)

    def reduceSortDecls(self, sortdecls, sortexpr, label, semicolon):
        "%reduce SortDecls id Label semicolon"
        global current_sort
        global sorts_table
        sorts_table[sortexpr.string] = label.label # Register sort for usage in substitution
        self.code = sortdecls.code + generate_sort_expression_constructors(label, sortexpr)
        self.sorts = sortdecls.sorts
        self.sorts.append(sortexpr.string)

        self.string = "%s\n%s %s" % (sortdecls.string, sortexpr.string, label.label)
        print "Parsed sort declaration %s" % (self.string)

class VarDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceVarDecl(self, id, colon, sortexpr, semicolon):
        "%reduce id colon SortExpr semicolon"
        variables_table[id.string] = sortexpr.string
        self.code = ""

        # Debugging
        self.string = id.string + ":" + sortexpr.string
        print "Parsed single variable declaration: %s" % (self.string)

    def reduceVarDecls(self, vardecls, id, colon, sortexpr, semicolon):
        "%reduce VarDecls id colon SortExpr semicolon"
        self.code = vardecls.code + ""
        variables_table[id.string] = sortexpr.string

        # Debugging
        self.string = vardecls.string + ';\n' + id.string + ":" + sortexpr.string + ";"
        print "Parsed variable declarations: %s" % (self.string)

class OpDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceOpDecl(self, id, label, colon, sortexpr, semicolon):
        "%reduce id Label colon SortExpr semicolon"
        print "Parsing opdecl"
        self.code = generate_function_constructors(id, label, sortexpr)
        functions_table[id.string] = label.label
        store_projection_arguments(id, label, sortexpr)
        
        # Debugging
        self.string = id.string + label.string + ":" + sortexpr.string
        print "Parsed single function declaration: %s" % (self.string)

    def reduceOpDecls(self, opdecls, id, label, colon, sortexpr, semicolon):
        "%reduce OpDecls id Label colon SortExpr semicolon"
        print "Parsing opdecls"
        self.lastcode = generate_function_constructors(id, label, sortexpr)
        functions_table[id.string] = label.label
        store_projection_arguments(id, label, sortexpr)
        self.code = opdecls.code + self.lastcode

        # Debugging
        self.string = opdecls.string + ';\n' + id.string + label.label + ":" + sortexpr.string + ";"
        print "Parsed function declarations: %s" % (self.string)

class EqnDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceDataEqn(self, lhs, equals, rhs, semicolon):
        "%reduce DataExpr equals DataExpr semicolon"
        self.code = ""
        self.variables = lhs.variables
        self.variables= self.variables.union(rhs.variables)
        self.condition = TokenID(parser, "true")
        self.condition.inlinecode = "%s()" % (functions_table[self.condition.string])
        self.inlinecode = generate_equation_code(self.variables, self.condition, lhs, rhs)
        self.equations = [self.inlinecode]

        # Debugging
        self.string = lhs.string + " = " + rhs.string
        print "Parsed single data equation: %s" % (self.string)

    def reduceDataEqnCondition(self, condition, arrow, lhs, equals, rhs, semicolon):
        "%reduce DataExpr arrow DataExpr equals DataExpr semicolon"
        self.code = ""
        self.variables = lhs.variables
        self.variables= self.variables.union(rhs.variables)
        self.inlinecode = generate_equation_code(self.variables, condition, lhs, rhs)
        self.equations = [self.inlinecode]

        # Debugging
        self.string = condition.string + " -> " + lhs.string + " = " + rhs.string
        print "Parsed single data equation: %s" % (self.string)

    def reduceDataEqns(self, dataeqns, lhs, equals, rhs, semicolon):
        "%reduce EqnDecls DataExpr equals DataExpr semicolon"
        self.code = ""
        self.variables = lhs.variables
        self.variables = self.variables.union(rhs.variables)
        self.condition = TokenID(parser, "true")
        self.condition.inlinecode = "%s()" % (functions_table[self.condition.string])
        self.eqncode = generate_equation_code(self.variables, self.condition, lhs, rhs)
        self.inlinecode = dataeqns.inlinecode + "\n" + self.eqncode
        self.equations = dataeqns.equations
        self.equations.append(self.eqncode)

        # Debugging
        self.string = dataeqns.string + '\n' + lhs.string + " = " + rhs.string
        print "Parsed data equations: %s" % (self.string)

    def reduceDataEqnsCondition(self, dataeqns, condition, arrow, lhs, equals, rhs, semicolon):
        "%reduce EqnDecls DataExpr arrow DataExpr equals DataExpr semicolon"
        self.code = ""
        self.variables = lhs.variables
        self.variables = self.variables.union(rhs.variables)
        self.eqncode = generate_equation_code(self.variables, condition, lhs, rhs)
        self.inlinecode = dataeqns.inlinecode + "\n" + self.eqncode
        self.equations = dataeqns.equations
        self.equations.append(self.eqncode)

        # Debugging
        self.string = dataeqns.string + '\n' + condition.string + " -> " + lhs.string + " = " + rhs.string
        print "Parsed data equations: %s" % (self.string)

class DataExpr(Parsing.Nonterm):
    "%nonterm"
    def reduceDataExprPrimary(self, dataexprprimary):
        "%reduce DataExprPrimary"
        self.inlinecode = dataexprprimary.inlinecode
        self.variables = dataexprprimary.variables

        # Debugging
        self.string = dataexprprimary.string
        print "Parsed simple data expression: %s" % (self.string)

#    def reduceDataExprInfix(self, dataexprinfix):
#        "%reduce DataExprInfix"
#        self.string = dataexprinfix.string
#        print "Parsed data expression: %s" % (self.string)

    def reduceApplication(self, head, lbrack, arguments, rbrack):
        "%reduce DataExpr lbrack DataExprs rbrack"
        self.inlinecode = "%s(%s)" % (head.inlinecode.rstrip('()'), arguments.inlinecode)
        self.variables = head.variables
        self.variables = self.variables.union(arguments.variables)

        # Debugging
        self.string = head.string +"(" + arguments.string + ")"
        print "Parsed function application: %s" % (self.string)

class DataExprPrimary(Parsing.Nonterm):
    "%nonterm"
    def reduceID(self, id):
        "%reduce id"
        if id.string in functions_table:
            self.inlinecode = "%s()" % (functions_table[id.string])
            self.variables = set()
        else:
            self.inlinecode = "variable(\"%s\", %s())" % (id.string, sorts_table[variables_table[id.string]])
            self.variables = set(id.string)

        # Debugging
        self.string = id.string
        print "Parsed simple data expression: %s" % (self.string)

    def reduceBracketedDataExpr(self, lbrack, dataexpr, rbrack):
        "%reduce lbrack DataExpr rbrack"
        self.inlinecode = dataexpr.inlinecode
        self.variables = dataexpr.variables

        # Debugging
        self.string = "(" + dataexpr.string + ")"
        print "Parsed bracketed data expression: %s" % (self.string)

class DataExprs(Parsing.Nonterm):
    "%nonterm"
    def reduceDataExpr(self, dataexpr):
        "%reduce DataExpr"
        self.inlinecode = dataexpr.inlinecode
        self.variables = dataexpr.variables

        # Debugging
        self.string = dataexpr.string
        print "Parsed data expression: %s" % (self.string)

    def reduceDataExprs(self, dataexprs, comma, dataexpr):
        "%reduce DataExprs comma DataExpr"
        self.inlinecode = dataexprs.inlinecode + ", " + dataexpr.inlinecode
        self.variables = dataexprs.variables
        self.variables = self.variables.union(dataexpr.variables)

        # Debugging
        self.string = dataexprs.string + ", " + dataexpr.string
        print "Parsed data expressions: %s" % (self.string)

class SortExpr(Parsing.Nonterm):
    "%nonterm"
    def reduceSortExprPrimary(self, sortexpr):
        "%reduce SortExprPrimary"
        print "Parsing SortExprPrimary"
        self.inlinecode = sortexpr.inlinecode
        self.argumentcode = sortexpr.argumentcode
        self.recogniserstring = sortexpr.recogniserstring
        
        # Debugging
        self.string = sortexpr.string
        print "Parsed SortExprPrimary: %s" % (sortexpr.string)
        print "recogniserstring: %s" % (self.recogniserstring)

    def reduceSortExprArrow(self, domain, arrow, sortexpr):
        "%reduce Domain arrow SortExpr"
        print "Parsing sort arrow"
        self.inlinecode = "function_sort(%s, %s)" % (domain.inlinecode, sortexpr.inlinecode)
        self.argumentcode = domain.argumentcode
        self.recogniserstring = domain.recogniserstring

        # Debugging
        self.string = "%s -> %s" % (domain.string, sortexpr.string)
        print "Parsed SortExprArrow: %s" % (self.string)
        print "recogniserstring: %s" % (self.recogniserstring)

class Domain(Parsing.Nonterm):
    "%nonterm"
    def reduceSortExprPrimary(self, expr, label):
        "%reduce SortExprPrimary Label"
        print "Parsing singular domain"
        self.inlinecode = "%s()" % (sorts_table[expr.string])
        self.argumentcode = "%s" % (sorts_table[expr.string])
        self.recogniserstring = "%s" % (label.label)

        # Debugging
        self.string = expr.string + label.string
        print "Parsed SortExprPrimary: %s" % (self.string)

    def reduceHashedDomain(self, Domain, hash, SortExprPrimary, label):
        "%reduce Domain hash SortExprPrimary Label"
        print "Parsing domain"
        self.inlinecode = "%s, %s" % (Domain.inlinecode, SortExprPrimary.inlinecode)
        self.argumentcode = "%s, %s" % (Domain.argumentcode, SortExprPrimary.argumentcode)
        self.recogniserstring = "%s, %s" % (Domain.recogniserstring, label.label)

        # Debugging
        self.string = Domain.string + " # " + SortExprPrimary.string + label.label
        print "Parsed SortExprHashedDomain: %s" % (self.string)

class SortExprPrimary(Parsing.Nonterm):
    "%nonterm"
    def reduceId(self, expr):
        "%reduce id"
        print "Parsing id"
        self.inlinecode = "%s()" % (sorts_table[expr.string])
        self.argumentcode = "%s" % (sorts_table[expr.string])
        self.recogniserstring = ""

        # Debugging
        self.string = expr.string
        print "Parsed sort identifier: %s" % (self.string)

    def reduceParen(self, lbrack, SortExpr, rbrack):
        "%reduce lbrack SortExpr rbrack"
        print "parsing Paren"
        self.inlinecode = SortExpr.inlinecode
        self.argumentcode = SortExpr.argumentscode
        self.recogniserstring = SortExpr.recogniserstring

        # Debugging
        self.string = "(" + SortExpr.string + ")"
        print "Parsed bracketed expression: %s" % (self.string)

class Label(Parsing.Nonterm):
    "%nonterm"
    def reduce(self, lang, id, rang):
        "%reduce lang id rang"
        self.label = id.string

        # Debugging
        self.string = "< %s >" % (self.label)
        print "Parsed label: %s" % self.string

# -------------------------------------------------------#
# parser
# -------------------------------------------------------#
# Parser subclasses the Lr parser driver.  Since the grammar is unambiguous, we
# have no need of the Glr driver's extra functionality, though there is nothing
# preventing us from using it.
#
# If you are curious how much more work the GLR driver has to do, simply change
# the superclass from Parsing.Lr to Parsing.Glr, then, run this program with
# verbosity enabled.
class Parser(Parsing.Lr):
    def __init__(self, spec):
	Parsing.Lr.__init__(self, spec)

    # Brain-dead scanner.  The scanner does not have to be a method of this
    # class, so for more complex parsers it is no problem to separate the
    # scanner into a separate module.
    def scan(self, input):
        syms = {"->": TokenArrow,
                "#":  TokenHash,
                "#include": TokenInclude,
                ":":  TokenColon,
                ";":  TokenSemiColon,
                ",":  TokenComma,
                "(":  TokenLBrack,
                ")":  TokenRBrack,
                "<\"":  TokenLAng,
                "\">":  TokenRAng,
                "=":  TokenEquals,
                "sort": TokenSort,
                "cons": TokenCons,
                "map":  TokenMap,
                "var":  TokenVar,
                "eqn":  TokenEqn
               }

        # First make sure the needed separators are surrounded by spaces
        # Some parts always need to get extra whitespace
        input = re.sub('(->|[():;,])', r" \1 ", input)
        # # needs to get whitespace if it is not followed by "include"
        input = re.sub('(#)(?!include)', r" \1 ", input)
        # < needs to get whitespace if it starts a label
        input = re.sub('(<\")(?=\w)', r"\1 ", input)
        # > needs to get whitespace if it ends a label
        input = re.sub('(?<=\w)(\">)', r" \1", input)

        # Split the input at whitespace, producing the tokens
        p=re.compile(r'\s+')
        print p.split(input)

        for word in p.split(input):
            if word != '':
    	        if word in syms:
		    token = syms[word](self)
	        else:
		    token = TokenID(parser, word)
                    # Feed token to parser.
	        self.token(token)
	# Tell the parser that the end of input has been reached.
	self.eoi()

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

#-------------------------------------------------------#
# parse_spec
#-------------------------------------------------------#
# This parses the input file and removes comment lines from it

def filter_comments(filename):
    paragraphs = read_paragraphs(filename)
    clines = [] # comment lines
    glines = [] # grammar lines

    for paragraph in paragraphs:
        lines = string.split(paragraph, '\n')
        for line in lines:
            if re.match('%.*', line):
                clines.append(line)
            else:
                glines.append(line)
    comment = string.join(clines, '\n')
    spec = string.join(glines, '\n')
    return spec

def get_includes(input):
    lines = string.split(input, '\n')
    includes = []
    for line in lines:
        if re.match('#include.*', line):
            includes.append(line.replace('#include ',''))
        else:
            break
    return includes

def parse_spec(infilename):
    global outputcode
    global recognisers
    global parser
    global includes_table
    input = filter_comments(infilename)
    includes = get_includes(input)

    parser.reset()
    outputcode = ""
    recognisers = {}

    # Now first process the includes:
    for include in includes:
        if not includes_table.has_key(include):
            includeinput = parse_spec(include)
            includes_table[include] = current_sort

    outputcode = ""
    recognisers = {}
    parser.reset()
    if input not in includes_table:
        parser.scan(input)


# -------------------------------------------------------#
# main
# -------------------------------------------------------#
def main():
    global outputcode
    usage = "usage: %prog [options] infile outfile"
    option_parser = OptionParser(usage)
    option_parser.add_option("-v", "--verbose", action="store_true", help="give verbose output")
    (options, args) = option_parser.parse_args()

    if options.verbose:
        parser.verbose = True

    if len(args) > 0:
        infilename = args[0]
        outfilename = args[1]
        try:
            infile = open(infilename)
            outfile = open(outfilename, "w")
        except IOError, e:
            print "Unable to open file ", filename, " ", e
            return

        parse_spec(infilename)
        
        outfile.write(outputcode)

    else:
        option_parser.print_help()



# -------------------------------------------------------#
# global parser stuff, needs to be here
# -------------------------------------------------------#

# Introspect this module to generate a parser.  Enable all the bells and
# whistles.
spec = Parsing.Spec(sys.modules[__name__],
                    pickleFile="codegen.pickle",
                    skinny=False,
                    logFile="codegen.log",
                    graphFile="codegen.dot",
                    verbose=True)

# Create a parser that uses the parser tables encapsulated by spec.  In this
# program, we are only creating one parser instance, but it is possible for
# multiple parsers to use the same Spec simultaneously.
parser = Parser(spec)

if __name__ == "__main__":
    main()

