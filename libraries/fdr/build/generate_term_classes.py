#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from generate_term_functions import *
from generate_classes import *
from fdr_classes import *

def make_forward_header(classname):
    TEXT = '''// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/<CLASSNAME>.h
/// \brief Forward declaration of class <CLASSNAME>.

#ifndef MCRL2_FDR_<CLASSNAME_UPPER>_FWD_H
#define MCRL2_FDR_<CLASSNAME_UPPER>_FWD_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"

namespace mcrl2 {

namespace fdr {

class <CLASSNAME>;
typedef atermpp::term_list<<CLASSNAME>> <CLASSNAME>_list;
typedef atermpp::vector<<CLASSNAME>> <CLASSNAME>_vector;

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_<CLASSNAME_UPPER>_FWD_H
'''
    filename = path('../include/mcrl2/fdr/%s_fwd.h' % classname)
    text = TEXT
    text = re.sub('<CLASSNAME>', classname, text)
    text = re.sub('<CLASSNAME_UPPER>', classname.upper(), text)
    path(filename).write_text(text)

def do_expression_classes(name, aterm):
    classname = '%s_expression' % name
    if classname.startswith('_'):
        classname = classname[1:]
    class_specifications = eval('%s_CLASSES' % classname.upper())
    make_class_declarations('../include/mcrl2/fdr/%s.h' % classname, class_specifications, classname, 'fdr', superclass_aterm = aterm, label = '%s expression' % name)
    make_class_definitions('../source/term_functions.cpp', class_specifications, classname, 'fdr', label = '%s expression' % name)
    make_is_functions('../include/mcrl2/fdr/%s.h' % classname, class_specifications, classname, 'fdr')
    make_forward_header(classname)

if __name__ == "__main__":
    rules = parse_ebnf('../doc/CSP-term.txt')
    generate_soundness_check_functions(rules, '../include/mcrl2/fdr/detail/syntax_checks.h')
    generate_libstruct_functions(rules, '../include/mcrl2/fdr/detail/term_functions.h')
    generate_constructor_functions(rules, '../include/mcrl2/fdr/detail/constructors.h')

    # generate expression classes (classes that have a given expression class as super class)
    do_expression_classes('comprehension', 'Comprehension')
    do_expression_classes('numeric', 'Number')
    do_expression_classes('boolean', 'Bool')
    do_expression_classes('set', 'Set')
    do_expression_classes('seq', 'Seq')
    do_expression_classes('tuple', 'Tuple')
    do_expression_classes('dotted', 'Dotted')
    do_expression_classes('lambda', 'Lambda')
    do_expression_classes('common', 'Common')
    do_expression_classes('', 'Expr')
    do_expression_classes('process', 'Proc')
    do_expression_classes('definition', 'Defn')
    do_expression_classes('failuremodel', 'FailureModel')
    do_expression_classes('targ', 'FailureModel')
    do_expression_classes('field', 'Field')
    do_expression_classes('testtype', 'TestType')
    do_expression_classes('vartype', 'VarType')
    do_expression_classes('type', 'Type')
    do_expression_classes('check', 'Check')
    do_expression_classes('model', 'Model')
    do_expression_classes('test', 'Test')
    do_expression_classes('trname', 'TrName')
    do_expression_classes('any', 'Any')
    do_expression_classes('linkpar', 'LinkPar')

    # generate other classes
    #make_classes('../include/mcrl2/fdr/refined.h', REFINED_CLASSES, None, 'fdr')
    #make_classes('../include/mcrl2/fdr/generator.h', GEN_CLASSES, None, 'fdr')

    make_class_declarations('../include/mcrl2/fdr/refined.h', REFINED_CLASSES, None, 'fdr', label = 'refined')
    make_class_definitions('../source/term_functions.cpp', REFINED_CLASSES, None, 'fdr', label = 'refined')
    make_class_declarations('../include/mcrl2/fdr/generator.h', GEN_CLASSES, None, 'fdr', label = 'generator')
    make_class_definitions('../source/term_functions.cpp', GEN_CLASSES, None, 'fdr', label = 'generator')

    make_classes('../include/mcrl2/fdr/filename.h', FILENAME_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/fdrspec.h', FDRSPEC_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/renaming.h', RENAMING_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/map.h', MAP_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/link.h', LINK_CLASSES, None, 'fdr')
