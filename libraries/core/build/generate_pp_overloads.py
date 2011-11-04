from mcrl2_utility import *

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

def make_overloads(filename, text, namespace):
    words = text.split()
    for word in words:
        print 'std::string pp(const %s& x);' % word

    stext = ''
    for word in words:
        stext = stext + re.sub('>>', '> >', 'std::string pp(const %s& x) { return %s::pp< %s >(x); }\n' % (word, namespace, word))
    insert_text_in_file(filename, stext, 'generated %s overloads' % namespace)

make_overloads('../../core/source/core.cpp', CORE, 'core')
make_overloads('../../data/source/data.cpp', DATA, 'data')
make_overloads('../../lps/source/lps.cpp', ACTION_FORMULAS, 'action_formulas')
make_overloads('../../lps/source/lps.cpp', REGULAR_FORMULAS, 'regular_formulas')
make_overloads('../../lps/source/lps.cpp', STATE_FORMULAS, 'state_formulas')
make_overloads('../../lps/source/lps.cpp', LPS, 'lps')
make_overloads('../../pbes/source/pbes.cpp', PBES, 'pbes_system')
make_overloads('../../process/source/process.cpp', PROCESS, 'process')
