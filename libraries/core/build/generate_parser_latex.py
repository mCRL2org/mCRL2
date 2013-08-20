#!/usr/bin/env python

#~ Copyright 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
import string
from optparse import OptionParser
from parse_mcrl2_syntax import *
from path import *

#---------------------------------------------------------------#
#                          print_alternative
#---------------------------------------------------------------#
def print_alternative(text, comment, annotation):
# Replace text in quotes with \texttt{...}.
    text = re.sub(r"'([^']*)'", r'\\texttt{\1}', text) 
# Replace other strings with {\it ...}.
    text = re.sub(r'(\b\w+)\s', r'{\\it \1} ', text)
    text = re.sub(r'(\b\w+)$', r'{\\it \1}', text)
# &&  --> \&\&.
    text = re.sub(r'&&', r'\\&\\&', text)
# #  --> \#.
    text = re.sub(r'#', r'\\#', text)
# {}  --> \{\}.
    text = re.sub(r'{}', r'\\{\\}', text)
# Replace $unary_left by ; Left in an annotation.
    annotation = re.sub(r'\$unary_left',r'; Left',annotation)
    annotation = re.sub(r'\$unary_right',r'; Right',annotation)
    annotation = re.sub(r'\$left',r'; Left',annotation)
    annotation = re.sub(r'\$right',r'; Right',annotation)
    annotation = re.sub(r'\$unary_op_left',r'; Left',annotation)
    annotation = re.sub(r'\$unary_op_right',r'; Right',annotation)
    annotation = re.sub(r'\$binary_op_left',r'; Left',annotation)
    annotation = re.sub(r'\$binary_op_right',r'; Right',annotation)
    text = re.sub(r'_',r'\\_',text)

    print '%s & %s%s\\\\' % (text, comment, annotation)

#---------------------------------------------------------------#
#                          print_production
#---------------------------------------------------------------#
def print_production(lhs, rhs):
    print '{\\it %s} & ::= & ' % lhs
    firsttime=True
    for (text, comment, annotation) in rhs:
        if not firsttime:
               print '& | & '
        firsttime=False
        print_alternative(text, comment, annotation)

#---------------------------------------------------------------#
#                          print_section
#---------------------------------------------------------------#
def print_section(title, productions):
    print '\\section{%s}' % title
    for (lhs, rhs) in productions:
        print '\\small'
        print '\\begin{syntax}'
        print_production(lhs, rhs)
        print '\\end{syntax}'
#---------------------------------------------------------------#
#                          main
#---------------------------------------------------------------#
def main():
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    (options, args) = parser.parse_args()

    filename = '../../../doc/specs/mcrl2-syntax.g'
    sections = parse_mcrl2_syntax(filename)
    for (title, productions) in sections:
        print_section(title, productions)

if __name__ == "__main__":
    main()
