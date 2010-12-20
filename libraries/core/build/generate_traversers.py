#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
from path import *
from mcrl2_classes import *
from mcrl2_utility import *

BUILDER_FUNCTION = r'''EXPRESSION operator()(const QUALIFIED_NODE& x)
{
  static_cast<Derived&>(*this).enter(x);
  VISIT_FUNCTIONS
  static_cast<Derived&>(*this).leave(x);
  return result;
}
'''

def compare_classes(x, y):
    if 'X' in x.modifiers() and 'X' in y.modifiers():
        return cmp(x.index, y.index)
    return cmp('X' in x.modifiers(), 'X' in y.modifiers())

def make_traverser(filename, classnames, all_classes):
    result = []
    classes = [all_classes[name] for name in classnames]

    # preserve the same order as old generation
    classes.sort(compare_classes)

    for c in classes:
        result.append(c.traverse_function(all_classes))
    text = '\n'.join(result)

    #----------------------------------------------------------------------------------------#
    # N.B. THIS IS AN UGLY HACK to deal with the optional time function in some LPS classes
    # TODO: investigate if the time interface can be improved
    text = re.sub(r'static_cast<Derived&>\(\*this\)\(x.time\(\)\)', 'if (x.has_time()) static_cast<Derived&>(*this)(x.time());', text)
    #----------------------------------------------------------------------------------------#

    insert_text_in_file(filename, text, 'generated code')

def make_builder_expression_functions(filename, class_text, dependencies, expression_class, namespace, verbose = False):
    result = []
    classes = parse_classes(class_text)
    for c in classes:
        if c.qualifier() != '': # skip classes residing in a different name space
            continue
        if 'X' in c.modifiers():
            continue
        if verbose:
            print 'generating builder functions for class', c.name()
        f = c.constructor
        parameters = []
        for p in f.parameters():
            ptype = p.type(include_modifiers = False, include_namespace = True)
            if is_dependent_type(dependencies, ptype):
                parameters.append('static_cast<Derived&>(*this)(x.%s())' % p.name())
            else:
                parameters.append('x.%s()' % p.name())
        vtext = '%s result = %s(%s);' % (expression_class, f.qualified_name(), ', '.join(parameters))
        ctext = BUILDER_FUNCTION
        ctext = re.sub('EXPRESSION', expression_class, ctext)
        ctext = re.sub('QUALIFIED_NODE', f.qualified_name(), ctext)
        ctext = re.sub('VISIT_FUNCTIONS', vtext, ctext)
        if f.is_template():
            ctext = 'template <typename ' + ', typename '.join(f.template_parameters()) + '>\n' + ctext
        result.append(ctext)

    ctext = BUILDER_FUNCTION
    ctext = re.sub('QUALIFIED_NODE', expression_class, ctext)
    classes = parse_classes(class_text)
    visit_functions = []
    for c in classes:
        f = c.constructor
        is_function = re.sub('_$', '', f.name())
        visit_functions.append('if (%sis_%s(x)) { result = static_cast<Derived&>(*this)(%s(atermpp::aterm_appl(x))); }' % (f.qualifier(), is_function, f.qualified_name()))
    vtext = '%s result;\n  ' % expression_class
    vtext = vtext + '\n  else '.join(visit_functions)
    ctext = re.sub('EXPRESSION', expression_class, ctext)
    ctext = re.sub('VISIT_FUNCTIONS', vtext, ctext)
    result.append(ctext)
    ctext = '\n'.join(result)

    #----------------------------------------------------------------------------------------#
    # N.B. THIS IS AN UGLY HACK to deal with the optional time function in some LPS classes
    # TODO: investigate if the time interface can be improved
    ctext = re.sub(r'static_cast<Derived&>\(\*this\)\(x.time\(\)\)', 'if (x.has_time()) static_cast<Derived&>(*this)(x.time());', ctext)
    #----------------------------------------------------------------------------------------#

    insert_text_in_file(filename, ctext, 'generated %s_builder code' % expression_class)

def make_builder_class_functions(filename, class_text, dependencies, expression_class, namespace, verbose = False):
    result = []
    classes = parse_classes(class_text)
    for c in classes:
        if c.qualifier() != '': # skip classes residing in a different name space
            continue
        if verbose:
            print 'generating builder functions for class', c.name()
        f = c.constructor
        parameters = []
        for p in f.parameters():
            ptype = p.type(include_modifiers = False, include_namespace = True)
            if is_dependent_type(dependencies, ptype):
                parameters.append('static_cast<Derived&>(*this)(x.%s())' % p.name())
            else:
                parameters.append('x.%s()' % p.name())
        vtext = '%s result = %s(%s);' % (expression_class, f.qualified_name(), ', '.join(parameters))
        ctext = BUILDER_FUNCTION
        ctext = re.sub('EXPRESSION', expression_class, ctext)
        ctext = re.sub('QUALIFIED_NODE', f.qualified_name(), ctext)
        ctext = re.sub('VISIT_FUNCTIONS', vtext, ctext)
        if f.is_template():
            ctext = 'template <typename ' + ', typename '.join(f.template_parameters()) + '>\n' + ctext
        result.append(ctext)

    ctext = BUILDER_FUNCTION
    ctext = re.sub('QUALIFIED_NODE', expression_class, ctext)
    classes = parse_classes(class_text)
    visit_functions = []
    for c in classes:
        f = c.constructor
        is_function = re.sub('_$', '', f.name())
        visit_functions.append('if (%sis_%s(x)) { result = static_cast<Derived&>(*this)(%s(atermpp::aterm_appl(x))); }' % (f.qualifier(), is_function, f.qualified_name()))
    vtext = '%s result;\n  ' % expression_class
    vtext = vtext + '\n  else '.join(visit_functions)
    ctext = re.sub('EXPRESSION', expression_class, ctext)
    ctext = re.sub('VISIT_FUNCTIONS', vtext, ctext)
    result.append(ctext)
    ctext = '\n'.join(result)

    #----------------------------------------------------------------------------------------#
    # N.B. THIS IS AN UGLY HACK to deal with the optional time function in some LPS classes
    # TODO: investigate if the time interface can be improved
    ctext = re.sub(r'static_cast<Derived&>\(\*this\)\(x.time\(\)\)', 'if (x.has_time()) static_cast<Derived&>(*this)(x.time());', ctext)
    #----------------------------------------------------------------------------------------#

    insert_text_in_file(filename, ctext, 'generated %s_builder code' % expression_class)

# c: Class
def make_builder_function(c, dependencies, modifiability_map):
    text = r'''RETURN_TYPE operator()(const CLASS_NAME& x)
{
  static_cast<Derived&>(*this).enter(x);
  VISIT_TEXT
  static_cast<Derived&>(*this).leave(x);
  RETURN_STATEMENT
}
'''
    class_name = c.namespace() + '::' + c.classname()
    visit_text = ''
    dependent = False
    if modifiability_map[class_name]:
        return_type = 'void'
        return_statement = ''

        updates = []
        f = c.constructor
        for p in f.parameters():
            ptype = p.type(include_modifiers = False, include_namespace = True)
            if is_dependent_type(dependencies, ptype):
                dependent = True
                if modifiability_map[ptype]:
                    updates.append('static_cast<Derived&>(*this)(x.%s())' % p.name())
                else:
                    updates.append('x.%s() = static_cast<Derived&>(*this)(x.%s())' % (p.name(), p.name()))
            else:
                continue
        if dependent:
            visit_text = '\n'.join(updates)
        else:
            visit_text = '// skip'
    else:
        if 'E' in c.modifiers():
            return_type = '%s::%s' % (c.superclass_namespace(), c.superclass())
        else:
            return_type = class_name
        updates = []
        f = c.constructor
        for p in f.parameters():
            ptype = p.type(include_modifiers = False, include_namespace = True)
            if is_dependent_type(dependencies, ptype):
                updates.append('x.%s()' % p.name())
            else:
                dependent = True
                updates.append('static_cast<Derived&>(*this)(x.%s())' % p.name())
        if dependent:
            visit_text = '%s result = %s(%s);' % (return_type, f.qualified_name(), ', '.join(updates))
            return_statement = 'return result;'
        else:
            visit_text = '// skip'
            return_statement = 'return x;'

    text = re.sub('RETURN_TYPE', return_type, text)
    text = re.sub('CLASS_NAME', class_name, text)
    text = re.sub('VISIT_TEXT', visit_text, text)
    text = re.sub('RETURN_STATEMENT', return_statement, text)
    if f.is_template():
        text = 'template <typename ' + ', typename '.join(f.template_parameters()) + '>\n' + text
    return text

def test_builder_functions(class_list, dependencies, modifiability_map):
    for item in class_list:
        class_text, namespace = item
        classes = parse_classes(class_text, namespace = namespace)
        for c in classes:
            print make_builder_function(c, dependencies, modifiability_map)

if __name__ == "__main__":
    class_list = [
          (CORE_CLASSES                               , 'core'            ),
          (DATA_EXPRESSION_CLASSES                    , 'data'            ),
          (ASSIGNMENT_EXPRESSION_CLASSES              , 'data'            ),
          (SORT_EXPRESSION_CLASSES                    , 'data'            ),
          (CONTAINER_TYPES                            , 'data'            ),
          (BINDER_TYPES                               , 'data'            ),
          (ABSTRACTION_EXPRESSION_CLASSES             , 'data'            ),
          (STRUCTURED_SORT_ELEMENTS                   , 'data'            ),
          (DATA_CLASSES                               , 'data'            ),
          (STATE_FORMULA_CLASSES                      , 'state_formulas'  ),
          (REGULAR_FORMULA_CLASSES                    , 'regular_formulas'),
          (ACTION_FORMULA_CLASSES                     , 'action_formulas' ),
          (LPS_CLASSES                                , 'lps'             ),
          (PROCESS_CLASSES                            , 'process'         ),
          (PROCESS_EXPRESSION_CLASSES                 , 'process'         ),
          (PBES_CLASSES                               , 'pbes_system'     ),
          (PBES_EXPRESSION_CLASSES                    , 'pbes_system'     ),
          (BOOLEAN_CLASSES                            , 'bes'             ),
          (BOOLEAN_EXPRESSION_CLASSES                 , 'bes'             ),
        ]

    all_classes = parse_class_list(class_list)

    make_traverser('../../bes/include/mcrl2/bes/detail/traverser.inc.h'                          , parse_classnames(BOOLEAN_EXPRESSION_CLASSES + BOOLEAN_CLASSES, 'bes'), all_classes)
    make_traverser('../../lps/include/mcrl2/lps/detail/traverser.inc.h'                          , parse_classnames(LPS_CLASSES, 'lps'), all_classes)
    make_traverser('../../process/include/mcrl2/process/detail/traverser.inc.h'                  , parse_classnames(PROCESS_EXPRESSION_CLASSES + PROCESS_CLASSES, 'process'), all_classes)
    make_traverser('../../data/include/mcrl2/data/detail/traverser.inc.h'                        , parse_classnames(ASSIGNMENT_EXPRESSION_CLASSES + BINDER_TYPES + STRUCTURED_SORT_ELEMENTS + CONTAINER_TYPES + SORT_EXPRESSION_CLASSES + DATA_EXPRESSION_CLASSES + ABSTRACTION_EXPRESSION_CLASSES + DATA_CLASSES, 'data'), all_classes)
    make_traverser('../../pbes/include/mcrl2/pbes/detail/traverser.inc.h'                        , parse_classnames(PBES_EXPRESSION_CLASSES + PBES_CLASSES, 'pbes_system'), all_classes)
    make_traverser('../../lps/include/mcrl2/modal_formula/detail/action_formula_traverser.inc.h' , parse_classnames(ACTION_FORMULA_CLASSES, 'action_formulas'), all_classes)
    make_traverser('../../lps/include/mcrl2/modal_formula/detail/regular_formula_traverser.inc.h', parse_classnames(REGULAR_FORMULA_CLASSES, 'regular_formulas'), all_classes)
    make_traverser('../../lps/include/mcrl2/modal_formula/detail/state_formula_traverser.inc.h'  , parse_classnames(STATE_FORMULA_CLASSES, 'state_formulas'), all_classes)

    boolean_expression_dependencies = find_dependencies(all_classes, 'bes::boolean_expression')
    data_expression_dependencies    = find_dependencies(all_classes, 'data::data_expression')
    pbes_expression_dependencies    = find_dependencies(all_classes, 'pbes_system::pbes_expression')
    process_expression_dependencies = find_dependencies(all_classes, 'process::process_expression')
    sort_expression_dependencies    = find_dependencies(all_classes, 'data::sort_expression')
    action_formula_dependencies     = find_dependencies(all_classes, 'action_formulas::action_formula')
    regular_formula_dependencies    = find_dependencies(all_classes, 'regular_formulas::regular_formula')
    state_formula_dependencies      = find_dependencies(all_classes, 'state_formulas::state_formula')

    modifiability_map = make_modifiability_map(class_list)

    #print_dependencies(data_expression_dependencies, '--- data_expression_dependencies ---')

    #test_builder_functions(class_list, data_expression_dependencies, modifiability_map)

    #for t in sorted(modifiability_map):
    #    print t, '->', modifiability_map[t]

    #make_builder_expression_functions(  '../../process/include/mcrl2/process/detail/process_expression_builder.inc.h', PROCESS_EXPRESSION_CLASSES, process_expression_dependencies, 'process_expression', namespace = 'process')
    #make_builder_expression_functions(  '../../pbes/include/mcrl2/pbes/detail/data_expression_builder.inc.h', PBES_CLASSES + PBES_EXPRESSION_CLASSES, data_expression_dependencies, 'data_expression', namespace = 'pbes_system')
    #make_builder_expression_functions(  '../../pbes/include/mcrl2/pbes/detail/pbes_expression_builder.inc.h', PBES_EXPRESSION_CLASSES, pbes_expression_dependencies, 'pbes_expression', namespace = 'pbes_system')
    #make_builder_expression_functions(  '../../bes/include/mcrl2/bes/detail/builder.inc.h', BOOLEAN_EXPRESSION_CLASSES, boolean_expression_dependencies, 'boolean_expression', namespace = 'bes')
    #make_builder_expression_functions  ('../../data/include/mcrl2/data/detail/data_expression_builder.inc.h', DATA_EXPRESSION_CLASSES_WITHOUT_ABSTRACTION + ABSTRACTION_EXPRESSIONS, data_expression_dependencies, 'data_expression', namespace = 'data')
    #make_builder_expression_functions  ('../../data/include/mcrl2/data/detail/sort_expression_builder.inc.h', SORT_EXPRESSION_CLASSES, sort_expression_dependencies, 'sort_expression', namespace = 'data')
    #make_builder_expression_functions(  '../../lps/include/mcrl2/modal_formula/detail/action_formula_builder.inc.h', ACTION_FORMULA_CLASSES, action_formula_dependencies, 'action_formula', namespace = 'action_formulas')
    #make_builder_expression_functions(  '../../lps/include/mcrl2/modal_formula/detail/regular_formula_builder.inc.h', REGULAR_FORMULA_CLASSES, regular_formula_dependencies, 'regular_formula', namespace = 'regular_formulas')
    #make_builder_expression_functions(  '../../lps/include/mcrl2/modal_formula/detail/state_formula_builder.inc.h', STATE_FORMULA_CLASSES, state_formula_dependencies, 'state_formula', namespace = 'state_formulas')
