#~ Copyright 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
import string
from path import *

#---------------------------------------------------------------#
#                          parse_alternative
#---------------------------------------------------------------#
# returns a 3-tuple (text, comment, annotation), for example
# ('DataExpr => DataExpr', 'implication', 'Right 1')
def parse_alternative(text):
    # remove the leading '|' or ':'
    text = re.sub('^\s*[|:]', '', text)

    # extract the optional comment
    comment = ''
    words = map(string.strip, text.split('//'))
    if len(words) == 2:
        text = words[0]
        comment = words[1]

    # extract the optional annotation
    annotation = ''
    text = re.sub('(\$unary_)|(\$binary_)(op_)?', '$$', text)
    words = map(string.strip, text.split('$$'))
    if len(words) == 2:
        text = words[0]
        annotation = words[1]

    return (text, comment, annotation)

def split_lines(text):
    result = []
    for line in text.splitlines():
        line = line.strip()
        if len(line) > 0:
            result.append(line)
    return result

#---------------------------------------------------------------#
#                          parse_production
#---------------------------------------------------------------#
# returns (lhs, rhs), where lhs is the symbol at the left hand side
# of a production rule, and rhs is a list of 'alternatives'
def parse_production(text):
    # remove trailing ';'
    text = re.sub(';\s*$', '', text)
    lines = split_lines(text)
    if len(lines) > 1:
        lhs = lines[0]
        rhs = map(parse_alternative, lines[1:])
    else:
        words = text.split(':', 1)
        assert(len(words)) == 2
        lhs = words[0]
        rhs = [parse_alternative(words[1])]
    return (lhs, rhs)

#---------------------------------------------------------------#
#                          parse_mcrl2_syntax
#---------------------------------------------------------------#
# returns a list of (title, productions) where title is the title of
# a section, and productions is a list of production rules
def parse_mcrl2_syntax(filename):
    result = []
    text = path(filename).text()
    sections = map(string.strip, text.split(r'//---', re.S | re.M))[1:]
    for section in sections:
        paragraphs = re.split('\n\s*\n', section)
        title = paragraphs[0].strip()

        # skip the identifiers and whitespace sections
        if title in ['Identifiers', 'Whitespace']:
            continue

        productions = paragraphs[1:]
        result.append((title, map(parse_production, productions)))
    return result
