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
def make_classes(filename, class_text, namespace, add_constructor_overloads = False):
    classes = parse_classes(class_text, namespace = namespace)

    # skip the classes with a namespace qualifier (they are defined elsewhere)
    classes = [c for c in classes if c.qualifier() == '']

    if path(filename).isdir():
        for c in classes:
            if 'S' in c.modifiers():
                continue
            fname = path(filename).normcase() / ('%s.h' % c.name())
            text = c.class_inline_definition()
            insert_text_in_file(fname, text, 'generated class %s' % c.name(), handle_user_sections = True)
    else:
        class_definitions = [c.class_inline_definition() for c in classes]
        ctext = '\n'.join(class_definitions)
        insert_text_in_file(filename, ctext, 'generated classes', handle_user_sections = True)

## Generates class declarations from class_text and inserts them in the file
## filename.
##
## If superclass is defined, it will be the base class of the generated
## classes. Otherwise atermpp::aterm_appl will be taken as the base class.
#def make_class_declarations(filename, class_text, superclass = None, namespace = 'core', add_constructor_overloads = False, superclass_aterm = None, label = ''):
#    classes = parse_classes(class_text, use_base_class_name = True)
#
#    # skip the classes with a namespace qualifier (they are defined elsewhere)
#    classes = [c for c in classes if c.qualifier() == '']
#
#    class_declarations = [c.class_declaration(namespace, True, add_constructor_overloads) for c in classes]
#    ctext = '\n'.join(class_declarations)
#
#    if superclass != None and superclass_aterm != None:
#        text = Class(superclass_aterm, '%s()' % superclass, 'class %s' % superclass, superclass = None, use_base_class_name = False).class_inline_definition(add_constructor_overloads = add_constructor_overloads)
#        # TODO: this code should be generated in a cleaner way
#        text = re.sub('check_term', 'check_rule', text)
#        ctext = text + ctext
#    if label != '':
#        label = label + ' '
#    insert_text_in_file(filename, ctext, 'generated %sclass declarations' % label)
#
## Generates class member function definitions from class_text and inserts them in the file
## filename.
##
## If superclass is defined, it will be the base class of the generated
## classes. Otherwise atermpp::aterm_appl will be taken as the base class.
#def make_class_definitions(filename, class_text, superclass = None, namespace = 'core', add_constructor_overloads = False, label = ''):
#    classes = parse_classes(class_text, use_base_class_name = True)
#
#    # skip the classes with a namespace qualifier (they are defined elsewhere)
#    classes = [c for c in classes if c.qualifier() == '']
#
#    class_definitions = [c.class_member_function_definitions(namespace, True, add_constructor_overloads) for c in classes]
#    ctext = '\n'.join(class_definitions)
#    if label != '':
#        label = label + ' '
#    insert_text_in_file(filename, ctext, 'generated %sclass definitions' % label)

if __name__ == "__main__":
    make_classes('../../bes/include/mcrl2/bes/boolean_expression.h',         BOOLEAN_EXPRESSION_CLASSES   , namespace = 'bes'             )
    make_classes('../../data/include/mcrl2/data/assignment.h',               ASSIGNMENT_EXPRESSION_CLASSES, namespace = 'data'            )
    make_classes('../../data/include/mcrl2/data/binder_type.h',              BINDER_TYPES                 , namespace = 'data'            )
    make_classes('../../data/include/mcrl2/data/container_type.h',           CONTAINER_TYPES              , namespace = 'data'            )
    make_classes('../../lps/include/mcrl2/modal_formula/state_formula.h',    STATE_FORMULA_CLASSES        , namespace = 'state_formulas'  )
    make_classes('../../lps/include/mcrl2/modal_formula/regular_formula.h',  REGULAR_FORMULA_CLASSES      , namespace = 'regular_formulas')
    make_classes('../../lps/include/mcrl2/modal_formula/action_formula.h',   ACTION_FORMULA_CLASSES       , namespace = 'action_formulas' )
    make_classes('../../pbes/include/mcrl2/pbes/pbes_expression.h',          PBES_EXPRESSION_CLASSES      , namespace = 'pbes_system'     )
    make_classes('../../process/include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_CLASSES   , namespace = 'process'         )
    make_classes('../../data/include/mcrl2/data',                            DATA_EXPRESSION_CLASSES      , namespace = 'data'            )
    make_classes('../../data/include/mcrl2/data',                            SORT_EXPRESSION_CLASSES      , namespace = 'data'            )
    make_classes('../../data/include/mcrl2/data',                            STRUCTURED_SORT_ELEMENTS     , namespace = 'data'            )
