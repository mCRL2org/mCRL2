import re
import string
from path import *

# removes 'const' and '&' from a type
def extract_type(text):
    text = re.sub('^const\s*', '', text)
    text = re.sub('\s*&$', '', text)
    return text

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
    
    # returns the name of the function
    #
    # 'name'
    def name(self):
        return re.sub('\(.*', '', self.text)

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

# /// \brief The or operator for state formulas
# class or_: public state_formula
# {
#   public:
#     /// \brief Constructor.
#     /// \param term A term
#     or_(atermpp::aterm_appl term)
#       : state_formula(term)
#     {
#       assert(core::detail::check_term_StateOr(m_term));
#     }
# 
#     /// \brief Constructor.
#     /// \param left A process expression
#     /// \param d A data expression
#     or_(const state_formula& left, const state_formula& right)
#       : or_(core::detail::gsMakeStateOr(left, right))
#     {}
# };
CLASS_DEFINITION = r'''/// \\brief DESCRIPTION
class CLASSNAME: public SUPERCLASS
{
  public:
    /// \\brief Constructor.
    /// \\param term A term
    CLASSNAME(atermpp::aterm_appl term)
      : SUPERCLASS(term)
    {
      assert(core::detail::check_term_ATERM(m_term));
    }

    /// \\brief Constructor.
    CONSTRUCTOR
      : SUPERCLASS(core::detail::gsMakeATERM(PARAMETERS))
    {}MEMBER_FUNCTIONS
};'''

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

# Terms starting with @ are defined in another library
PROCESS_EXPRESSION_CLASSES = r'''
Action            | process_action(const lps::action_label& label, const data::data_expression_list& arguments)                                    | An action
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

# parses lines that contain entries separated by '|'
# empty lines are removed
#
# example input:
#
# ActTrue   | true_()  | The value true for action formulas  
# ActFalse  | false_() | The value false for action formulas 
#
# each line is split w.r.t. the '|' character; the words of the line
# are put in a tuple, and the sequence of tuples is returned
def parse_classes(text):
    result = []
    lines = text.rsplit('\n')
    for line in lines:
        words = map(string.strip, line.split('|'))
        if len(words) < 2:
            continue
        result.append(words)
    return result

# generates the actual code for expression classses, by substituting values in the
# template CLASS_DEFINITION
#
# example input:
#
# ActTrue   | true_()  | The value true for action formulas  
# ActFalse  | false_() | The value false for action formulas 
#
# returns a sequence of class definitions
def generate_classes(text, superclass):
    result = []
    classes = parse_classes(text)
    for c in classes:
        (aterm, constructor, description) = c
        f = FunctionDeclaration(constructor)
        classname = f.name()

        member_functions = f.class_member_functions()
        mtext = '\n\n'.join(member_functions)
        if mtext != '':
            mtext = '\n\n' + mtext

        parameters = [p.name() for p in f.parameters()]
        ptext = ', '.join(parameters)

        ctext = CLASS_DEFINITION
        ctext = re.sub('DESCRIPTION'     , description, ctext)
        ctext = re.sub('CLASSNAME'       , classname  , ctext)
        ctext = re.sub('ATERM'           , aterm      , ctext)
        ctext = re.sub('CONSTRUCTOR'     , constructor, ctext)
        ctext = re.sub('PARAMETERS'      , ptext      , ctext)
        ctext = re.sub('SUPERCLASS'      , superclass , ctext)
        ctext = re.sub('MEMBER_FUNCTIONS', mtext, ctext)
        result.append(ctext)
    return result
                                                                                          
def make_expression_classes(filename, class_text, class_name):
    classes = generate_classes(class_text, class_name)
    ctext = '\n\n'.join(classes) + '\n'
    text = path(filename).text()
    text = re.compile(r'//--- start generated expression classes ---//.*//--- end generated expression classes ---//', re.S).sub(
                  '//--- start generated expression classes ---//\n' + ctext + '//--- end generated expression classes ---//',
                  text)
    path(filename).write_text(text)   

EXPRESSION_VISITOR_CODE = r'''/// \\brief Visitor class for MYEXPRESSIONs.
///
/// There is a visit_<node> and a leave_<node>
/// function for each type of node. By default these functions do nothing, so they
/// must be overridden to add behavior. If the visit_<node> function returns true,
/// the recursion is continued in the children of the node.
template <typename Arg=void>
struct MYEXPRESSION_visitor
{
  /// \\brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \\brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \\brief Destructor.
  virtual ~MYEXPRESSION_visitor()
  { }
%s
  /// \\brief Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  /// \param a An additional argument for the recursion
  void visit(const MYEXPRESSION& x, Arg& a)
  {
%s
  }
};

/// \\brief Visitor class for expressions.
///
/// If a visit_<node> function returns true, the recursion is continued
/// in the children of this node, otherwise not.
template <>
struct MYEXPRESSION_visitor<void>
{
  /// \\brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \\brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \\brief Destructor.
  virtual ~MYEXPRESSION_visitor()
  { }
%s

  /// \\brief Visits the nodes of the expression and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  void visit(const MYEXPRESSION& x)
  {
%s
  }
};
'''

EXPRESSION_BUILDER_CODE = r'''/// \\brief Modifying visitor class for expressions.
///
/// During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns MYEXPRESSION(), the recursion is continued
/// in the children of this node, otherwise not.
/// An arbitrary additional argument may be passed during the recursion.
template <typename Arg = void>
struct MYEXPRESSION_builder
{
  /// \\brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \\brief Returns true if the expression is not equal to MYEXPRESSION().
  /// This is used to determine if the recursion in a node needs to be continued.
  /// \\param x A expression
  /// \\return True if the term is not equal to MYEXPRESSION()
  bool is_finished(const MYEXPRESSION& x)
  {
    return x != MYEXPRESSION();
  }

  /// \\brief Destructor.
  virtual ~MYEXPRESSION_builder()
  { }
%s
  /// \\brief Visits the nodes of the expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals MYEXPRESSION(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  /// \\param x A expression
  /// \\param a An additional argument for the recursion
  /// \\return The visit result
  MYEXPRESSION visit(const MYEXPRESSION& x, Arg& a)
  {
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit>" << pp(x) << std::endl;
#endif
    MYEXPRESSION result;
%s
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit result>" << pp(result) << std::endl;
#endif
    return result;
  }
};

/// \\brief Modifying visitor class for expressions.
///
/// If a visit_<node> function returns true, the recursion is continued
/// in the children of this node, otherwise not.
template <>
struct MYEXPRESSION_builder<void>
{
  /// \\brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \\brief Returns true if the expression is not equal to MYEXPRESSION().
  /// This is used to determine if the recursion in a node needs to be continued.
  /// \\param x A expression
  /// \\return True if the term is not equal to MYEXPRESSION()
  bool is_finished(const MYEXPRESSION& x)
  {
    return x != MYEXPRESSION();
  }

  /// \\brief Destructor.
  virtual ~MYEXPRESSION_builder()
  { }
%s

  /// \\brief Visits the nodes of the expression and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  MYEXPRESSION visit(const MYEXPRESSION& x)
  {
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit>" << pp(x) << std::endl;
#endif
    MYEXPRESSION result;
%s
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit result>" << pp(result) << std::endl;
#endif
    return result;
  }
};
'''

EXPRESSION_VISITOR_NODE_TEXT = r'''
  /// \\brief Visit NODE node
  /// \\return The result of visiting the node
  virtual bool visit_NODE(const NODE& xEXTRA_ARG)
  {
    return continue_recursion;
  }

  /// \\brief Leave NODE node
  virtual void leave_NODE()
  {}
'''

EXPRESSION_BUILDER_NODE_TEXT = r'''              
  /// \\brief Visit NODE node
  /// \\return The result of visiting the node
  virtual MYEXPRESSION visit_NODE(const MYEXPRESSION& xARGUMENTSEXTRA_ARG)
  {
    return MYEXPRESSION();
  }
'''

def indent_text(text, indent):
    lines = []
    for line in string.split(text, '\n'):
        lines.append(indent + line)
    return string.join(lines, '\n')

def make_expression_visitor(filename, expression, text):
    classes = parse_classes(text)
    vtext = ''
    wtext = ''
    else_text = ''

    for c in classes:
        (aterm, constructor, description) = c
        f = FunctionDeclaration(constructor)

        node = f.name()
        types = [p.type() for p in f.parameters()]
        names = [p.name() for p in f.parameters()]
        arguments = f.argument_text()

        text = EXPRESSION_VISITOR_NODE_TEXT
        text = re.sub('NODE', node, text)
        vtext = vtext + text
    
        #--- generate code fragments like this
        #
        #    if (is_imp(e))
        #    {
        #      term_type l = left(e);
        #      term_type r = right(e);
        #      bool result = visit_imp(e, l, r);
        #      if (result) {
        #        visit(l);
        #        visit(r);
        #      }
        #      leave_imp();
        #    }
        text = ''
        text = text + '%sif (is_%s(x))\n' % (else_text, node)
        if else_text == '':
            else_text = 'else '
        text = text + '{\n'
        #for i in range(len(types)):
        #    text = text + '  %s %s = %s(x).%s();\n' % (types[i], names[i], node, names[i])
        has_children = expression in map(extract_type, types)
        args = ', '.join(names)
        if args != '':
            args = ', ' + args
        rtext = ''
        if has_children:
            rtext = 'bool result = '
        #text = text + '  %svisit_%s(x%sEXTRA_ARG);\n' % (rtext, node, args)
        text = text + '  %svisit_%s(%s(x)EXTRA_ARG);\n' % (rtext, node, node)
        if has_children:
            text = text + '  if (result) {\n'
            for i in range(len(types)):
                if extract_type(types[i]) == expression:
                    #text = text + '    visit(%sEXTRA_ARG);\n' % names[i]
                    text = text + '    visit(%s(x).%s()EXTRA_ARG);\n' % (node, names[i])
            text = text + '  }\n'
        text = text + '  leave_%s();\n' % node
        text = text + '}\n'
        wtext = wtext + text
    wtext = indent_text(wtext, '    ')
    
    vtext1 = re.sub('EXTRA_ARG', '', vtext)
    vtext2 = re.sub('EXTRA_ARG', ', Arg& /* a */', vtext)
    wtext1 = re.sub('EXTRA_ARG', '', wtext)
    wtext2 = re.sub('EXTRA_ARG', ', a', wtext)
    
    rtext = EXPRESSION_VISITOR_CODE % (vtext2, wtext2, vtext1, wtext1)
    rtext = re.sub('MYEXPRESSION', expression, rtext)
    text = path(filename).text()
    text = re.compile(r'//--- start generated visitor ---//.*//--- end generated visitor ---//', re.S).sub(
                  '//--- start generated visitor ---//\n' + rtext + '//--- end generated visitor ---//',
                  text)
    path(filename).write_text(text)

def make_expression_builder(filename, expression, text):
    classes = parse_classes(text)
    vtext = ''
    wtext = ''
    else_text = ''

    for c in classes:
        (aterm, constructor, description) = c
        f = FunctionDeclaration(constructor)

        node = f.name()
        types = [p.type() for p in f.parameters()]
        names = [p.name() for p in f.parameters()]
        arguments = f.argument_text()

        text = EXPRESSION_BUILDER_NODE_TEXT
        text = re.sub('MYEXPRESSION', expression, text)
        text = re.sub('NODE', node, text)
        args = arguments
        if args.strip() != '':
            args = ', ' + args
        text = re.sub('ARGUMENTS', args, text)
        vtext = vtext + text
    
        #--- generate code fragments like this
        #
        #    if (is_and(x))
        #    {
        #      process_expression l = left(x);
        #      process_expression r = right(x);
        #      result = visit_and(x, l, r, a);
        #      if (!is_finished(result)) {
        #        result = core::optimized_and(visit(l, a), visit(r, a));
        #      }
        #    }  
        text = ''
        text = text + '%sif (is_%s(x))\n' % (else_text, node)
        if else_text == '':
            else_text = 'else '
        text = text + '{\n'
        for i in range(len(types)):
            text = text + '  %s %s = %s(x).%s();\n' % (types[i], names[i], node, names[i])
        args = ', '.join(names)
        if args != '':
            args = ', ' + args
        text = text + '  result = visit_%s(x%sEXTRA_ARG);\n' % (node, args)
        text = text + '  if (!is_finished(result))\n'
        text = text + '  {\n'
        stext = ''
        for i in range(len(types)):
            if stext != '':
                stext = stext + ', '
            if types[i] == expression:
                stext = stext + 'visit(%sEXTRA_ARG)' % names[i]
            else:
                stext = stext + names[i]
        text = text + '    result = %s(%s);\n' % (node, stext)
        text = text + '  }\n'
        text = text + '}\n'
        wtext = wtext + text
    wtext = indent_text(wtext, '    ')
    
    vtext1 = re.sub('EXTRA_ARG', '', vtext)
    vtext2 = re.sub('EXTRA_ARG', ', Arg& /* a */', vtext)
    wtext1 = re.sub('EXTRA_ARG', '', wtext)
    wtext2 = re.sub('EXTRA_ARG', ', a', wtext)
    
    rtext = EXPRESSION_BUILDER_CODE % (vtext2, wtext2, vtext1, wtext1)
    rtext = re.sub('MYEXPRESSION', expression, rtext)
    text = path(filename).text()
    text = re.compile(r'//--- start generated visitor ---//.*//--- end generated visitor ---//', re.S).sub(
                  '//--- start generated visitor ---//\n' + rtext + '//--- end generated visitor ---//',
                  text)
    path(filename).write_text(text)

def make_is_functions(filename, text):
    TERM_TRAITS_TEXT = r'''
    /// \\brief Test for a %s expression
    /// \\param t A term
    /// \\return True if it is a %s expression
    inline
    bool is_%s(const process_expression& t)
    {
      return core::detail::gsIs%s(t);
    }
'''

    rtext = ''
    classes = parse_classes(text)
    for c in classes:
        (aterm, constructor, description) = c
        f = FunctionDeclaration(constructor)
        name = f.name()
        rtext = rtext + TERM_TRAITS_TEXT % (name, name, name, aterm)
    text = path(filename).text()
    text = re.compile(r'//--- start generated is-functions ---//.*//--- end generated is-functions ---//', re.S).sub(
                  '//--- start generated is-functions ---//\n' + rtext + '//--- end generated is-functions ---//',
                  text)
    path(filename).write_text(text)

make_expression_visitor('../../process/include/mcrl2/process/process_expression_visitor.h', 'process_expression', PROCESS_EXPRESSION_CLASSES)
make_expression_builder('../../process/include/mcrl2/process/process_expression_builder.h', 'process_expression', PROCESS_EXPRESSION_CLASSES)
make_is_functions(      '../../process/include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_CLASSES)

# N.B. This doesn't work, since the pbes expression visitors need to be patched for the value true
# make_expression_visitor('../../pbes/include/mcrl2/pbes/pbes_expression_visitor.h', 'pbes_expression', PBES_EXPRESSION_CLASSES)
# make_expression_builder('../../pbes/include/mcrl2/pbes/pbes_expression_builder.h', 'pbes_expression', PBES_EXPRESSION_CLASSES)
# make_is_functions('../../pbes/include/mcrl2/pbes/pbes_expression.h', PBES_EXPRESSION_CLASSES)

make_expression_classes('../../lps/include/mcrl2/modal_formula/state_formula.h', STATE_FORMULA_CLASSES, 'state_formula')
make_expression_classes('../../lps/include/mcrl2/modal_formula/action_formula.h', ACTION_FORMULA_CLASSES, 'action_formula')
make_expression_classes('../../process/include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_CLASSES, 'process_expression')
make_expression_classes('../../pbes/include/mcrl2/pbes/pbes_expression.h', PBES_EXPRESSION_CLASSES, 'pbes_expression')
