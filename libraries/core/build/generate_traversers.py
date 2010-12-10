#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
from path import *
from mcrl2_classes import *
from mcrl2_utility import *

TRAVERSE_FUNCTION = r'''void operator()(const QUALIFIED_NODE& x)
{
  static_cast<Derived&>(*this).enter(x);VISIT_FUNCTIONS
  static_cast<Derived&>(*this).leave(x);
}
'''

BUILDER_FUNCTION = r'''EXPRESSION operator()(const QUALIFIED_NODE& x)
{
  static_cast<Derived&>(*this).enter(x);
  VISIT_FUNCTIONS
  static_cast<Derived&>(*this).leave(x);
  return result;
}
'''

def make_traverser_inc_file(filename, class_text, expression_classes = [], namespace = None, verbose = False):
    result = []
    classes = parse_classes(class_text)
    for c in classes:
        if c.qualifier() != '': # skip classes residing in a different name space
            continue
        if verbose:
            print 'generating traverse functions for class', c.name()
        f = c.constructor
        visit_functions = []       
        for p in f.parameters():
            #----------------------------------------------------------------------------------------#
            # N.B. The data traverser skips data_specification, so it needs to be done here too!
            # TODO: investigate why it is skipped
            if p.type().find('data_specification') != -1:
                continue
            visit_functions.append('\n  static_cast<Derived&>(*this)(x.%s());' % p.name())
        vtext = ''.join(visit_functions)
        ctext = TRAVERSE_FUNCTION
        qualified_node = f.qualified_name()
        if namespace != None and re.search('::', qualified_node) == None:
            qualified_node = namespace + '::' + qualified_node
        ctext = re.sub('QUALIFIED_NODE', qualified_node, ctext)
        ctext = re.sub('VISIT_FUNCTIONS', vtext, ctext)
        if f.is_template():
            ctext = 'template <typename ' + ', typename '.join(f.template_parameters()) + '>\n' + ctext
        result.append(ctext)

    for (expression_class, expression_text) in expression_classes:
        ctext = TRAVERSE_FUNCTION
        qualified_node = expression_class
        if namespace != None and re.search('::', qualified_node) == None:
            qualified_node = namespace + '::' + qualified_node
        ctext = re.sub('QUALIFIED_NODE', qualified_node, ctext)
        classes = parse_classes(expression_text)
        visit_functions = []
        for c in classes:
            f = c.constructor
            is_function = re.sub('_$', '', f.name())
            nspace = ''
            if namespace != None and re.search('::', f.qualified_name()) == None:
                nspace = namespace + '::'
            visit_functions.append('if (%sis_%s(x)) { static_cast<Derived&>(*this)(%s%s(atermpp::aterm_appl(x))); }' % (f.qualifier(), is_function, nspace, f.qualified_name()))
        vtext = '\n  ' + '\n  else '.join(visit_functions)
        ctext = re.sub('VISIT_FUNCTIONS', vtext, ctext)
        result.append(ctext)
    ctext = '\n'.join(result)

    #----------------------------------------------------------------------------------------#
    # N.B. THIS IS AN UGLY HACK to deal with the optional time function in some LPS classes
    # TODO: investigate if the time interface can be improved
    ctext = re.sub(r'static_cast<Derived&>\(\*this\)\(x.time\(\)\)', 'if (x.has_time()) static_cast<Derived&>(*this)(x.time());', ctext)   
    #----------------------------------------------------------------------------------------#

    insert_text_in_file(filename, ctext, 'generated code')

def add_namespace(p, namespace):
    if p.find(':') == -1:
        return '%s::%s' % (namespace, p)
    return p

def print_dependencies(dependencies):
    for type in sorted(dependencies):
        print type, dependencies[type]

def update_dependencies(class_map, dependencies, expression_dependencies):
    changed = False
    for classname in class_map:
        if dependencies[classname] == True:
            continue
        namespace = re.sub('\\:.*', '', classname)
        c = class_map[classname]
        
        # check superclass dependency
        if c.superclass() != None:
            superclass = add_namespace(c.superclass(), namespace)
            if dependencies[superclass] == True:
                changed = True
                dependencies[classname] = True
                dependencies[classname + '_list'] = True
                dependencies[classname + '_vector'] = True
                continue
                
        # check parameter dependencies
        for p in c.constructor.parameters():
            type = add_namespace(extract_type(p.type()), namespace)
            if dependencies[type] == True:
                changed = True
                dependencies[classname] = True
                dependencies[classname + '_list'] = True
                dependencies[classname + '_vector'] = True
                break

    # check expression class dependencies
    for classname in expression_dependencies:
        if dependencies[classname] == True:
            continue
        for d in expression_dependencies[classname]:
            if dependencies[d] == True:
                changed = True
                dependencies[classname] = True
                dependencies[classname + '_list'] = True
                dependencies[classname + '_vector'] = True

    return changed

def find_dependencies(class_text_list, expression_class, expression_class_namespace):
    class_map = {}               # maps class names to classes
    dependencies = {}            # maps class names to True/False
    expression_dependencies = {} # maps expression classes to derivatives

    # Create parameter map
    for item in class_text_list:
        class_text, namespace, superclass = item
        if superclass == 'None':
            superclass = None
        classes = parse_classes(class_text, superclass, namespace = expression_class_namespace)
        for c in classes:
            class_name = add_namespace(c.classname(), namespace)
            class_map[class_name] = c
            f = c.constructor
            dependencies[class_name] = False
            for p in f.parameters():
                type = add_namespace(extract_type(p.type()), namespace)
                dependencies[type] = False
            if superclass != None:
                sname = add_namespace(superclass, namespace)
                if not sname in expression_dependencies:
                    expression_dependencies[sname] = []
                expression_dependencies[sname].append(class_name)

    # Initial dependency: the expression class depends on itself
    type = add_namespace(expression_class, expression_class_namespace)
    dependencies[type] = True
    dependencies[type + '_list'] = True
    dependencies[type + '_vector'] = True
    
    while update_dependencies(class_map, dependencies, expression_dependencies):
        pass
    print_dependencies(dependencies)
    
def make_builder_inc_file(filename, class_text, expression_class, verbose = False):
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
            if extract_type(p.type()) not in [expression_class, expression_class + '_list']:
                parameters.append('x.%s()' % p.name())
            else:
                parameters.append('static_cast<Derived&>(*this)(x.%s())' % p.name())
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

    insert_text_in_file(filename, ctext, 'generated code')

PROCESS_ADDITIONAL_CLASSES = '''
ActId | lps::action_label(const core::identifier_string& name, const data::sort_expression_list& sorts) | An action label
'''

if __name__ == "__main__":
    find_dependencies(
        [ (CONTAINER_TYPES                              , 'data', None),
          (STRUCTURED_SORT_ELEMENTS                     , 'data', None),
          (SORT_EXPRESSION_CLASSES                      , 'data', None),
          (BINDER_TYPES                                 , 'data', None),
          (ASSIGNMENT_EXPRESSION_CLASSES                , 'data', None),
          (ABSTRACTION_CLASS                            , 'data', None),
          (DATA_EXPRESSION_CLASSES_WITHOUT_ABSTRACTION  , 'data', None),
          (ABSTRACTION_EXPRESSIONS                      , 'data', None),
          (DATA_CLASSES                                 , 'data', None),
          (STATE_FORMULA_CLASSES                        , 'state_formulas', 'state_formula'),
          (REGULAR_FORMULA_CLASSES                      , 'regular_formulas', 'regular_formula'),
          (ACTION_FORMULA_CLASSES                       , 'action_formulas', 'action_formula'),
          (LPS_CLASSES                                  , 'lps', None),
          (PROCESS_CLASSES                              , 'process', None),
          (PROCESS_EXPRESSION_CLASSES                   , 'process', 'process_expression'),
          (PBES_CLASSES                                 , 'pbes_system', None),
          (PBES_EXPRESSION_CLASSES                      , 'pbes_system', 'pbes_expression'),
          (BOOLEAN_CLASSES                              , 'bes', None),
          (BOOLEAN_EXPRESSION_CLASSES                   , 'bes', 'boolean_expression')
        ],
        'data_expression',
        'data')

    make_traverser_inc_file('../../lps/include/mcrl2/lps/detail/traverser.inc.h', LPS_CLASSES, namespace = 'lps')

    make_traverser_inc_file('../../process/include/mcrl2/process/detail/traverser.inc.h', PROCESS_ADDITIONAL_CLASSES + PROCESS_EXPRESSION_CLASSES + PROCESS_CLASSES, [('process_expression', PROCESS_EXPRESSION_CLASSES)], namespace = 'process')
    make_builder_inc_file(  '../../process/include/mcrl2/process/detail/process_expression_builder.inc.h', PROCESS_EXPRESSION_CLASSES, 'process_expression')

    make_traverser_inc_file('../../pbes/include/mcrl2/pbes/detail/traverser.inc.h', PBES_EXPRESSION_CLASSES + PBES_CLASSES, [('pbes_expression', PBES_EXPRESSION_CLASSES)], namespace = 'pbes_system')
    make_builder_inc_file(  '../../pbes/include/mcrl2/pbes/detail/pbes_expression_builder.inc.h', PBES_EXPRESSION_CLASSES, 'pbes_expression')

    make_traverser_inc_file('../../bes/include/mcrl2/bes/detail/traverser.inc.h', BOOLEAN_EXPRESSION_CLASSES + BOOLEAN_CLASSES, [('boolean_expression', BOOLEAN_EXPRESSION_CLASSES)], namespace = 'bes')
    make_builder_inc_file(  '../../bes/include/mcrl2/bes/detail/builder.inc.h', BOOLEAN_EXPRESSION_CLASSES, 'boolean_expression')

    make_traverser_inc_file('../../data/include/mcrl2/data/detail/traverser.inc.h', ASSIGNMENT_EXPRESSION_CLASSES + BINDER_TYPES + STRUCTURED_SORT_ELEMENTS + CONTAINER_TYPES + SORT_EXPRESSION_CLASSES + DATA_EXPRESSION_CLASSES_WITHOUT_ABSTRACTION + ABSTRACTION_EXPRESSIONS + DATA_CLASSES, [('data_expression', DATA_EXPRESSION_CLASSES), ('assignment_expression', ASSIGNMENT_EXPRESSION_CLASSES), ('sort_expression', SORT_EXPRESSION_CLASSES), ('container_type', CONTAINER_TYPES), ('binder_type', BINDER_TYPES), ('abstraction', ABSTRACTION_EXPRESSIONS)], namespace = 'data')
    make_builder_inc_file  ('../../data/include/mcrl2/data/detail/data_expression_builder.inc.h', DATA_EXPRESSION_CLASSES_WITHOUT_ABSTRACTION + ABSTRACTION_EXPRESSIONS, 'data_expression')
    make_builder_inc_file  ('../../data/include/mcrl2/data/detail/sort_expression_builder.inc.h', SORT_EXPRESSION_CLASSES, 'sort_expression')

    make_traverser_inc_file('../../lps/include/mcrl2/modal_formula/detail/action_formula_traverser.inc.h', ACTION_FORMULA_CLASSES, [('action_formula', ACTION_FORMULA_CLASSES)], namespace = 'action_formulas')
    make_builder_inc_file(  '../../lps/include/mcrl2/modal_formula/detail/action_formula_builder.inc.h', ACTION_FORMULA_CLASSES, 'action_formula')

    make_traverser_inc_file('../../lps/include/mcrl2/modal_formula/detail/regular_formula_traverser.inc.h', REGULAR_FORMULA_CLASSES, [('regular_formula', REGULAR_FORMULA_CLASSES)], namespace = 'regular_formulas')
    make_builder_inc_file(  '../../lps/include/mcrl2/modal_formula/detail/regular_formula_builder.inc.h', REGULAR_FORMULA_CLASSES, 'regular_formula')

    make_traverser_inc_file('../../lps/include/mcrl2/modal_formula/detail/state_formula_traverser.inc.h', STATE_FORMULA_CLASSES, [('state_formula', STATE_FORMULA_CLASSES)], namespace = 'state_formulas')
    make_builder_inc_file(  '../../lps/include/mcrl2/modal_formula/detail/state_formula_builder.inc.h', STATE_FORMULA_CLASSES, 'state_formula')
