import re
import string
from path import *

PROCESS_EXPRESSION_TEXT = '''action(const lps::action_label& l, const data::data_expression_list& v) (label, arguments)
process_instance(const process_identifier pi, const data::data_expression_list& v) (identifier, actual_parameters)
process_instance_assignment(const process_identifier& pi, const data::assignment_list& v) (identifier, assignments)
delta() ()
tau() ()
sum(const data::variable_list& v, const process_expression& right) (bound_variables, operand)
block(const core::identifier_string_list& s, const process_expression& right) (block_set, operand)
hide(const core::identifier_string_list& s, const process_expression& right) (hide_set, operand)
rename(const rename_expression_list& r, const process_expression& right) (rename_set, operand)
comm(const communication_expression_list& c, const process_expression& right) (comm_set, operand)
allow(const action_name_multiset_list& s, const process_expression& right) (allow_set, operand)
sync(const process_expression& left, const process_expression& right) (left, right)
at(const process_expression& left, const data::data_expression& d) (operand, time_stamp)
seq(const process_expression& left, const process_expression& right) (left, right)
if_then(const data::data_expression& d, const process_expression& right) (condition, then_case)
if_then_else(const data::data_expression& d, const process_expression& left, const process_expression& right) (condition, then_case, else_case)
bounded_init(const process_expression& left, const process_expression& right) (left, right)
merge(const process_expression& left, const process_expression& right) (left, right)
left_merge(const process_expression& left, const process_expression& right) (left, right)
choice(const process_expression& left, const process_expression& right) (left, right)'''

PROCESS_EXPRESSION_TERMS = '''
  action                      Action           
  process_instance            Process          
  process_instance_assignment ProcessAssignment
  delta                       Delta            
  tau                         Tau              
  sum                         Sum              
  block                       Block            
  hide                        Hide             
  rename                      Rename           
  comm                        Comm             
  allow                       Allow            
  sync                        Sync             
  at                          AtTime           
  seq                         Seq              
  if_then                     IfThen           
  if_then_else                IfThenElse       
  bounded_init                BInit            
  merge                       Merge            
  left_merge                  LMerge           
  choice                      Choice           
'''

EXPRESSION_VISITOR_CODE = r'''/// \\brief Visitor class for expressions.
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
  virtual bool visit_NODE(const MYEXPRESSION& xARGUMENTSEXTRA_ARG)
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

def split_arguments(arguments):
    if arguments.strip() == '':
        return ([], [])
    arguments = re.sub('const ', '', arguments)
    arguments = re.sub('&', '', arguments)
    words = map(string.strip, arguments.split(','))
    variables = []
    names = []
    for word in words:
        w = word.split(' ')
        (v, n) = w
        variables.append(v)
        names.append(n)
    return (variables, names)

def make_expression_visitor(filename, expression, expression_text):
    lines = expression_text.split('\n')
    vtext = ''
    wtext = ''
    else_text = ''
    for line in lines:
        words = map(string.strip, re.split('\(', line))
        node = words[0]
        arguments = words[1][:-1]
        accessors = re.split(r',\s*', words[2][:-1])
    
        text = EXPRESSION_VISITOR_NODE_TEXT

        text = re.sub('MYEXPRESSION', expression, text)
        text = re.sub('NODE', node, text)
        args = arguments
        if args.strip() != '':
            args = ', ' + args
        text = re.sub('ARGUMENTS', args, text)
        vtext = vtext + text
    
        (types, names) = split_arguments(arguments)
    
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
        for i in range(len(types)):
            text = text + '  %s %s = %s(x).%s();\n' % (types[i], names[i], node, accessors[i])
        has_children = re.search('process_expression', line) != None
        args = ', '.join(names)
        if args != '':
            args = ', ' + args
        rtext = ''
        if has_children:
            rtext = 'bool result = '
        text = text + '  %svisit_%s(x%sEXTRA_ARG);\n' % (rtext, node, args)
        if has_children:
            text = text + '  if (result) {\n'
            for i in range(len(types)):
                if types[i] == 'process_expression':
                    text = text + '    visit(%sEXTRA_ARG);\n' % names[i]
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
    text = re.compile(r'//--- start generated text ---//.*//--- end generated text ---//', re.S).sub(
                  '//--- start generated text ---//\n' + rtext + '//--- end generated text ---//',
                  text)
    path(filename).write_text(text)

def make_expression_builder(filename, expression):
    lines = PROCESS_EXPRESSION_TEXT.split('\n')
    vtext = ''
    wtext = ''
    else_text = ''
    for line in lines:
        words = map(string.strip, re.split('\(', line))
        node = words[0]
        arguments = words[1][:-1]
        accessors = re.split(r',\s*', words[2][:-1])
    
        text = EXPRESSION_BUILDER_NODE_TEXT

        text = re.sub('MYEXPRESSION', expression, text)
        text = re.sub('NODE', node, text)
        args = arguments
        if args.strip() != '':
            args = ', ' + args
        text = re.sub('ARGUMENTS', args, text)
        vtext = vtext + text
    
        (types, names) = split_arguments(arguments)
    
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
            text = text + '  %s %s = %s(x).%s();\n' % (types[i], names[i], node, accessors[i])
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
            if types[i] == 'process_expression':
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
    text = re.compile(r'//--- start generated text ---//.*//--- end generated text ---//', re.S).sub(
                  '//--- start generated text ---//\n' + rtext + '//--- end generated text ---//',
                  text)
    path(filename).write_text(text)

def make_is_functions(filename, term_text):
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
    terms = term_text.rsplit('\n')
    for t in terms:
        words = t.split()
        if len(words) != 2:
            continue
        rtext = rtext + TERM_TRAITS_TEXT % (words[0], words[0], words[0], words[1])
    text = path(filename).text()
    text = re.compile(r'//--- start generated text ---//.*//--- end generated text ---//', re.S).sub(
                  '//--- start generated text ---//\n' + rtext + '//--- end generated text ---//',
                  text)
    path(filename).write_text(text)

make_expression_visitor('../include/mcrl2/process/process_expression_visitor.h', 'process_expression', PROCESS_EXPRESSION_TEXT)
make_expression_builder('../include/mcrl2/process/process_expression_builder.h', 'process_expression')
make_is_functions('../include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_TERMS)
