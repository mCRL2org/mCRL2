from mcrl2_classes import *
from mcrl2_utility import *

class_map = mcrl2_class_map()
all_classes = parse_class_map(class_map)
modifiability_map = make_modifiability_map(all_classes)

CORE = '''
identifier_string
'''

DATA = '''
sort_expression
sort_expression_list
sort_expression_vector
data_expression
data_expression_list
data_expression_vector
assignment
assignment_list
assignment_vector
variable
variable_list
variable_vector
function_symbol
function_symbol_list
function_symbol_vector
structured_sort_constructor
structured_sort_constructor_list
structured_sort_constructor_vector
data_equation
data_equation_list
data_equation_vector
'''

LPS = '''
specification
linear_process
action
action_list
action_vector
action_label
action_label_list
action_label_vector
multi_action
process_initializer
'''

PBES = '''
fixpoint_symbol
pbes<>
pbes_equation
atermpp::vector<pbes_equation>
pbes_expression
pbes_expression_list
pbes_expression_vector
propositional_variable
propositional_variable_list
propositional_variable_vector
propositional_variable_instantiation
propositional_variable_instantiation_list
propositional_variable_instantiation_vector
'''

PROCESS = '''
action_name_multiset
process_identifier
process_identifier_list
process_identifier_vector
process_specification
process_expression
process_expression_list
process_expression_vector
process_instance
process_instance_assignment
delta
tau
sum
block
hide
rename
comm
allow
sync
at
seq
if_then
if_then_else
bounded_init
merge
left_merge
choice
'''

ACTION_FORMULAS = '''
action_formula
'''

REGULAR_FORMULAS = '''
regular_formula
'''

STATE_FORMULAS = '''
state_formula
'''

def pp_overloads(text, namespace):
    words = text.split()
    for word in words:
        print 'std::string pp(const %s& x);' % word

    result = ''
    for word in words:
        result = result + re.sub('>>', '> >', 'std::string pp(const %s& x) { return %s::pp< %s >(x); }\n' % (word, namespace, word))
    return result

def normalize_sorts_overloads(text, namespace):
    result = ''
    words = text.split()
    for word in words:
        type = '%s::%s' % (namespace, word)
        if modifiability_map[type]:
            print 'void normalize_sorts(%s& x, const data::data_specification& dataspec);' % word
            result = result + 'void normalize_sorts(%s& x, const data::data_specification& dataspec) { %s::normalize_sorts(x, dataspec); }\n' % (word, namespace)
        else:
            print '%s normalize_sorts(const %s& x, const data::data_specification& dataspec);' % (word, word)
            result = result + '%s normalize_sorts(const %s& x, const data::data_specification& dataspec) { return %s::normalize_sorts(x, dataspec); }\n' % (word, word, namespace)
    return result

def make_overloads(filename, text, namespace):
    insert_text_in_file(filename, text, 'generated %s overloads' % namespace)

filename = '../../core/source/core.cpp'
namespace = 'core'
text = pp_overloads(CORE, namespace)
make_overloads(filename, text, namespace)

filename = '../../data/source/data.cpp'
namespace = 'data'
text = pp_overloads(DATA, namespace)
make_overloads(filename, text, namespace)

filename = '../../lps/source/lps.cpp'
namespace = 'action_formulas'
text = pp_overloads(ACTION_FORMULAS , namespace)
make_overloads(filename, text, namespace)

filename = '../../lps/source/lps.cpp'
namespace = 'regular_formulas'
text = pp_overloads(REGULAR_FORMULAS, namespace)
make_overloads(filename, text, namespace)

filename = '../../lps/source/lps.cpp'
namespace = 'state_formulas'
text = pp_overloads(STATE_FORMULAS , namespace)
make_overloads(filename, text, namespace)

filename = '../../lps/source/lps.cpp'
namespace = 'lps'
text = pp_overloads(LPS , namespace)
make_overloads(filename, text, namespace)

filename = '../../pbes/source/pbes.cpp'
namespace = 'pbes_system'
text = pp_overloads(PBES , namespace)
make_overloads(filename, text, namespace)

filename = '../../process/source/process.cpp'
namespace = 'process'
text = pp_overloads(PROCESS , namespace)
make_overloads(filename, text, namespace)

normalize_sorts_overloads(LPS, 'lps')
