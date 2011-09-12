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
    print '  alternative="%s" comment="%s" annotation="%s"' % (text, comment, annotation)

#---------------------------------------------------------------#
#                          print_production
#---------------------------------------------------------------#
def print_production(lhs, rhs):
    print 'production %s' % lhs
    for (text, comment, annotation) in rhs:
        print_alternative(text, comment, annotation)

#---------------------------------------------------------------#
#                          print_section
#---------------------------------------------------------------#
def print_section(title, productions):
    print 'section = %s' % title
    for (lhs, rhs) in productions:
        print_production(lhs, rhs)

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
