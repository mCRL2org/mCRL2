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
def make_classes(filename, class_text, namespace, add_constructor_overloads = False, generate_is_functions = False, expression_class = None, superclass_aterm = None):
    classes = parse_classes(class_text, use_base_class_name = True, namespace = namespace)

    # skip the classes with a namespace qualifier (they are defined elsewhere)
    classes = [c for c in classes if c.qualifier() == '']

    class_definitions = [c.class_inline_definition(add_container_typedefs = (expression_class == None), add_constructor_overloads = add_constructor_overloads) for c in classes]
 
    if path(filename).isdir():
        for i in range(len(class_definitions)):
            fname = path(filename).normcase() / ('%s.h' % classes[i].name())
            text = class_definitions[i]
            insert_text_in_file(fname, text, 'generated class %s' % classes[i].name(), handle_user_sections = True)
    else:
        ctext = '\n'.join(class_definitions)

        # define expression class
        if expression_class != None and superclass_aterm != None:
            text = Class(aterm = superclass_aterm,
                         constructor = '%s()' % expression_class,
                         description = 'class %s' % expression_class,
                         superclass = 'atermpp::aterm_appl',
                         use_base_class_name = False,
                         namespace = namespace).class_inline_definition(True, add_constructor_overloads)
            # TODO: this code should be generated in a cleaner way
            text = re.sub('check_term', 'check_rule', text)
            ctext = text + ctext
        insert_text_in_file(filename, ctext, 'generated classes', handle_user_sections = True)

    if generate_is_functions:
        make_is_functions(filename, class_text, expression_class, aterm_namespace = 'core')

# Generates class declarations from class_text and inserts them in the file
# filename.
#
# If superclass is defined, it will be the base class of the generated
# classes. Otherwise atermpp::aterm_appl will be taken as the base class.
def make_class_declarations(filename, class_text, superclass = None, namespace = 'core', add_constructor_overloads = False, superclass_aterm = None, label = ''):
    classes = parse_classes(class_text, superclass, use_base_class_name = True)

    # skip the classes with a namespace qualifier (they are defined elsewhere)
    classes = [c for c in classes if c.qualifier() == '']

    class_declarations = [c.class_declaration(namespace, True, add_constructor_overloads) for c in classes]
    ctext = '\n'.join(class_declarations)

    if superclass != None and superclass_aterm != None:
        text = Class(superclass_aterm, '%s()' % superclass, 'class %s' % superclass, superclass = None, use_base_class_name = False).class_inline_definition(add_container_typedefs = True, add_constructor_overloads = add_constructor_overloads)
        # TODO: this code should be generated in a cleaner way
        text = re.sub('check_term', 'check_rule', text)
        ctext = text + ctext
    if label != '':
        label = label + ' '
    insert_text_in_file(filename, ctext, 'generated %sclass declarations' % label)

# Generates class member function definitions from class_text and inserts them in the file
# filename.
#
# If superclass is defined, it will be the base class of the generated
# classes. Otherwise atermpp::aterm_appl will be taken as the base class.
def make_class_definitions(filename, class_text, superclass = None, namespace = 'core', add_constructor_overloads = False, label = ''):
    classes = parse_classes(class_text, superclass, use_base_class_name = True)

    # skip the classes with a namespace qualifier (they are defined elsewhere)
    classes = [c for c in classes if c.qualifier() == '']

    class_definitions = [c.class_member_function_definitions(namespace, True, add_constructor_overloads) for c in classes]
    ctext = '\n'.join(class_definitions)
    if label != '':
        label = label + ' '
    insert_text_in_file(filename, ctext, 'generated %sclass definitions' % label)

def make_is_functions(filename, class_text, classname, aterm_namespace = 'core'):
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
        if c.qualifier() != '': # skip classes residing in a different name space
            continue       
        name = c.name()
        if name[-1] == '_':
            name = name[:-1]
        aterm = c.aterm
        rtext = rtext + TERM_TRAITS_TEXT % (name, name, name, classname, aterm_namespace, aterm)
    insert_text_in_file(filename, rtext, 'generated is-functions')

if __name__ == "__main__":
    make_classes('../../bes/include/mcrl2/bes/boolean_expression.h',         BOOLEAN_EXPRESSION_CLASSES   , namespace = 'bes'             , generate_is_functions = True,  add_constructor_overloads = False, expression_class = 'boolean_expression')
    make_classes('../../data/include/mcrl2/data/assignment.h',               ASSIGNMENT_EXPRESSION_CLASSES, namespace = 'data'            , generate_is_functions = True,  add_constructor_overloads = True , expression_class = 'assignment_expression')
    make_classes('../../data/include/mcrl2/data/binder_type.h',              BINDER_TYPES                 , namespace = 'data'            , generate_is_functions = True,  add_constructor_overloads = True , expression_class = 'binder_type')
    make_classes('../../data/include/mcrl2/data/container_type.h',           CONTAINER_TYPES              , namespace = 'data'            , generate_is_functions = True,  add_constructor_overloads = True , expression_class = 'container_type')
    make_classes('../../lps/include/mcrl2/modal_formula/state_formula.h',    STATE_FORMULA_CLASSES        , namespace = 'state_formulas'  , generate_is_functions = True,  add_constructor_overloads = False, expression_class = 'state_formula')
    make_classes('../../lps/include/mcrl2/modal_formula/regular_formula.h',  REGULAR_FORMULA_CLASSES      , namespace = 'regular_formulas', generate_is_functions = True,  add_constructor_overloads = False, expression_class = 'regular_formula'      , superclass_aterm = 'RegFrm')
    make_classes('../../lps/include/mcrl2/modal_formula/action_formula.h',   ACTION_FORMULA_CLASSES       , namespace = 'action_formulas' , generate_is_functions = True,  add_constructor_overloads = False, expression_class = 'action_formula')
    make_classes('../../pbes/include/mcrl2/pbes/pbes_expression.h',          PBES_EXPRESSION_CLASSES      , namespace = 'pbes_system'     , generate_is_functions = False, add_constructor_overloads = False, expression_class = 'pbes_expression')
    make_classes('../../process/include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_CLASSES   , namespace = 'process'         , generate_is_functions = True,  add_constructor_overloads = False, expression_class = 'process_expression'   , superclass_aterm = 'ProcExpr')
    
    # The classes below don't seem to be generated...
    # make_classes('../../data/include/mcrl2/data/data_expression.h', DATA_EXPRESSION_CLASSES, 'data_expression', add_constructor_overloads = True, generate_is_functions = True)
    # make_classes('../../data/include/mcrl2/data/sort_expression.h', SORT_EXPRESSION_CLASSES, 'sort_expression', add_constructor_overloads = True, generate_is_functions = True)
    # make_classes('../../data/include/mcrl2/data/structured_sort.h', STRUCTURED_SORT_ELEMENTS, 'atermpp::aterm_appl', add_constructor_overloads = True)
