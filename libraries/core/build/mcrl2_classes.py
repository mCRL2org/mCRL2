#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

# This file contains tables that are used to generate classes and traversal functions
# for these classes. A prerequisite is that each class has a corresponding ATerm
# representation (the first column of each table). The second column contains the
# constructor of the classes. If the name of the class has a postfix between brackets
# like variable[_base] then the generated class will be called variable_base, but the
# traversal will use variable. This is done to enable the user of the class to add
# additional behavior to the base class.

import re
import string

CONTAINER_TYPES = r'''
SortList 	| list_container() | Container type for lists
SortSet		| set_container() | Container type for sets
SortBag		| bag_container() | Container type for bags
SortFSet	| finite_set_container() | Container type for finite sets
SortFBag	| finite_bag_container() | Container type for finite bags
'''

STRUCTURED_SORT_ELEMENTS = r'''
StructCons	| structured_sort_constructor_base(const core::identifier_string& name, const structured_sort_constructor_argument_list& arguments, core::identifier_string& recogniser) | A constructor for a structured sort
StructProj	| structured_sort_constructor_argument_base(const core::identifier_string& name, const sort_expression& sort) | An argument of a constructor of a structured sort
'''

SORT_EXPRESSION_CLASSES = r'''
SortId		| basic_sort_base(const core::identifier_string& name) | A basic sort
SortCons	| container_sort_base(const container_type& container_name, const sort_expression& s) | A container sort
SortStruct	| structured_sort_base(const structured_sort_constructor_list& constructors) | A structured sort
SortArrow	| function_sort_base(const sort_expression_list& domain, const sort_expression& codomain) | A function sort
SortUnknown	| unknown_sort_base() | Unknown sort expression
SortsPossible	| multiple_possible_sorts_base(const sort_expression_list& sorts) | Multiple possible sorts
'''

BINDER_TYPES = r'''
SetBagComp 	| set_or_bag_comprehension_binder() | Binder for set or bag comprehension
SetComp		| set_comprehension_binder()	| Binder for set comprehension
BagComp		| bag_comprehension_binder()	| Binder for bag comprehension
Forall		| forall_binder()		| Binder for universal quantification
Exists		| exists_binder()		| Binder for existential quantification
Lambda		| lambda_binder()		| Binder for lambda abstraction
'''

ASSIGNMENT_EXPRESSION_CLASSES = r'''
DataVarIdInit	| assignment[_base](const variable& lhs, const data_expression& rhs) | Assignment of a data expression to a variable)
IdInit		| identifier_assignment[_base](const identifier& lhs, const data_expression& rhs) | Assignment of a data expression to a string
'''

DATA_EXPRESSION_CLASSES = r'''
Id		| identifier[_base](const core::identifier_string& name) | An identifier
DataVarId	| variable[_base](const core::identifier_string& name, const sort_expression& sort)	| A data variable
OpId		| function_symbol[_base](const core::identifier_string& name, const sort_expression& sort) | A function symbol
DataAppl	| application[_base](const data_expression& head, data_expression_list const& arguments) | An application of a data expression to a number of arguments
Binder		| abstraction[_base](const binder_type& binding_operator, const variable_list& variables, const data_expression& body) | An abstraction expression.
Whr		| where_clause[_base](const data_expression& body, const assignment_expression_list& declarations) | A where expression
'''

STATE_FORMULA_CLASSES = r'''
StateTrue       | true_()                                                                                                         | The value true for state formulas
StateFalse      | false_()                                                                                                        | The value false for state formulas
StateNot        | not_(const state_formula& operand)                                                                              | The not operator for state formulas
StateAnd        | and_(const state_formula& left, const state_formula& right)                                                     | The and operator for state formulas
StateOr         | or_(const state_formula& left, const state_formula& right)                                                      | The or operator for state formulas
StateImp        | imp(const state_formula& left, const state_formula& right)                                                      | The implication operator for state formulas
StateForall     | forall(const data::variable_list& variables, const state_formula& operand)                                      | The universal quantification operator for state formulas
StateExists     | exists(const data::variable_list& variables, const state_formula& operand)                                      | The existential quantification operator for state formulas
StateMust       | must(const regular_formulas::regular_formula& formula, const state_formula& operand)                            | The must operator for state formulas
StateMay        | may(const regular_formulas::regular_formula& formula, const state_formula& operand)                             | The may operator for state formulas
StateYaled      | yaled()                                                                                                         | The yaled operator for state formulas
StateYaledTimed | yaled_timed(const data::data_expression& time_stamp)                                                            | The timed yaled operator for state formulas
StateDelay      | delay()                                                                                                         | The delay operator for state formulas      
StateDelayTimed | delay_timed(const data::data_expression& time_stamp)                                                            | The timed delay operator for state formulas
StateVar        | variable(const core::identifier_string& name, const data::data_expression_list& arguments)                      | The state formula variable
StateNu         | nu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand) | The nu operator for state formulas
StateMu         | mu(const core::identifier_string& name, const data::assignment_list& assignments, const state_formula& operand) | The mu operator for state formulas
'''                                                                                                                                 
                                                                                                                                    
ACTION_FORMULA_CLASSES = r'''                                                                                                       
ActTrue   | true_()                                                                     | The value true for action formulas  
ActFalse  | false_()                                                                    | The value false for action formulas 
ActNot    | not_(const action_formula& operand)                                         | The not operator for action formulas
ActAnd    | and_(const action_formula& left, const action_formula& right)               | The and operator for action formulas
ActOr     | or_(const action_formula& left, const action_formula& right)                | The or operator for action formulas 
ActImp    | imp(const action_formula& left, const action_formula& right)                | The implication operator for action formulas               
ActForall | forall(const data::variable_list& variables, const action_formula& operand) | The universal quantification operator for action formulas  
ActExists | exists(const data::variable_list& variables, const action_formula& operand) | The existential quantification operator for action formulas
ActAt     | at(const action_formula& operand, const data::data_expression& time_stamp)  | The at operator for action formulas
'''                                                                                       

# N.B. This one is problematic due to the optional time in deadlock/multi_action.
LPS_CLASSES = r'''
ActId             | action_label(const core::identifier_string& name, const data::sort_expression_list& sorts) | An action label
Action            | action(const action_label& label, const data::data_expression_list& arguments) | An action
None              | deadlock(const data::data_expression& time) | A deadlock
None              | multi_action(const action_list& actions, const data::data_expression& time) | A multi-action
None              | deadlock_summand(const data::variable_list& summation_variables, const data::data_expression& condition, const lps::deadlock& deadlock) | A deadlock summand
None              | action_summand(const data::variable_list& summation_variables, const data::data_expression& condition, const lps::multi_action& multi_action, const data::assignment_list& assignments) | An action summand
LinearProcessInit | process_initializer(const data::assignment_list& assignments) | A process initializer
LinearProcess     | linear_process(const data::variable_list& process_parameters, const deadlock_summand_vector& deadlock_summands, const action_summand_vector& action_summands) | A linear process
LinProcSpec       | specification(const data::data_specification& data, const action_label_list& action_labels, const atermpp::set<data::variable>& global_variables,const linear_process& process, const process_initializer& initial_process) | A linear process specification
'''

PROCESS_CLASSES = r'''
ProcSpec    | process_specification(const data::data_specification& data, const lps::action_label_list& action_labels, const process_equation_list& equations, const process_expression& init) | A process specification
ProcVarId   | process_identifier(const core::identifier_string& name, const data::sort_expression_list& sorts) | A process identifier
ProcEqn     | process_equation(const process_identifier& identifier, const data::variable_list& formal_parameters, const process_expression& expression) | A process equation
RenameExpr  | rename_expression(core::identifier_string source, core::identifier_string target) | A rename expression
CommExpr    | communication_expression(const action_name_multiset& action_name, const core::identifier_string& name) | A communication expression
MultActName | action_name_multiset(const core::identifier_string_list& names) | A multi-action
'''

PROCESS_EXPRESSION_CLASSES = r'''
Action            | lps::action(const lps::action_label& label, const data::data_expression_list& arguments)                                       | An action
Process           | process_instance(const process_identifier identifier, const data::data_expression_list& actual_parameters)                     | A process
ProcessAssignment | process_instance_assignment(const process_identifier& identifier, const data::assignment_list& assignments)                    | A process assignment
Delta             | delta()                                                                                                                        | The value delta
Tau               | tau()                                                                                                                          | The value tau
Sum               | sum(const data::variable_list& bound_variables, const process_expression& operand)                                             | The sum operator
Block             | block(const core::identifier_string_list& block_set, const process_expression& operand)                                        | The block operator
Hide              | hide(const core::identifier_string_list& hide_set, const process_expression& operand)                                          | The hide operator
Rename            | rename(const rename_expression_list& rename_set, const process_expression& operand)                                            | The rename operator
Comm              | comm(const communication_expression_list& comm_set, const process_expression& operand)                                         | The communication operator
Allow             | allow(const action_name_multiset_list& allow_set, const process_expression& operand)                                           | The allow operator
Sync              | sync(const process_expression& left, const process_expression& right)                                                          | The synchronization operator
AtTime            | at(const process_expression& operand, const data::data_expression& time_stamp)                                                 | The at operator
Seq               | seq(const process_expression& left, const process_expression& right)                                                           | The sequential composition
IfThen            | if_then(const data::data_expression& condition, const process_expression& then_case)                                           | The if-then operator
IfThenElse        | if_then_else(const data::data_expression& condition, const process_expression& then_case, const process_expression& else_case) | The if-then-else operator
BInit             | bounded_init(const process_expression& left, const process_expression& right)                                                  | The bounded initialization
Merge             | merge(const process_expression& left, const process_expression& right)                                                         | The merge operator
LMerge            | left_merge(const process_expression& left, const process_expression& right)                                                    | The left merge operator
Choice            | choice(const process_expression& left, const process_expression& right)                                                        | The choice operator
'''

PBES_CLASSES = r'''
FixPoint    | fixpoint_symbol() | A fixpoint symbol
PropVarDecl | propositional_variable(const core::identifier_string& name, const data::variable_list& parameters) | A propositional variable declaration
PropVarInst | propositional_variable_instantiation(const core::identifier_string& name, const data::data_expression_list& parameters) | A propositional variable instantiation
PBEqn       | pbes_equation(const fixpoint_symbol& symbol, const propositional_variable& variable, const pbes_expression& formula) | A PBES equation
PBES        | pbes<Container>(const data::data_specification& data, const Container& equations, const atermpp::set<data::variable>& global_variables, const propositional_variable_instantiation& initial_state) | A PBES
'''

PBES_EXPRESSION_CLASSES = r'''
PBESTrue          | true_()                                                                   | The value true for pbes expressions                         
PBESFalse         | false_()                                                                  | The value false for pbes expressions                        
PBESNot           | not_(const pbes_expression& operand)                                      | The not operator for pbes expressions                       
PBESAnd           | and_(const pbes_expression& left, const pbes_expression& right)           | The and operator for pbes expressions                       
PBESOr            | or_(const pbes_expression& left, const pbes_expression& right)            | The or operator for pbes expressions                        
PBESImp           | imp(const pbes_expression& left, const pbes_expression& right)            | The implication operator for pbes expressions               
PBESForall        | forall(const data::variable_list& variables, const pbes_expression& body) | The universal quantification operator for pbes expressions  
PBESExists        | exists(const data::variable_list& variables, const pbes_expression& body) | The existential quantification operator for pbes expressions
'''

# removes 'const' and '&' from a type
def extract_type(text):
    text = re.sub(r'\bconst\b', '', text)
    text = re.sub(r'\s*&$', '', text)
    text = text.strip()
    return text

#indents the text with the given prefix
def indent_text(text, indent):
    lines = []
    for line in string.split(text, '\n'):
        lines.append(indent + line)
    return string.join(lines, '\n')

# Generates a member function of a class, by substituting values in a template
def member_function(arg, n):
    MEMBER_FUNCTION = '''    TYPE NAME() const
    {
      return atermpp::ARG(*this);
    }'''

    p = arg.rpartition(' ')
    type = extract_type(p[0].strip())
    name = p[2].strip()
    arg = 'arg' + str(n)
    # TODO: this check for a list is unsafe; the ATerm grammar should be used to make it precise
    if type.endswith('list'):
        arg = 'list_' + arg
    text = MEMBER_FUNCTION
    text = re.sub('TYPE', type, text)
    text = re.sub('NAME', name, text)
    text = re.sub('ARG', arg, text)
    return text

# Represents a variable declaration like the following;
#
# const core::identifier_string& name
class VariableDeclaration:
    def __init__(self, text):
        self.text = text.strip()

    # returns the type of the variable
    #
    # 'const core::identifier_string&'
    def type(self):
        return re.sub(r'\s+\S+$', '', self.text)

    # returns the name of the variable
    #
    # 'name'
    def name(self):
        return self.text.split(' ')[-1]

    def __repr__(self):
        return '%s %s' % (self.type(), self.name())

# Represents a function declaration like the following;
#
# variable(const core::identifier_string& name, const data::data_expression_list& arguments)
class FunctionDeclaration:
    def __init__(self, text):
        self.text = text.strip()

    def __repr__(self):
        return self.text
    
    # returns the name of the function including a namespace qualification, if available
    #
    # 'name'
    def qualified_name(self):
        return re.sub('\(.*', '', self.text)

    # returns the namespace qualifier of the function
    #
    def qualifier(self):
        if self.qualified_name().find('::') == -1:
            return ''
        return re.sub('::.*$', '::', self.qualified_name())

    # returns the name of the function without namespace qualification
    #
    def name(self):
        return re.sub(r'^.*::', '', self.qualified_name())

    # returns true if it is a template function
    #
    def is_template(self):
        return self.name().find('<') != -1

    # returns the template arguments of the function
    #
    def template_arguments(self):
        m = re.search('<(.*)>', self.name())
        if m == None:
            return []
        text = m.group(1)
        text = re.sub(r',', '', text)
        return text.rsplit(r'\s')

    # returns the argument text of the function
    #
    # 'const core::identifier_string& name, const data::data_expression_list& arguments'
    def argument_text(self):
        text = self.text
        text = re.sub('.*\(', '', text)
        text = re.sub('\).*', '', text)
        return text

    # returns the parameters of the function as a sequence of VariableDeclarations
    def parameters(self):
        text = self.argument_text()
        words = map(string.strip, text.split(','))
        if len(words) == 1 and words[0] == '':
            words = []
        return map(VariableDeclaration, words)

    # generates class member functions for the parameters like this:
    #
    #    core::identifier_string name() const
    #    {
    #      return atermpp::arg1(*this);
    #    }    
    def class_member_functions(self):
        result = []
        index = 1
        for p in self.parameters():
            result.append(member_function(str(p), index))
            index = index + 1
        return result

DERIVED_CLASS_CONSTRUCTOR = r'''

    /// \\\\brief Constructor.
    <CONSTRUCTOR>
      : <SUPERCLASS>(<NAMESPACE>::detail::gsMake<ATERM>(<PARAMETERS>))
    {}
'''

DERIVED_CLASS_DEFINITION = r'''/// \\brief <DESCRIPTION>
class <CLASSNAME>: public <SUPERCLASS>
{
  public:
    /// \\brief Default constructor.
    <CLASSNAME>()
      : <SUPERCLASS>(<NAMESPACE>::detail::construct<ATERM>())
    {}

    /// \\brief Constructor.
    /// \\param term A term
    <CLASSNAME>(atermpp::aterm_appl term)
      : <SUPERCLASS>(term)
    {
      assert(<NAMESPACE>::detail::check_term_<ATERM>(m_term));
    }<CONSTRUCTOR><MEMBER_FUNCTIONS>
};
'''

CLASS_CONSTRUCTOR = '''

    /// \\\\brief Constructor.
    <CONSTRUCTOR>
    {}
'''

CLASS_DEFINITION = r'''/// \\brief <DESCRIPTION>
class <CLASSNAME>
{
  public:
    /// \\brief Default constructor.
    <CLASSNAME>()
      : <SUPERCLASS>(<NAMESPACE>::detail::construct<ATERM>())
    {}

    /// \\brief Constructor.
    /// \\param term A term
    <CLASSNAME>(atermpp::aterm_appl term)
    {
      assert(<NAMESPACE>::detail::check_term_<ATERM>(m_term));
    }<CONSTRUCTOR><MEMBER_FUNCTIONS>
};
'''

CONTAINER_TYPEDEFS = r'''/// \\brief list of CLASSNAMEs
    typedef atermpp::term_list<<CLASSNAME>> CLASSNAME_list;

    /// \\brief vector of CLASSNAMEs
    typedef atermpp::vector<<CLASSNAME>>    CLASSNAME_vector;
'''

# Represents a class definition
#
# it can be initialized with a string like this:
#
# ActTrue   | true_()  | The value true for action formulas  
#
# self.aterm:       the name of the corresponding ATerm
# self.constructor: the constructor of the class
# self.description: a string description of the class
class Class:
    def __init__(self, aterm, constructor, description):
        self.aterm = aterm
        self.description = description
        name = re.sub('\(.*', '', constructor)
        arguments = re.sub('.*\(', '(', constructor)
        self.classname = re.sub('\[[^]]*\]', '', name)
        self.base_classname = re.sub('\[|\]', '', name)
        self.constructor = FunctionDeclaration(self.classname + arguments)
        self.base_class_constructor = FunctionDeclaration(self.base_classname + arguments)

    # Returns the name of the class including a namespace qualification, if available
    #
    def qualified_name(self):
        return self.constructor.qualified_name()

    # Returns the namespace qualifier of the class (or '' if not available)
    def qualifier(self):
        return self.constructor.qualifier()

    # Returns the name of the class without namespace qualification
    #
    def name(self, use_base_class_name = False):
        if use_base_class_name:
            return self.base_class_constructor.name()
        else:
            return self.constructor.name()

    # Returns the class definition
    def class_definition(self, superclass = None, namespace = 'core', use_base_class_name = True, add_container_typedefs = True):
        f = self.constructor
        if use_base_class_name:
            classname = self.base_classname
            constructor = str(self.base_class_constructor)
        else:
            classname = self.classname
            constructor = str(self.constructor)

        print 'generating class', classname

        member_functions = f.class_member_functions()
        mtext = '\n\n'.join(member_functions)
        if mtext != '':
            mtext = '\n' + mtext

        parameters = [p.name() for p in f.parameters()]
        ptext = ', '.join(parameters)

        if superclass == None:
            if len(self.constructor.parameters()) > 0:
                ctext = CLASS_CONSTRUCTOR
            else:
                ctext = ''
            text = re.sub('<CONSTRUCTOR>', ctext, CLASS_DEFINITION)
        else:
            if len(self.constructor.parameters()) > 0:
                ctext = DERIVED_CLASS_CONSTRUCTOR
            else:
                ctext = ''
            text = re.sub('<CONSTRUCTOR>', ctext, DERIVED_CLASS_DEFINITION)

        text = re.sub('<DESCRIPTION>'     , self.description, text)
        text = re.sub('<CLASSNAME>'       , classname, text)
        text = re.sub('<ATERM>'           , self.aterm, text)
        text = re.sub('<CONSTRUCTOR>'     , constructor, text)
        text = re.sub('<PARAMETERS>'      , ptext      , text)
        text = re.sub('<NAMESPACE>'       , namespace  , text)
        if superclass != None:
            text = re.sub('<SUPERCLASS>'      , superclass , text)
        text = re.sub('<MEMBER_FUNCTIONS>', mtext, text)
        if add_container_typedefs and (superclass == None):
            atext = re.sub('<CLASSNAME>', classname, CONTAINER_TYPEDEFS)
            if use_base_class_name and (self.classname != self.base_classname):
                atext = 'class %s;\n\n' % classname + atext
            text = text + atext
        return text

# parses lines that contain entries separated by '|'
# empty lines are removed
#
# example input:
#
# ActTrue   | true_()  | The value true for action formulas  
# ActFalse  | false_() | The value false for action formulas 
#
# Each line is split w.r.t. the '|' character; the words of the line
# are put in a tuple, and the sequence of tuples is returned
#
# If the name of a function contains a postfix between brackets (like variable[_base]),
# then the parameter use_base_class determines whether it is used or not.
def parse_classes(text):
    result = []
    lines = text.rsplit('\n')
    for line in lines:
        words = map(string.strip, line.split('|'))
        if len(words) < 3:
            continue
        aterm, constructor, description = words
        result.append(Class(aterm, constructor, description))
    return result
