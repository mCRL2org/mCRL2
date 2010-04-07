#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
from path import *
from mcrl2_classes import *
from mcrl2_utility import *

# Generates classes from class_text and inserts them in the file
# filename. If filename is a directory, then each of the # classes is
# inserted in a separate file.
#
# If superclass is defined, it will be the base class of the generated
# classes. Otherwise atermpp::aterm_appl will be taken as the base class.
def make_classes(filename, class_text, superclass = None, namespace = 'core', add_constructor_overloads = False):
    classes = parse_classes(class_text, superclass, use_base_class_name = True)

    # skip the classes with a namespace qualifier (they are defined elsewhere)
    classes = [c for c in classes if c.qualifier() == '']

    class_definitions = [c.class_definition(namespace, True, add_constructor_overloads) for c in classes]

    if path(filename).isdir():
        for i in range(len(class_definitions)):
            fname = path(filename).normcase() / ('%s.h' % classes[i].name())
            text = class_definitions[i]
            insert_text_in_file(fname, text, 'generated class')
    else:
        ctext = '\n'.join(class_definitions)
        insert_text_in_file(filename, ctext, 'generated classes')

def make_is_functions(filename, class_text, classname, namespace = 'core'):
    TERM_TRAITS_TEXT = r'''
    /// \\brief Test for a %s expression
    /// \\param t A term
    /// \\return True if it is a %s expression
    inline
    bool is_%s(const %s& t)
    {
      return %s::detail::gsIs%s(t);
    }
'''

    rtext = ''
    classes = parse_classes(class_text)
    for c in classes:
        name = c.name()
        aterm = c.aterm
        rtext = rtext + TERM_TRAITS_TEXT % (name, name, name, classname, namespace, aterm)
    insert_text_in_file(filename, rtext, 'generated is-functions')

if __name__ == "__main__":
    make_classes('../../lps/include/mcrl2/modal_formula/state_formula.h', STATE_FORMULA_CLASSES, 'state_formula')
    make_classes('../../lps/include/mcrl2/modal_formula/action_formula.h', ACTION_FORMULA_CLASSES, 'action_formula')
    make_classes('../../process/include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_CLASSES, 'process_expression')
    make_classes('../../pbes/include/mcrl2/pbes/pbes_expression.h', PBES_EXPRESSION_CLASSES, 'pbes_expression')
    make_is_functions('../../process/include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_CLASSES, 'process_expression')
    make_classes('../../data/include/mcrl2/data/', DATA_EXPRESSION_CLASSES, 'data_expression', add_constructor_overloads = True)
    make_is_functions('../../data/include/mcrl2/data/data_expression.h', DATA_EXPRESSION_CLASSES, 'data_expression')
    make_classes('../../data/include/mcrl2/data/assignment.h', ASSIGNMENT_EXPRESSION_CLASSES, 'assignment_expression', add_constructor_overloads = True)
    make_is_functions('../../data/include/mcrl2/data/assignment.h', ASSIGNMENT_EXPRESSION_CLASSES, 'assignment_expression')
    make_classes('../../data/include/mcrl2/data/binder_type.h', BINDER_TYPES, 'binder_type', add_constructor_overloads = True)
    make_classes('../../data/include/mcrl2/data/', SORT_EXPRESSION_CLASSES, 'sort_expression', add_constructor_overloads = True)
    make_is_functions('../../data/include/mcrl2/data/sort_expression.h', SORT_EXPRESSION_CLASSES, 'sort_expression')
    make_classes('../../data/include/mcrl2/data/container_type.h', CONTAINER_TYPES, 'container_type', add_constructor_overloads = True)
    make_classes('../../data/include/mcrl2/data/', STRUCTURED_SORT_ELEMENTS, 'atermpp::aterm_appl', add_constructor_overloads = True)
