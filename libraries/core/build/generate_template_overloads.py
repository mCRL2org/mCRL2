from mcrl2_classes import *
from mcrl2_utility import *

class_map = mcrl2_class_map()
all_classes = parse_class_map(class_map)
modifiability_map = make_modifiability_map(all_classes)

file_map = {
  'action_formulas' : '../../lps/source/lps.cpp',
  'core' : '../../core/source/core.cpp',
  'data' : '../../data/source/data.cpp',
  'lps' : '../../lps/source/lps.cpp',
  'pbes_system' : '../../pbes/source/pbes.cpp',
  'process' : '../../process/source/process.cpp',
  'regular_formulas' : '../../lps/source/lps.cpp',
  'state_formulas' : '../../lps/source/lps.cpp',
}

PP_CLASSNAMES = '''
core::identifier_string
data::sort_expression
data::sort_expression_list
data::sort_expression_vector
data::data_expression
data::data_expression_list
data::data_expression_vector
data::assignment
data::assignment_list
data::assignment_vector
data::variable
data::variable_list
data::variable_vector
data::function_symbol
data::function_symbol_list
data::function_symbol_vector
data::structured_sort_constructor
data::structured_sort_constructor_list
data::structured_sort_constructor_vector
data::data_equation
data::data_equation_list
data::data_equation_vector
data::data_specification
lps::specification
lps::linear_process
lps::action
lps::action_list
lps::action_vector
lps::action_label
lps::action_label_list
lps::action_label_vector
lps::multi_action
lps::process_initializer
pbes_system::fixpoint_symbol
pbes_system::pbes<>
pbes_system::pbes_equation
pbes_system::pbes_equation_vector
pbes_system::pbes_expression
pbes_system::pbes_expression_list
pbes_system::pbes_expression_vector
pbes_system::propositional_variable
pbes_system::propositional_variable_list
pbes_system::propositional_variable_vector
pbes_system::propositional_variable_instantiation
pbes_system::propositional_variable_instantiation_list
pbes_system::propositional_variable_instantiation_vector
process::action_name_multiset
process::process_identifier
process::process_identifier_list
process::process_identifier_vector
process::process_specification
process::process_expression
process::process_expression_list
process::process_expression_vector
process::process_equation
process::process_equation_list
process::process_equation_vector
process::process_instance
process::process_instance_assignment
process::delta
process::tau
process::sum
process::block
process::hide
process::rename
process::comm
process::allow
process::sync
process::at
process::seq
process::if_then
process::if_then_else
process::bounded_init
process::merge
process::left_merge
process::choice
action_formulas::action_formula
regular_formulas::regular_formula
state_formulas::state_formula
'''

NORMALIZE_SORTS_CLASSNAMES = '''
data::data_equation
data::data_equation_list
data::data_equation_vector
data::data_expression
data::sort_expression
data::variable_list
lps::action
lps::action_label_list
lps::multi_action
process::process_equation_vector
process::process_specification
pbes_system::pbes_equation_vector
pbes_system::pbes<>
state_formulas::state_formula
'''

TRANSLATE_USER_NOTATION_CLASSNAMES = '''
data::data_expression
data::data_equation
lps::action
lps::multi_action
pbes_system::pbes<>
process::process_specification
state_formulas::state_formula
'''

FIND_SORT_EXPRESSIONS_CLASSNAMES = '''
data::data_equation
data::data_expression
data::sort_expression
lps::action_label_list
lps::specification
pbes_system::pbes<>
process::process_equation_vector
process::process_expression
process::process_specification
state_formulas::state_formula
'''

FIND_VARIABLES_CLASSNAMES = '''
action_formulas::action_formula
data::data_expression
data::data_expression_list
data::function_symbol
data::variable
data::variable_list
lps::linear_process
lps::specification
lps::deadlock
lps::multi_action
pbes_system::pbes<>
state_formulas::state_formula
'''

FIND_FREE_VARIABLES_CLASSNAMES = '''
data::data_expression
data::data_expression_list
lps::action
lps::linear_process
lps::specification
lps::deadlock
lps::multi_action
lps::process_initializer
pbes_system::pbes<>
pbes_system::pbes_equation
state_formulas::state_formula
'''

FIND_FUNCTION_SYMBOLS_CLASSNAMES = '''
data::data_equation
lps::specification
pbes_system::pbes<>
'''

FIND_PROPOSITIONAL_VARIABLE_INSTANTIATIONS_CLASSNAMES = '''
pbes_system::pbes_expression
'''

FIND_IDENTIFIERS_CLASSNAMES = '''
data::variable_list
lps::specification
process::process_specification
pbes_system::pbes_expression
state_formulas::state_formula
'''

FIND_NIL_CLASSNAMES = '''
state_formulas::state_formula
'''

SEARCH_VARIABLE_CLASSNAMES = '''
data::data_expression
pbes_system::pbes_expression
'''

def has_specification(type):
    return type.endswith('specification') or type.endswith('pbes<>')

def is_modifiable(type):
    if type in modifiability_map:
        return modifiability_map[type]
    elif type.endswith('_list'):
        return False
    elif type.endswith('_vector'):
        return True
    elif type.endswith('pbes<>'):
        return True
    elif type.endswith('vector<pbes_equation>'):
        return True
    raise Exception('Unknown type %s!' % type)

def extract_namespace(classname):
    return re.sub('::.*', '', classname)

def generate_traverser_overloads(classnames, function, return_type, result):
    for classname in classnames:
        namespace = extract_namespace(classname)
        text = re.sub('>>', '> >', '%s %s(const %s& x) { return %s::%s< %s >(x); }\n' % (return_type, function, classname, namespace, function, classname))
        result[namespace].append(text)

def generate_builder_overloads(classnames, function, result):
    for classname in classnames:
        namespace = extract_namespace(classname)
        if is_modifiable(classname):
            text = 'void %s(%s& x) { %s::%s< %s >(x); }\n' % (function, classname, namespace, function, classname)
        else:
            text = '%s %s(const %s& x) { return %s::%s< %s >(x); }\n' % (classname, function, classname, namespace, function, classname)
        result[namespace].append(text)

# special because of additional variable argument
def generate_search_variable_overloads(classnames, function, return_type, result):
    for classname in classnames:
        namespace = extract_namespace(classname)
        text = re.sub('>>', '> >', '%s %s(const %s& x, const data::variable& v) { return %s::%s< %s >(x, v); }\n' % (return_type, function, classname, namespace, function, classname))
        result[namespace].append(text)

# special because of additional data_specification argument
def generate_normalize_sorts_overloads(classnames, result):
    for classname in classnames:
        namespace = extract_namespace(classname)
        if is_modifiable(classname):
            text = 'void normalize_sorts(%s& x, const data::data_specification& dataspec) { %s::normalize_sorts< %s >(x, dataspec); }\n' % (classname, namespace, classname)
        else:
            text = '%s normalize_sorts(const %s& x, const data::data_specification& dataspec) { return %s::normalize_sorts< %s >(x, dataspec); }\n' % (classname, classname, namespace, classname)
        if has_specification(classname):
            text = re.sub('x, dataspec', 'x, x.data()', text)
            text = re.sub('& dataspec', '& /* dataspec */', text)
        result[namespace].append(text)

result = {}
for namespace in file_map:
    result[namespace] = []

classnames = PP_CLASSNAMES.strip().split()
generate_traverser_overloads(classnames, 'pp', 'std::string', result)

classnames = NORMALIZE_SORTS_CLASSNAMES.strip().split()
generate_normalize_sorts_overloads(classnames, result)

classnames = TRANSLATE_USER_NOTATION_CLASSNAMES.strip().split()
generate_builder_overloads(classnames, 'translate_user_notation', result)

classnames = FIND_SORT_EXPRESSIONS_CLASSNAMES.strip().split()
generate_traverser_overloads(classnames, 'find_sort_expressions', 'std::set<data::sort_expression>', result)

classnames = FIND_VARIABLES_CLASSNAMES.strip().split()
generate_traverser_overloads(classnames, 'find_variables', 'std::set<data::variable>', result)

classnames = FIND_FREE_VARIABLES_CLASSNAMES.strip().split()
generate_traverser_overloads(classnames, 'find_free_variables', 'std::set<data::variable>', result)

classnames = FIND_FUNCTION_SYMBOLS_CLASSNAMES.strip().split()
generate_traverser_overloads(classnames, 'find_function_symbols', 'std::set<data::function_symbol>', result)

classnames = FIND_PROPOSITIONAL_VARIABLE_INSTANTIATIONS_CLASSNAMES.strip().split()
generate_traverser_overloads(classnames, 'find_propositional_variable_instantiations', 'std::set<pbes_system::propositional_variable_instantiation>', result)

classnames = FIND_IDENTIFIERS_CLASSNAMES.strip().split()
generate_traverser_overloads(classnames, 'find_identifiers', 'std::set<core::identifier_string>', result)

classnames = FIND_NIL_CLASSNAMES.strip().split()
generate_traverser_overloads(classnames, 'find_nil', 'bool', result)

classnames = SEARCH_VARIABLE_CLASSNAMES.strip().split()
generate_search_variable_overloads(classnames, 'search_variable', 'bool', result)

for namespace in result:
    filename = file_map[namespace]
    text = ''.join(result[namespace])
    label = 'generated %s overloads' % namespace
    insert_text_in_file(filename, text, label)

    print '--- %s ---' % namespace
    print re.sub(' \{.*\}', ';', text)