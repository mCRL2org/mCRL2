#~ Copyright 2007 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import re
from mcrl2_parser import *

#--------------------------------------------------------#
#                insert_text_in_file
#--------------------------------------------------------#
# search for the lines equal to 'beginline' and 'endline'
# in filename, and replace the lines between them with 'text'
#
def insert_text_in_file(text, filename, beginline = '<!-- begin -->', endline = '<!-- end -->'):
    tmpfile = filename + '.tmp'
    try:
        infile  = open(filename)
        outfile = open(tmpfile, "w")
    except IOError, e:
        print "Unable to open file ", filename, " ", e
        return

    lines = infile.readlines()

    for i in range(len(lines)):
        outfile.write(lines[i])
        if string.rstrip(lines[i]) == beginline:
            break

    first = i+1
    for i in range(first, len(lines)):
        if string.rstrip(lines[i]) == endline:
            outfile.write(text + '\n')
            outfile.write(lines[i])
            break

    if i < len(lines)-1:
        first = i+1

    for i in range(first, len(lines)):
        outfile.write(lines[i])

    infile.close()
    outfile.close()
    os.remove(filename)
    os.rename(tmpfile, filename)

LIBSTRUCT_SYMBOL_FUNCTIONS = '''// %(name)s
inline
AFun initAFun%(name)s(AFun& f)
{
  f = ATmakeAFun("%(name)s", %(arity)d, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFun%(name)s()
{
  static AFun AFun%(name)s = initAFun%(name)s(AFun%(name)s);
  return AFun%(name)s;
}

inline
bool gsIs%(name)s(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFun%(name)s();
}

'''

LIBSTRUCT_MAKE_FUNCTION = '''inline
ATermAppl gsMake%(name)s(%(parameters)s)
{
  return ATmakeAppl%(arity)d(gsAFun%(name)s()%(arguments)s);
}

'''

#---------------------------------------------------------------#
#                      generate_libstruct_functions
#---------------------------------------------------------------#
# generates C++ code for libstruct functions
#
def generate_libstruct_functions(rules, filename, ignored_phases = []):
    begin = '//--- begin generated code'
    end   = '//--- end generated code'
    names = {}
    calls = {}
    decls = {}

    functions = find_functions(rules, ignored_phases)

    for f in functions:
        name = f.name()
        names[name] = f.arity()
        calls[name] = f.default_call()
        decls[name] = f.default_declaration()

    text = ''
    mtext = '' # gsMake functions
    
    name_keys = names.keys()
    name_keys.sort()
    for name in name_keys:
        arity = names[name]
        text = text + LIBSTRUCT_SYMBOL_FUNCTIONS % {
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
        mtext = mtext + LIBSTRUCT_MAKE_FUNCTION % {
            'name'       : name,
            'arity'      : arity,
            'parameters' : decls[name],
            'arguments'  : comma + calls[name]
        } 
    insert_text_in_file(string.strip(text + mtext), filename, begin, end)

CHECK_RULE = '''template <typename Term>
bool check_rule_%(name)s(Term t)
{
%(body)s
}

'''

CHECK_TERM = '''// %(name)s(%(arguments)s)
template <typename Term>
bool %(check_name)s(Term t)
{
%(body)s
  return true;
}

'''
CHECK_TERM_TYPE = '''  // check the type of the term
  aterm term(aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  aterm_appl a(term);
  if (!gsIs%(name)s(a))
    return false;

'''

CHECK_TERM_CHILDREN = '''  // check the children
  if (a.size() != %(arity)d)
    return false;
'''

#---------------------------------------------------------------#
#                      generate_soundness_check_functions
#---------------------------------------------------------------#
# generates C++ code for checking if terms are in the right format
#
def generate_soundness_check_functions(rules, filename, ignored_phases = []):
    begin = '//--- begin generated code'
    end   = '//--- end generated code'
    text  = '' # function definitions
    ptext = '' # function declarations (prototypes)

    functions = find_functions(rules, ignored_phases)

    for rule in rules:
        name = rule.name()
        rhs_functions = rule.functions(ignored_phases)
        body = '  return    ' + '\n         || '.join(map(lambda x: x.check_name() + '(t)', rhs_functions)) + ';'
        text = text + CHECK_RULE % {
            'name'      : name,
            'body'      : body
        }
        ptext = ptext + 'template <typename Term> bool check_rule_%s(Term t);\n' % rule.name()
    
    for f in functions:
        arguments = ', '.join(map(lambda x: x.full_name(), f.arguments))
        name = f.name()
        arity = len(f.arguments)

        body = CHECK_TERM_TYPE % {
            'name' : name
        }
        body = body + CHECK_TERM_CHILDREN % {
            'arity' : len(f.arguments)
        }
        if arity > 0:
            body = body + '#ifndef LPE_NO_RECURSIVE_SOUNDNESS_CHECKS\n'
            for i in range(arity):
                arg = f.arguments[i]
                if arg.repetitions == '':
                    body = body + '  if (!check_term_argument(a(%d), %s<aterm>))\n'    % (i, arg.check_name())
                elif arg.repetitions == '*':
                    body = body + '  if (!check_list_argument(a(%d), %s<aterm>, 0))\n' % (i, arg.check_name())
                elif arg.repetitions == '+':
                    body = body + '  if (!check_list_argument(a(%d), %s<aterm>, 1))\n' % (i, arg.check_name())
                body = body + '    return false;\n'
            body = body + '#endif // LPE_NO_RECURSIVE_SOUNDNESS_CHECKS\n'

        text = text + CHECK_TERM % {
            'name'       : name,
            'arguments'  : arguments,
            'check_name' : f.check_name(),
            'body'       : body
        }
        ptext = ptext + 'template <typename Term> bool %s(Term t);\n' % f.check_name()

    insert_text_in_file(string.strip(ptext + '\n' + text), filename, begin, end)

#---------------------------------------------------------------#
#                          find_functions
#---------------------------------------------------------------#
# find all functions that appear in the rhs of a rule whose phase is not in ignored_phases
def find_functions(rules, ignored_phases):
    function_map = {}
    for rule in rules:
        for f in rule.functions(ignored_phases):
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
    nonterminals = []
    nonterminal_names = {}

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
#                          main
#---------------------------------------------------------------#
if __name__ == '__main__':
    filename = '../../../../specs/mcrl2.internal.txt'
    rules = parse_ebnf(filename)
    
    ignored_phases = ['-tc', '-lin', '-di', '-rft']
    filename = '../include/lpe/soundness_checks.h'
    generate_soundness_check_functions(rules, filename, ignored_phases)
    
    ignored_phases = []
    filename = '../../mcrl2_basic/include/struct/libstruct_core.h'
    generate_libstruct_functions(rules, filename, ignored_phases)
