#!/usr/bin/env python

#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
import sys
from path import *
from mcrl2_classes import *
from mcrl2_utility import *

# Generates classes from class_text and inserts them in the file
# filename. If filename is a directory, then each of the classes is
# inserted in a separate file.
def make_classes(filename, class_text, namespace, add_constructor_overloads = False):
    classes = parse_classes(class_text, namespace = namespace)

    # skip the classes with a namespace qualifier (they are defined elsewhere)
    classes = [c for c in classes if c.qualifier() == '']

    result = True
    if path(filename).isdir():
        for c in classes:
            if 'S' in c.modifiers():
                continue
            fname = path(filename).normcase() / ('%s.h' % c.name())
            text = c.class_inline_definition()
            result = insert_text_in_file(fname, text, 'generated class %s' % c.name(), handle_user_sections = True) and result
    else:
        class_definitions = [c.class_inline_definition() for c in classes]
        ctext = '\n'.join(class_definitions)
        result = insert_text_in_file(filename, ctext, 'generated classes', handle_user_sections = True) and result
    return result

if __name__ == "__main__":
    result = True
    result = make_classes('../../core/include/mcrl2/core',                            CORE_CLASSES                 , namespace = 'core'            ) and result
    result = make_classes('../../bes/include/mcrl2/bes/boolean_expression.h',         BOOLEAN_EXPRESSION_CLASSES   , namespace = 'bes'             ) and result
    result = make_classes('../../data/include/mcrl2/data/assignment.h',               ASSIGNMENT_EXPRESSION_CLASSES, namespace = 'data'            ) and result
    result = make_classes('../../data/include/mcrl2/data/binder_type.h',              BINDER_TYPES                 , namespace = 'data'            ) and result
    result = make_classes('../../data/include/mcrl2/data/container_type.h',           CONTAINER_TYPES              , namespace = 'data'            ) and result
    result = make_classes('../../lps/include/mcrl2/modal_formula/state_formula.h',    STATE_FORMULA_CLASSES        , namespace = 'state_formulas'  ) and result
    result = make_classes('../../lps/include/mcrl2/modal_formula/regular_formula.h',  REGULAR_FORMULA_CLASSES      , namespace = 'regular_formulas') and result
    result = make_classes('../../lps/include/mcrl2/modal_formula/action_formula.h',   ACTION_FORMULA_CLASSES       , namespace = 'action_formulas' ) and result
    result = make_classes('../../pbes/include/mcrl2/pbes/pbes_expression.h',          PBES_EXPRESSION_CLASSES      , namespace = 'pbes_system'     ) and result
    result = make_classes('../../process/include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_CLASSES   , namespace = 'process'         ) and result
    result = make_classes('../../data/include/mcrl2/data',                            DATA_EXPRESSION_CLASSES      , namespace = 'data'            ) and result
    result = make_classes('../../data/include/mcrl2/data',                            SORT_EXPRESSION_CLASSES      , namespace = 'data'            ) and result
    result = make_classes('../../data/include/mcrl2/data',                            STRUCTURED_SORT_ELEMENTS     , namespace = 'data'            ) and result
    result = make_classes('../../data/include/mcrl2/data',                            DATA_CLASSES                 , namespace = 'data'            ) and result
    result = make_classes('../../lps/include/mcrl2/lps',                              LPS_CLASSES                  , namespace = 'lps'             ) and result
    sys.exit(not result) # 0 result indicates successful execution
