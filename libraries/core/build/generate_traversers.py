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

def make_traverser(filename, classnames, namespace, all_classes):
    result = []       
    for classname in classnames:
        c = all_classes[classname]
        if 'X' in c.modifiers():
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
        else:
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

    ctext = '\n'.join(result)

    #----------------------------------------------------------------------------------------#
    # N.B. THIS IS AN UGLY HACK to deal with the optional time function in some LPS classes
    # TODO: investigate if the time interface can be improved
    ctext = re.sub(r'static_cast<Derived&>\(\*this\)\(x.time\(\)\)', 'if (x.has_time()) static_cast<Derived&>(*this)(x.time());', ctext)   
    #----------------------------------------------------------------------------------------#

    insert_text_in_file(filename, ctext, 'generated code')

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

def print_dependencies(dependencies, message):
    print message
    for type in sorted(dependencies):
        print type, dependencies[type]

def is_dependent_type(dependencies, type):
    if type in dependencies:
        return dependencies[type]
    # TODO: handle template parameters
    elif type == 'Container':
        return True
    m = re.search('<(\w+)>', type)
    if m != None:
        return dependencies[m.group(1)]
    return False

def update_dependency(classname, all_classes, dependencies, value = True):
    dependencies[classname] = value
    if 'C' in all_classes[classname].modifiers():
        dependencies[classname + '_list'] = value
        dependencies[classname + '_vector'] = value

def update_dependencies(all_classes, dependencies):
    changed = False
    for classname in all_classes:
        c = all_classes[classname]

        # check expression class dependencies
        if dependencies[classname] == True:           
            if 'X' in c.modifiers(): # c is an expression super class:
                for expr in c.expression_classes():
                    if dependencies[expr] == False:
                        update_dependency(expr, all_classes, dependencies)
                        changed = True
            continue
        
        # check parameter dependencies
        for p in c.constructor.parameters():
            type = p.type(include_modifiers = False, include_namespace = True)
            if is_dependent_type(dependencies, type):
                update_dependency(classname, all_classes, dependencies)
                changed = True

    return changed

def find_dependencies(all_classes, type):
    dependencies = {} # maps class names to True/False

    # initially set all dependencies to False
    for classname in all_classes:
        update_dependency(classname, all_classes, dependencies, value = False)

    # initial dependency: the expression class depends on itself
    update_dependency(type, all_classes, dependencies, value = True)
    
    while update_dependencies(all_classes, dependencies):
        pass
    return dependencies

# Computes a mapping m, such that m[<type>] returns true if <type> is a type that can be modified in place.
def make_modifiability_map(class_list):
    result = {}
    for item in class_list:
        class_text, namespace = item
        classes = parse_classes(class_text, namespace = namespace)
        for c in classes:
            if c.namespace() != namespace:
                continue
            class_name = c.namespace() + '::' + c.classname()
            result[class_name] = (c.aterm == None) or ('M' in c.modifiers())
    return result

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

PROCESS_ADDITIONAL_CLASSES = '''
ActId | lps::action_label(const core::identifier_string& name, const data::sort_expression_list& sorts) | An action label
'''

if __name__ == "__main__":
    class_list = [
          (CORE_CLASSES                               , 'core'            ),
          (CONTAINER_TYPES                            , 'data'            ),
          (STRUCTURED_SORT_ELEMENTS                   , 'data'            ),
          (SORT_EXPRESSION_CLASSES                    , 'data'            ),
          (BINDER_TYPES                               , 'data'            ),
          (ASSIGNMENT_EXPRESSION_CLASSES              , 'data'            ),
          (ABSTRACTION_CLASS                          , 'data'            ),
          (DATA_EXPRESSION_CLASSES_WITHOUT_ABSTRACTION, 'data'            ),
          (ABSTRACTION_EXPRESSIONS                    , 'data'            ),
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

    boolean_expression_dependencies = find_dependencies(all_classes, 'bes::boolean_expression')
    data_expression_dependencies    = find_dependencies(all_classes, 'data::data_expression')
    pbes_expression_dependencies    = find_dependencies(all_classes, 'pbes_system::pbes_expression')
    process_expression_dependencies = find_dependencies(all_classes, 'process::process_expression')
    sort_expression_dependencies    = find_dependencies(all_classes, 'data::sort_expression')
    action_formula_dependencies     = find_dependencies(all_classes, 'action_formulas::action_formula')
    regular_formula_dependencies    = find_dependencies(all_classes, 'regular_formulas::regular_formula')
    state_formula_dependencies      = find_dependencies(all_classes, 'state_formulas::state_formula')

    #print_dependencies(data_expression_dependencies, '--- data_expression_dependencies ---')

    modifiability_map = make_modifiability_map(class_list)  
    #test_builder_functions(class_list, data_expression_dependencies, modifiability_map)    

    #for t in sorted(modifiability_map):
    #    print t, '->', modifiability_map[t]

    make_traverser('../../bes/include/mcrl2/bes/detail/traverser.inc.h', parse_classnames(BOOLEAN_CLASSES, 'bes') + ['bes::boolean_expression'], 'bes', all_classes)

    make_traverser_inc_file('../../lps/include/mcrl2/lps/detail/traverser.inc.h', LPS_CLASSES, namespace = 'lps')

    make_traverser_inc_file('../../process/include/mcrl2/process/detail/traverser.inc.h', PROCESS_ADDITIONAL_CLASSES + PROCESS_EXPRESSION_CLASSES + PROCESS_CLASSES, [('process_expression', PROCESS_EXPRESSION_CLASSES)], namespace = 'process')
    make_builder_expression_functions(  '../../process/include/mcrl2/process/detail/process_expression_builder.inc.h', PROCESS_EXPRESSION_CLASSES, process_expression_dependencies, 'process_expression', namespace = 'process')

    make_traverser_inc_file('../../pbes/include/mcrl2/pbes/detail/traverser.inc.h', PBES_EXPRESSION_CLASSES + PBES_CLASSES, [('pbes_expression', PBES_EXPRESSION_CLASSES)], namespace = 'pbes_system')
    make_builder_expression_functions(  '../../pbes/include/mcrl2/pbes/detail/data_expression_builder.inc.h', PBES_CLASSES + PBES_EXPRESSION_CLASSES, data_expression_dependencies, 'data_expression', namespace = 'pbes_system')
    make_builder_expression_functions(  '../../pbes/include/mcrl2/pbes/detail/pbes_expression_builder.inc.h', PBES_EXPRESSION_CLASSES, pbes_expression_dependencies, 'pbes_expression', namespace = 'pbes_system')

    #make_traverser_inc_file('../../bes/include/mcrl2/bes/detail/traverser.inc.h', BOOLEAN_EXPRESSION_CLASSES + BOOLEAN_CLASSES, [('boolean_expression', BOOLEAN_EXPRESSION_CLASSES)], namespace = 'bes')
    make_builder_expression_functions(  '../../bes/include/mcrl2/bes/detail/builder.inc.h', BOOLEAN_EXPRESSION_CLASSES, boolean_expression_dependencies, 'boolean_expression', namespace = 'bes')

    make_traverser_inc_file('../../data/include/mcrl2/data/detail/traverser.inc.h', ASSIGNMENT_EXPRESSION_CLASSES + BINDER_TYPES + STRUCTURED_SORT_ELEMENTS + CONTAINER_TYPES + SORT_EXPRESSION_CLASSES + DATA_EXPRESSION_CLASSES_WITHOUT_ABSTRACTION + ABSTRACTION_EXPRESSIONS + DATA_CLASSES, [('data_expression', DATA_EXPRESSION_CLASSES), ('assignment_expression', ASSIGNMENT_EXPRESSION_CLASSES), ('sort_expression', SORT_EXPRESSION_CLASSES), ('container_type', CONTAINER_TYPES), ('binder_type', BINDER_TYPES), ('abstraction', ABSTRACTION_EXPRESSIONS)], namespace = 'data')
    make_builder_expression_functions  ('../../data/include/mcrl2/data/detail/data_expression_builder.inc.h', DATA_EXPRESSION_CLASSES_WITHOUT_ABSTRACTION + ABSTRACTION_EXPRESSIONS, data_expression_dependencies, 'data_expression', namespace = 'data')
    make_builder_expression_functions  ('../../data/include/mcrl2/data/detail/sort_expression_builder.inc.h', SORT_EXPRESSION_CLASSES, sort_expression_dependencies, 'sort_expression', namespace = 'data')

    make_traverser_inc_file('../../lps/include/mcrl2/modal_formula/detail/action_formula_traverser.inc.h', ACTION_FORMULA_CLASSES, [('action_formula', ACTION_FORMULA_CLASSES)], namespace = 'action_formulas')
    make_builder_expression_functions(  '../../lps/include/mcrl2/modal_formula/detail/action_formula_builder.inc.h', ACTION_FORMULA_CLASSES, action_formula_dependencies, 'action_formula', namespace = 'action_formulas')

    make_traverser_inc_file('../../lps/include/mcrl2/modal_formula/detail/regular_formula_traverser.inc.h', REGULAR_FORMULA_CLASSES, [('regular_formula', REGULAR_FORMULA_CLASSES)], namespace = 'regular_formulas')
    make_builder_expression_functions(  '../../lps/include/mcrl2/modal_formula/detail/regular_formula_builder.inc.h', REGULAR_FORMULA_CLASSES, regular_formula_dependencies, 'regular_formula', namespace = 'regular_formulas')

    make_traverser_inc_file('../../lps/include/mcrl2/modal_formula/detail/state_formula_traverser.inc.h', STATE_FORMULA_CLASSES, [('state_formula', STATE_FORMULA_CLASSES)], namespace = 'state_formulas')
    make_builder_expression_functions(  '../../lps/include/mcrl2/modal_formula/detail/state_formula_builder.inc.h', STATE_FORMULA_CLASSES, state_formula_dependencies, 'state_formula', namespace = 'state_formulas')
