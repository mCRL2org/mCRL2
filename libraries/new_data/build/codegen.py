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
# Usage: codegen.py [-v] "<input> <output>"
#          <input> is a filename
#          <output> is a filename
#
#===============================================================================

import sys
import re
import os
import string
from optparse import OptionParser
import Parsing
import copy

from data import *

context = None

# Verbose printing of message
def printVerbose(string, object):
  if verbose:
    print("%s: %s" % (string, object.to_string()))

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
class TokenMid(Parsing.Token):
    "%token mid"
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
class TokenStruct(Parsing.Token):
    "%token struct"
class TokenCons(Parsing.Token):
    "%token cons"
class TokenMap(Parsing.Token):
    "%token map"
class TokenVar(Parsing.Token):
    "%token var"
class TokenEqn(Parsing.Token):
    "%token eqn"
class TokenLambda(Parsing.Token):
    "%token lambda"
class TokenForall(Parsing.Token):
    "%token forall"
class TokenExists(Parsing.Token):
    "%token exists"

class TokenID(Parsing.Token):
    "%token id"
    def __init__(self, parser, s):
        Parsing.Token.__init__(self, parser)
        self.data = identifier(s)

    def to_string(self):
        return self.data.to_string()

#===============================================================================
# Nonterminals, with associated productions.  In traditional BNF, the following
# productions would look something like:
# Result ::= Spec
#          | Includes Spec
# IncludesSpec ::= Spec
#          | Includes Spec
# Includes ::= Include
#          | Includes Include
# Include ::= "#include" ID
# Spec ::= SortSpec FunctionSpec VarSpec EqnSpec
# SortSpec ::= "sort" SortDecls        
# FunctionSpec ::= MapSpec
#                | ConsSpec MapSpec
# ConsSpec ::= "cons" OpDecls
# MapSpec ::= "map" OpDecls
# VarSpec ::= "var" VarDecls
# EqnSpec ::= "eqn" EqnDecls
# SortDecls ::= SortDecl
#            | SortDecls SortDecl
# SortDecl ::= SortExpr Label
#            | ID "(" SortParam ")" Label
#            | ID "(" SortParam ")" Label = StructSpec
# StructSpec ::= "struct" StructDecls
# StructDecls ::= StructDecl |
#                 StructDecl StructDecls
# StructDecl ::= ID Label |
#                ID Label ":" Domain
# VarDecls ::= VarDecl
#            | VarDecls VarDecl
# VarDecl ::= ID ":" SortExpr
# OpDecls ::= OpDecl
#           | OpDecls OpDecl
# OpDecl ::= ID Label ":" SortExpr
# EqnDecls ::= EqnDecl
#            | EqnDecls EqnDecl
# EqnDecl ::= DataExpr "=" DataExpr
# DataExpr ::= DataExprPrimary
#            | DataExpr "(" DataExprs ")"
#            | lambda(VarDecl, DataExpr)
#            | forall(VarDecls, DataExpr)
#            | exists(VarDecls, DataExpr)
# DataExprPrimary ::= ID
#                   | "(" DataExpr ")"
# DataExprs ::= DataExpr
#            | DataExprs "," DataExpr
# SortExpr ::= SortExprPrimary
#            | LabelledDomain -> SortExpr
#            | Domain -> SortExpr
#            | ID "(" SortParam ")"
# SortParam ::= ID
# LabelledDomain ::= SortExprPrimary Label
#                  | LabelledDomain # SortExprPrimary Label
# Domain ::= SortExprPrimary Label
#          | Domain # SortExprPrimary Label
# SortExprPrimary ::= ID
#                  | LPAR SortExpr RPAR
# Label ::= LANG ID RANG

class Result(Parsing.Nonterm):
    "%start"
    def reduceIncludesSpec(self, result):
        "%reduce IncludesSpec"
        global context
        self.data = result.data
        self.data.set_namespace()

        if context == None:
          context = self.data
        else:
          self.data.merge_specification(context)
          context = self.data

class IncludesSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceSpec(self, spec):
        "%reduce Spec"
        self.data = spec.data
        printVerbose("IncludesSpec", self.data)

    def reduceIncludesSpec(self, includes, spec):
        "%reduce Includes Spec"
        self.data = spec.data
        self.data.set_includes(includes.data)
        printVerbose("IncludesSpec", self.data)

class Includes(Parsing.Nonterm):
    "%nonterm"
    def reduceInclude(self, incl):
        "%reduce Include"
        self.data = include_list([incl.data])
        printVerbose("Includes", self.data)

    def reduceIncludes(self, includes, include):
        "%reduce Includes Include"
        self.data = includes.data
        self.data.push_back(include.data)
        printVerbose("Includes", self.data)

class Include(Parsing.Nonterm):
    "%nonterm"
    def reduceInclude(self, incl, id):
        "%reduce include id"
        self.data = include(id.data)
        printVerbose("Include", self.data)

class Spec(Parsing.Nonterm):
    "%nonterm"
    def reduce(self, sortspec, functionspec, varspec, eqnspec):
        "%reduce SortSpec FunctionSpec VarSpec EqnSpec"
        self.data = specification(sortspec.data, functionspec.data, varspec.data, eqnspec.data)
        printVerbose("Spec", self.data)

class SortSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceSortSpec(self, sort, sortdecls):
        "%reduce sort SortDecls"
        self.data = sort_specification(sortdecls.data)
        printVerbose("SortSpec", self.data)

class FunctionSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceMapSpec(self, mapspec):
        "%reduce MapSpec"
        self.data = function_specification(mapspec.data, constructor_specification(function_declaration_list([])))
        printVerbose("FunctionSpec", self.data)

    def reduceConsMapSpec(self, consspec, mapspec):
        "%reduce ConsSpec MapSpec"
        self.data = function_specification(mapspec.data, consspec.data)
        printVerbose("FunctionSpec", self.data)

class ConsSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceConsSpec(self, cons, opdecls):
        "%reduce cons OpDecls"
        self.data = constructor_specification(opdecls.data)
        printVerbose("ConsSpec", self.data)

class MapSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceMapSpec(self, map, opdecls):
        "%reduce map OpDecls"
        self.data = mapping_specification(opdecls.data)
        printVerbose("MapSpec", self.data)

class VarSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceVarSpec(self, var, vardecls):
        "%reduce var VarDecls"
        self.data = variable_specification(vardecls.data)
        printVerbose("VarSpec", self.data)

class EqnSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceEqnSpec(self, eqn, eqndecls):
        "%reduce eqn EqnDecls"
        self.data = equation_specification(eqndecls.data)
        printVerbose("EqnSpec", self.data)

class SortDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceSortDecl(self, sortdecl, semicolon):
        "%reduce SortDecl semicolon"
        self.data = sort_declaration_list([sortdecl.data])
        printVerbose("SortDecls", self.data)

    def reduceSortDecls(self, sortdecls, sortdecl, semicolon):
        "%reduce SortDecls SortDecl semicolon"
        self.data = sortdecls.data
        self.data.push_back(sortdecl.data)
        printVerbose("SortDecls", self.data)

class SortDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceSortDecl(self, sortexpr, label):
        "%reduce id Label"
        self.data = sort_declaration(sort_identifier(sortexpr.data), label.data)
        printVerbose("SortDecl", self.data)

    def reduceSortDeclParam(self, container, lbrack, param, rbrack, label):
        "%reduce id lbrack SortParam rbrack Label"
        self.data = sort_declaration(sort_container(container.data, param.data), label.data)
        printVerbose("SortDecl", self.data)

    def reduceSortAliasParam(self, container, lbrack, param, rbrack, label, equals, struct):
        "%reduce id lbrack SortParam rbrack Label equals StructSpec"
        self.data = sort_declaration(sort_container(container.data, param.data), label.data, struct.data)
        printVerbose("SortDecl", self.data)

class StructSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceStructSpec(self, struct, structdecls):
        "%reduce struct StructDecls"
        self.data = structured_sort_specification(structdecls.data)
        printVerbose("StructSpec", self.data)

class StructDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceStructDecl(self, structdecl):
        "%reduce StructDecl"
        self.data = structured_sort_declaration_list([structdecl.data])
        printVerbose("StructDecls", self.data)

    def reduceStructDecls(self, structdecls, mid, structdecl):
        "%reduce StructDecls mid StructDecl"
        self.data = structdecls.data
        self.data.push_back(structdecl.data)
        printVerbose("StructDecls", self.data)

class StructDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceStructNoDomain(self, id, label):
        "%reduce id Label"
        self.data = structured_sort_declaration(id.data, label.data)
        printVerbose("StructDecl", self.data)

    def reduceStructDecl(self, id, label, colon, labelleddomain):
        "%reduce id Label colon LabelledDomain"
        self.data = structured_sort_declaration(id.data, label.data, labelleddomain.data)
        printVerbose("StructDecl", self.data)
   
class VarDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceVarDecl(self, vardecl, semicolon):
        "%reduce VarDecl semicolon"
        self.data = variable_declaration_list([vardecl.data])
        printVerbose("VarDecls", self.data)

    def reduceVarDecls(self, vardecls, vardecl, semicolon):
        "%reduce VarDecls VarDecl semicolon"
        self.data = vardecls.data
        self.data.push_back(vardecl.data)
        printVerbose("VarDecls", self.data)

class VarDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceVarDecl(self, id, colon, sortexpr):
        "%reduce id colon SortExpr"
        self.data = variable_declaration(id.data, sortexpr.data)
        printVerbose("VarDecl", self.data)

class OpDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceOpDecl(self, opdecl, semicolon):
        "%reduce OpDecl semicolon"
        self.data = function_declaration_list([opdecl.data])
        printVerbose("OpDecls", self.data)

    def reduceOpDecls(self, opdecls, opdecl, semicolon):
        "%reduce OpDecls OpDecl semicolon"
        self.data = opdecls.data
        self.data.push_back(opdecl.data)
        printVerbose("OpDecls", self.data)

class OpDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceOpDecl(self, id, label, colon, sortexpr):
        "%reduce id Label colon SortExpr"
        self.data = function_declaration(id.data, sortexpr.data, label.data)
        printVerbose("OpDecl", self.data)

    # Hack for count on lists
    def reduceCount(self, hash, label, colon, sortexpr):
        "%reduce hash Label colon SortExpr"
        id = TokenID(parser, "#")
        self.data = function_declaration(id.data, sortexpr.data, label.data)
        printVerbose("OpDecl", self.data)

class EqnDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceDataEqn(self, eqndecl, semicolon):
        "%reduce EqnDecl semicolon"
        self.data = equation_declaration_list([eqndecl.data])
        printVerbose("EqnDecls", self.data)

    def reduceDataEqns(self, eqndecls, eqndecl, semicolon):
        "%reduce EqnDecls EqnDecl semicolon"
        self.data = eqndecls.data
        self.data.push_back(eqndecl.data)
        printVerbose("EqnDecls", self.data)

class EqnDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceDataEqn(self, lhs, equals, rhs):
        "%reduce DataExpr equals DataExpr"
        self.data = equation_declaration(lhs.data, rhs.data)
        printVerbose("EqnDecl", self.data)

    def reduceDataEqnCondition(self, condition, arrow, lhs, equals, rhs):
        "%reduce DataExpr arrow DataExpr equals DataExpr"
        self.data = equation_declaration(lhs.data, rhs.data, condition.data)
        printVerbose("EqnDecl", self.data)

class DataExpr(Parsing.Nonterm):
    "%nonterm"
    def reduceDataExprPrimary(self, dataexprprimary):
        "%reduce DataExprPrimary"
        self.data = dataexprprimary.data
        printVerbose("DataExpr", self.data)

    def reduceApplication(self, head, lbrack, arguments, rbrack):
        "%reduce DataExpr lbrack DataExprs rbrack"
        self.data = data_application(head.data, arguments.data)
        printVerbose("DataExpr", self.data)

    def reduceLambda(self, lmb, lbrack, vardecl, comma, expr, rbrack):
        "%reduce lambda lbrack VarDecl comma DataExpr rbrack"
        self.data = lambda_abstraction(vardecl.data, expr.data)
        printVerbose("DataExpr", self.data)

    def reduceForall(self, binder, lbrack, vardecl, comma, expr, rbrack):
        "%reduce forall lbrack VarDecl comma DataExpr rbrack"
        self.data = forall(vardecl.data, expr.data)
        printVerbose("DataExpr", self.data)

    def reduceExists(self, exists, lbrack, vardecl, comma, expr, rbrack):
        "%reduce exists lbrack VarDecl comma DataExpr rbrack"
        self.data = exists(vardecl.data, expr.data)
        printVerbose("DataExpr", self.data)

class DataExprPrimary(Parsing.Nonterm):
    "%nonterm"
    def reduceID(self, id):
        "%reduce id"
        self.data = data_variable_or_function_symbol(id.data)
        printVerbose("DataExprPrimary", self.data)

    # Hack for allowing application of list count
    def reduceCount(self, hash):
        "%reduce hash"
        id = TokenID(parser, "#")
        self.data = function_symbol(id.data)
        printVerbose("DataExprPrimary", self.data)

    def reduceBracketedDataExpr(self, lbrack, dataexpr, rbrack):
        "%reduce lbrack DataExpr rbrack"
        self.data = dataexpr.data
        printVerbose("DataExprPrimary", self.data)

class DataExprs(Parsing.Nonterm):
    "%nonterm"
    def reduceDataExpr(self, dataexpr):
        "%reduce DataExpr"
        self.data = data_expression_list([dataexpr.data])
        printVerbose("DataExprs", self.data)

    def reduceDataExprs(self, dataexprs, comma, dataexpr):
        "%reduce DataExprs comma DataExpr"
        self.data = dataexprs.data
        self.data.push_back(dataexpr.data)
        printVerbose("DataExprs", self.data)

class SortExpr(Parsing.Nonterm):
    "%nonterm"
    def reduceSortExprPrimary(self, sortexpr):
        "%reduce SortExprPrimary"
        self.data = sortexpr.data
        printVerbose("SortExpr", self.data)

    def reduceSortExprArrowLabelled(self, domain, arrow, sortexpr):
        "%reduce LabelledDomain arrow SortExpr"
        self.data = sort_arrow(domain.data, sortexpr.data)
        printVerbose("SortExpr", self.data)

    def reduceSortExprArrow(self, domain, arrow, sortexpr):
        "%reduce Domain arrow SortExpr"
        self.data = sort_arrow(domain.data, sortexpr.data)
        printVerbose("SortExpr", self.data)

class SortParam(Parsing.Nonterm):
    "%nonterm"
    def reduceId(self, id):
        "%reduce id"
        self.data = sort_identifier(id.data)
        printVerbose("SortParam", self.data)

class Domain(Parsing.Nonterm):
    "%nonterm"
    def reduceSortExprPrimary(self, expr):
        "%reduce SortExprPrimary"
        self.data = domain(False, [expr.data])
        printVerbose("Domain", self.data)

    def reduceHashedDomain(self, Domain, hash, SortExprPrimary):
        "%reduce Domain hash SortExprPrimary"
        self.data = Domain.data
        self.data.push_back(SortExprPrimary.data)
        printVerbose("Domain", self.data)

class LabelledDomain(Parsing.Nonterm):
    "%nonterm"
    def reduceSortExprPrimary(self, expr, label):
        "%reduce SortExprPrimary Label"
        self.data = domain(True, [(expr.data, label.data)])
        printVerbose("LabelledDomain", self.data)

    def reduceHashedLabelledDomain(self, Domain, hash, SortExprPrimary, label):
        "%reduce LabelledDomain hash SortExprPrimary Label"
        self.data = Domain.data
        self.data.push_back((SortExprPrimary.data, label.data))
        printVerbose("LabelledDomain", self.data)

class SortExprPrimary(Parsing.Nonterm):
    "%nonterm"
    def reduceId(self, expr):
        "%reduce id"
        self.data = sort_identifier(expr.data)
        printVerbose("SortExprPrimary", self.data)

    def reduceParen(self, lbrack, SortExpr, rbrack):
        "%reduce lbrack SortExpr rbrack"
        self.data = SortExpr.data
        printVerbose("SortExprPrimary", self.data)

    def reduceSortExprParam(self, container, lbrack, param, rbrack):
        "%reduce id lbrack SortParam rbrack"
        self.data = sort_container(container.data, param.data)
        printVerbose("SortExprPrimary", self.data)

class Label(Parsing.Nonterm):
    "%nonterm"
    def reduce(self, lang, id, rang):
        "%reduce lang id rang"
        self.data = label(id.data)
        printVerbose("Label", self.data)

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
        syms = {"->"      : TokenArrow,
                "#"       : TokenHash,
                "|"       : TokenMid,
                "#include": TokenInclude,
                ":"       : TokenColon,
                ";"       : TokenSemiColon,
                ","       : TokenComma,
                "("       : TokenLBrack,
                ")"       : TokenRBrack,
                "<\""     : TokenLAng,
                "\">"     : TokenRAng,
                "="       : TokenEquals,
                "sort"    : TokenSort,
                "cons"    : TokenCons,
                "map"     : TokenMap,
                "var"     : TokenVar,
                "eqn"     : TokenEqn,
                "struct"  : TokenStruct,
                "lambda"  : TokenLambda,
                "forall"  : TokenForall,
                "exists"  : TokenExists
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
        if verbose:
          print "split input: %s" % (p.split(input))

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

#
# filter_comments
#
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

#
# get_includes
#
def get_includes(input):
    lines = string.split(input, '\n')
    includes = []
    for line in lines:
        if re.match('#include.*', line):
            includes.append(line.replace('#include ',''))
        else:
            break
    return includes

#-------------------------------------------------------#
# parse_spec
#-------------------------------------------------------#
# This parses the input file and removes comment lines from it
def parse_spec(infilename):
    global outputcode
    global parser
    global includes_table
    input = filter_comments(infilename)
    includes = get_includes(input)

    parser.reset()
    # Now first process the includes:
    for include in includes:
        if not includes_table.has_key(include):
          parse_spec(include)

    parser.reset()
    if input not in includes_table:
        parser.scan(input)

# -------------------------------------------------------#
# main
# -------------------------------------------------------#
def main():
    global context
    global verbose
    global debug
    usage = "usage: %prog [options] infile outfile"
    option_parser = OptionParser(usage)
    option_parser.add_option("-v", "--verbose", action="store_true", help="give verbose output")
    option_parser.add_option("-d", "--debug", action="store_true", help="give debug output")
    (options, args) = option_parser.parse_args()

    if options.verbose:
        verbose = True

    if options.debug:
        verbose = True
        debug = True
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
        outputcode = context.code()
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


#
# Global variables to collect needed information during parsing.
#
verbose = False       # Switch on debug output?
debug = False         # Switch on verbose output? 
includes_table = {}   # Maps sorts to include files

if __name__ == "__main__":
    main()

