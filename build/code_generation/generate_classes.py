#!/usr/bin/env python3

#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os.path
import re
import sys
from mcrl2_classes import *
from mcrl2_utility import *

MCRL2_ROOT = '../../'

# Generates classes from class_text and inserts them in the file
# filename. If filename is a directory, then each of the classes is
# inserted in a separate file.
def make_classes(all_classes, filename, class_text, namespace, add_constructor_overloads = False):
    classes = parse_classes(class_text, namespace = namespace)

    # skip the classes with a namespace qualifier (they are defined elsewhere)
    classes = [c for c in classes if c.qualifier() == '']

    # N.B. use the classes defined in all_classes
    classes = [all_classes[c.classname(True)] for c in classes]

    result = True
    if os.path.isdir(filename):
        for c in classes:
            fname = os.path.join(os.path.normcase(filename), ('{}.h'.format(c.name())))
            text = c.class_inline_definition(all_classes)
            result = insert_text_in_file(fname, text, 'generated class {}'.format(c.name()), handle_user_sections = True) and result
    else:
        class_definitions = [c.class_inline_definition(all_classes) for c in classes]
        ctext = '\n'.join(class_definitions)
        result = insert_text_in_file(filename, ctext, 'generated classes', handle_user_sections = True) and result
    return result

if __name__ == "__main__":
    class_map = mcrl2_class_map()
    all_classes = parse_class_map(class_map)
    modifiability_map = make_modifiability_map(all_classes)

    result = True
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/core/include/mcrl2/core',                                      CORE_CLASSES                     , namespace = 'core'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/bes/include/mcrl2/bes/boolean_expression.h',                   BOOLEAN_EXPRESSION_CLASSES       , namespace = 'bes'             ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/bes/include/mcrl2/bes',                                        BOOLEAN_CLASSES                  , namespace = 'bes'             ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/data/include/mcrl2/data/assignment.h',                         ASSIGNMENT_EXPRESSION_CLASSES    , namespace = 'data'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/data/include/mcrl2/data/binder_type.h',                        BINDER_TYPES                     , namespace = 'data'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/data/include/mcrl2/data/container_type.h',                     CONTAINER_TYPES                  , namespace = 'data'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/modal_formula/include/mcrl2/modal_formula/state_formula.h',    STATE_FORMULA_CLASSES            , namespace = 'state_formulas'  ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/modal_formula/include/mcrl2/modal_formula/regular_formula.h',  REGULAR_FORMULA_CLASSES          , namespace = 'regular_formulas') and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/modal_formula/include/mcrl2/modal_formula/action_formula.h',   ACTION_FORMULA_CLASSES           , namespace = 'action_formulas' ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/modal_formula/include/mcrl2/modal_formula/state_formula_specification.h', MODAL_FORMULA_CLASSES , namespace = 'state_formulas'  ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/pbes/include/mcrl2/pbes/pbes_expression.h',                    PBES_EXPRESSION_CLASSES          , namespace = 'pbes_system'     ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/pbes/include/mcrl2/pbes',                                      PBES_CLASSES                     , namespace = 'pbes_system'     ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/process/include/mcrl2/process/process_expression.h',           PROCESS_EXPRESSION_CLASSES       , namespace = 'process'         ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/data/include/mcrl2/data',                                      DATA_EXPRESSION_CLASSES          , namespace = 'data'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/data/include/mcrl2/data',                                      ABSTRACTION_EXPRESSION_CLASSES   , namespace = 'data'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/data/include/mcrl2/data',                                      SORT_EXPRESSION_CLASSES          , namespace = 'data'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/data/include/mcrl2/data',                                      STRUCTURED_SORT_ELEMENTS         , namespace = 'data'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/data/include/mcrl2/data',                                      DATA_CLASSES                     , namespace = 'data'            ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/lps/include/mcrl2/lps',                                        LPS_CLASSES                      , namespace = 'lps'             ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/process/include/mcrl2/process',                                PROCESS_CLASSES                  , namespace = 'process'         ) and result
    result = make_classes(all_classes, MCRL2_ROOT + 'libraries/bes/include/mcrl2/bes/bdd_expression.h',                       BDD_EXPRESSION_CLASSES           , namespace = 'bdd'             ) and result
    sys.exit(not result) # 0 result indicates successful execution
