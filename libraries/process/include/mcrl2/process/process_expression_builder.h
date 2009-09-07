// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_expression_builder.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_EXPRESSION_BUILDER_H
#define MCRL2_PROCESS_PROCESS_EXPRESSION_BUILDER_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2 {

namespace process {

//--- start generated text ---//
/// \brief Modifying visitor class for expressions.
///
/// During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns process_expression(), the recursion is continued
/// in the children of this node, otherwise not.
/// An arbitrary additional argument may be passed during the recursion.
template <typename Arg = void>
struct process_expression_builder
{
  /// \brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \brief Returns true if the expression is not equal to process_expression().
  /// This is used to determine if the recursion in a node needs to be continued.
  /// \param x A expression
  /// \return True if the term is not equal to process_expression()
  bool is_finished(const process_expression& x)
  {
    return x != process_expression();
  }

  /// \brief Destructor.
  virtual ~process_expression_builder()
  { }
              
  /// \brief Visit action node
  /// \return The result of visiting the node
  virtual process_expression visit_process_action(const process_expression& x, const lps::action_label& l, const data::data_expression_list& v, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit process_instance node
  /// \return The result of visiting the node
  virtual process_expression visit_process_instance(const process_expression& x, const process_identifier pi, const data::data_expression_list& v, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit process_instance_assignment node
  /// \return The result of visiting the node
  virtual process_expression visit_process_instance_assignment(const process_expression& x, const process_identifier& pi, const data::assignment_list& v, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit delta node
  /// \return The result of visiting the node
  virtual process_expression visit_delta(const process_expression& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit tau node
  /// \return The result of visiting the node
  virtual process_expression visit_tau(const process_expression& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit sum node
  /// \return The result of visiting the node
  virtual process_expression visit_sum(const process_expression& x, const data::variable_list& v, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit block node
  /// \return The result of visiting the node
  virtual process_expression visit_block(const process_expression& x, const core::identifier_string_list& s, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit hide node
  /// \return The result of visiting the node
  virtual process_expression visit_hide(const process_expression& x, const core::identifier_string_list& s, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit rename node
  /// \return The result of visiting the node
  virtual process_expression visit_rename(const process_expression& x, const rename_expression_list& r, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit comm node
  /// \return The result of visiting the node
  virtual process_expression visit_comm(const process_expression& x, const communication_expression_list& c, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit allow node
  /// \return The result of visiting the node
  virtual process_expression visit_allow(const process_expression& x, const action_name_multiset_list& s, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit sync node
  /// \return The result of visiting the node
  virtual process_expression visit_sync(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit at node
  /// \return The result of visiting the node
  virtual process_expression visit_at(const process_expression& x, const process_expression& left, const data::data_expression& d, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit seq node
  /// \return The result of visiting the node
  virtual process_expression visit_seq(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit if_then node
  /// \return The result of visiting the node
  virtual process_expression visit_if_then(const process_expression& x, const data::data_expression& d, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  virtual process_expression visit_if_then_else(const process_expression& x, const data::data_expression& d, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit bounded_init node
  /// \return The result of visiting the node
  virtual process_expression visit_bounded_init(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit merge node
  /// \return The result of visiting the node
  virtual process_expression visit_merge(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit left_merge node
  /// \return The result of visiting the node
  virtual process_expression visit_left_merge(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit choice node
  /// \return The result of visiting the node
  virtual process_expression visit_choice(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return process_expression();
  }

  /// \brief Visits the nodes of the expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals process_expression(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  /// \param x A expression
  /// \param a An additional argument for the recursion
  /// \return The visit result
  process_expression visit(const process_expression& x, Arg& a)
  {
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit>" << pp(x) << std::endl;
#endif
    process_expression result;
    if (is_process_action(x))
    {
      lps::action_label l = action(x).label();
      data::data_expression_list v = action(x).arguments();
      result = visit_process_action(x, l, v, a);
      if (!is_finished(result))
      {
        result = action(l, v);
      }
    }
    else if (is_process_instance(x))
    {
      process_identifier pi = process_instance(x).identifier();
      data::data_expression_list v = process_instance(x).actual_parameters();
      result = visit_process_instance(x, pi, v, a);
      if (!is_finished(result))
      {
        result = process_instance(pi, v);
      }
    }
    else if (is_process_instance_assignment(x))
    {
      process_identifier pi = process_instance_assignment(x).identifier();
      data::assignment_list v = process_instance_assignment(x).assignments();
      result = visit_process_instance_assignment(x, pi, v, a);
      if (!is_finished(result))
      {
        result = process_instance_assignment(pi, v);
      }
    }
    else if (is_delta(x))
    {
      result = visit_delta(x, a);
      if (!is_finished(result))
      {
        result = delta();
      }
    }
    else if (is_tau(x))
    {
      result = visit_tau(x, a);
      if (!is_finished(result))
      {
        result = tau();
      }
    }
    else if (is_sum(x))
    {
      data::variable_list v = sum(x).bound_variables();
      process_expression right = sum(x).operand();
      result = visit_sum(x, v, right, a);
      if (!is_finished(result))
      {
        result = sum(v, visit(right, a));
      }
    }
    else if (is_block(x))
    {
      core::identifier_string_list s = block(x).block_set();
      process_expression right = block(x).operand();
      result = visit_block(x, s, right, a);
      if (!is_finished(result))
      {
        result = block(s, visit(right, a));
      }
    }
    else if (is_hide(x))
    {
      core::identifier_string_list s = hide(x).hide_set();
      process_expression right = hide(x).operand();
      result = visit_hide(x, s, right, a);
      if (!is_finished(result))
      {
        result = hide(s, visit(right, a));
      }
    }
    else if (is_rename(x))
    {
      rename_expression_list r = rename(x).rename_set();
      process_expression right = rename(x).operand();
      result = visit_rename(x, r, right, a);
      if (!is_finished(result))
      {
        result = rename(r, visit(right, a));
      }
    }
    else if (is_comm(x))
    {
      communication_expression_list c = comm(x).comm_set();
      process_expression right = comm(x).operand();
      result = visit_comm(x, c, right, a);
      if (!is_finished(result))
      {
        result = comm(c, visit(right, a));
      }
    }
    else if (is_allow(x))
    {
      action_name_multiset_list s = allow(x).allow_set();
      process_expression right = allow(x).operand();
      result = visit_allow(x, s, right, a);
      if (!is_finished(result))
      {
        result = allow(s, visit(right, a));
      }
    }
    else if (is_sync(x))
    {
      process_expression left = sync(x).left();
      process_expression right = sync(x).right();
      result = visit_sync(x, left, right, a);
      if (!is_finished(result))
      {
        result = sync(visit(left, a), visit(right, a));
      }
    }
    else if (is_at(x))
    {
      process_expression left = at(x).operand();
      data::data_expression d = at(x).time_stamp();
      result = visit_at(x, left, d, a);
      if (!is_finished(result))
      {
        result = at(visit(left, a), d);
      }
    }
    else if (is_seq(x))
    {
      process_expression left = seq(x).left();
      process_expression right = seq(x).right();
      result = visit_seq(x, left, right, a);
      if (!is_finished(result))
      {
        result = seq(visit(left, a), visit(right, a));
      }
    }
    else if (is_if_then(x))
    {
      data::data_expression d = if_then(x).condition();
      process_expression right = if_then(x).then_case();
      result = visit_if_then(x, d, right, a);
      if (!is_finished(result))
      {
        result = if_then(d, visit(right, a));
      }
    }
    else if (is_if_then_else(x))
    {
      data::data_expression d = if_then_else(x).condition();
      process_expression left = if_then_else(x).then_case();
      process_expression right = if_then_else(x).else_case();
      result = visit_if_then_else(x, d, left, right, a);
      if (!is_finished(result))
      {
        result = if_then_else(d, visit(left, a), visit(right, a));
      }
    }
    else if (is_bounded_init(x))
    {
      process_expression left = bounded_init(x).left();
      process_expression right = bounded_init(x).right();
      result = visit_bounded_init(x, left, right, a);
      if (!is_finished(result))
      {
        result = bounded_init(visit(left, a), visit(right, a));
      }
    }
    else if (is_merge(x))
    {
      process_expression left = merge(x).left();
      process_expression right = merge(x).right();
      result = visit_merge(x, left, right, a);
      if (!is_finished(result))
      {
        result = merge(visit(left, a), visit(right, a));
      }
    }
    else if (is_left_merge(x))
    {
      process_expression left = left_merge(x).left();
      process_expression right = left_merge(x).right();
      result = visit_left_merge(x, left, right, a);
      if (!is_finished(result))
      {
        result = left_merge(visit(left, a), visit(right, a));
      }
    }
    else if (is_choice(x))
    {
      process_expression left = choice(x).left();
      process_expression right = choice(x).right();
      result = visit_choice(x, left, right, a);
      if (!is_finished(result))
      {
        result = choice(visit(left, a), visit(right, a));
      }
    }
    
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit result>" << pp(result) << std::endl;
#endif
    return result;
  }
};

/// \brief Modifying visitor class for expressions.
///
/// If a visit_<node> function returns true, the recursion is continued
/// in the children of this node, otherwise not.
template <>
struct process_expression_builder<void>
{
  /// \brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \brief Returns true if the expression is not equal to process_expression().
  /// This is used to determine if the recursion in a node needs to be continued.
  /// \param x A expression
  /// \return True if the term is not equal to process_expression()
  bool is_finished(const process_expression& x)
  {
    return x != process_expression();
  }

  /// \brief Destructor.
  virtual ~process_expression_builder()
  { }
              
  /// \brief Visit action node
  /// \return The result of visiting the node
  virtual process_expression visit_process_action(const process_expression& x, const lps::action_label& l, const data::data_expression_list& v)
  {
    return process_expression();
  }
              
  /// \brief Visit process_instance node
  /// \return The result of visiting the node
  virtual process_expression visit_process_instance(const process_expression& x, const process_identifier pi, const data::data_expression_list& v)
  {
    return process_expression();
  }
              
  /// \brief Visit process_instance_assignment node
  /// \return The result of visiting the node
  virtual process_expression visit_process_instance_assignment(const process_expression& x, const process_identifier& pi, const data::assignment_list& v)
  {
    return process_expression();
  }
              
  /// \brief Visit delta node
  /// \return The result of visiting the node
  virtual process_expression visit_delta(const process_expression& x)
  {
    return process_expression();
  }
              
  /// \brief Visit tau node
  /// \return The result of visiting the node
  virtual process_expression visit_tau(const process_expression& x)
  {
    return process_expression();
  }
              
  /// \brief Visit sum node
  /// \return The result of visiting the node
  virtual process_expression visit_sum(const process_expression& x, const data::variable_list& v, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit block node
  /// \return The result of visiting the node
  virtual process_expression visit_block(const process_expression& x, const core::identifier_string_list& s, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit hide node
  /// \return The result of visiting the node
  virtual process_expression visit_hide(const process_expression& x, const core::identifier_string_list& s, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit rename node
  /// \return The result of visiting the node
  virtual process_expression visit_rename(const process_expression& x, const rename_expression_list& r, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit comm node
  /// \return The result of visiting the node
  virtual process_expression visit_comm(const process_expression& x, const communication_expression_list& c, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit allow node
  /// \return The result of visiting the node
  virtual process_expression visit_allow(const process_expression& x, const action_name_multiset_list& s, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit sync node
  /// \return The result of visiting the node
  virtual process_expression visit_sync(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit at node
  /// \return The result of visiting the node
  virtual process_expression visit_at(const process_expression& x, const process_expression& left, const data::data_expression& d)
  {
    return process_expression();
  }
              
  /// \brief Visit seq node
  /// \return The result of visiting the node
  virtual process_expression visit_seq(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit if_then node
  /// \return The result of visiting the node
  virtual process_expression visit_if_then(const process_expression& x, const data::data_expression& d, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  virtual process_expression visit_if_then_else(const process_expression& x, const data::data_expression& d, const process_expression& left, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit bounded_init node
  /// \return The result of visiting the node
  virtual process_expression visit_bounded_init(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit merge node
  /// \return The result of visiting the node
  virtual process_expression visit_merge(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit left_merge node
  /// \return The result of visiting the node
  virtual process_expression visit_left_merge(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return process_expression();
  }
              
  /// \brief Visit choice node
  /// \return The result of visiting the node
  virtual process_expression visit_choice(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return process_expression();
  }


  /// \brief Visits the nodes of the expression and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  process_expression visit(const process_expression& x)
  {
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit>" << pp(x) << std::endl;
#endif
    process_expression result;
    if (is_process_action(x))
    {
      lps::action_label l = action(x).label();
      data::data_expression_list v = action(x).arguments();
      result = visit_process_action(x, l, v);
      if (!is_finished(result))
      {
        result = action(l, v);
      }
    }
    else if (is_process_instance(x))
    {
      process_identifier pi = process_instance(x).identifier();
      data::data_expression_list v = process_instance(x).actual_parameters();
      result = visit_process_instance(x, pi, v);
      if (!is_finished(result))
      {
        result = process_instance(pi, v);
      }
    }
    else if (is_process_instance_assignment(x))
    {
      process_identifier pi = process_instance_assignment(x).identifier();
      data::assignment_list v = process_instance_assignment(x).assignments();
      result = visit_process_instance_assignment(x, pi, v);
      if (!is_finished(result))
      {
        result = process_instance_assignment(pi, v);
      }
    }
    else if (is_delta(x))
    {
      result = visit_delta(x);
      if (!is_finished(result))
      {
        result = delta();
      }
    }
    else if (is_tau(x))
    {
      result = visit_tau(x);
      if (!is_finished(result))
      {
        result = tau();
      }
    }
    else if (is_sum(x))
    {
      data::variable_list v = sum(x).bound_variables();
      process_expression right = sum(x).operand();
      result = visit_sum(x, v, right);
      if (!is_finished(result))
      {
        result = sum(v, visit(right));
      }
    }
    else if (is_block(x))
    {
      core::identifier_string_list s = block(x).block_set();
      process_expression right = block(x).operand();
      result = visit_block(x, s, right);
      if (!is_finished(result))
      {
        result = block(s, visit(right));
      }
    }
    else if (is_hide(x))
    {
      core::identifier_string_list s = hide(x).hide_set();
      process_expression right = hide(x).operand();
      result = visit_hide(x, s, right);
      if (!is_finished(result))
      {
        result = hide(s, visit(right));
      }
    }
    else if (is_rename(x))
    {
      rename_expression_list r = rename(x).rename_set();
      process_expression right = rename(x).operand();
      result = visit_rename(x, r, right);
      if (!is_finished(result))
      {
        result = rename(r, visit(right));
      }
    }
    else if (is_comm(x))
    {
      communication_expression_list c = comm(x).comm_set();
      process_expression right = comm(x).operand();
      result = visit_comm(x, c, right);
      if (!is_finished(result))
      {
        result = comm(c, visit(right));
      }
    }
    else if (is_allow(x))
    {
      action_name_multiset_list s = allow(x).allow_set();
      process_expression right = allow(x).operand();
      result = visit_allow(x, s, right);
      if (!is_finished(result))
      {
        result = allow(s, visit(right));
      }
    }
    else if (is_sync(x))
    {
      process_expression left = sync(x).left();
      process_expression right = sync(x).right();
      result = visit_sync(x, left, right);
      if (!is_finished(result))
      {
        result = sync(visit(left), visit(right));
      }
    }
    else if (is_at(x))
    {
      process_expression left = at(x).operand();
      data::data_expression d = at(x).time_stamp();
      result = visit_at(x, left, d);
      if (!is_finished(result))
      {
        result = at(visit(left), d);
      }
    }
    else if (is_seq(x))
    {
      process_expression left = seq(x).left();
      process_expression right = seq(x).right();
      result = visit_seq(x, left, right);
      if (!is_finished(result))
      {
        result = seq(visit(left), visit(right));
      }
    }
    else if (is_if_then(x))
    {
      data::data_expression d = if_then(x).condition();
      process_expression right = if_then(x).then_case();
      result = visit_if_then(x, d, right);
      if (!is_finished(result))
      {
        result = if_then(d, visit(right));
      }
    }
    else if (is_if_then_else(x))
    {
      data::data_expression d = if_then_else(x).condition();
      process_expression left = if_then_else(x).then_case();
      process_expression right = if_then_else(x).else_case();
      result = visit_if_then_else(x, d, left, right);
      if (!is_finished(result))
      {
        result = if_then_else(d, visit(left), visit(right));
      }
    }
    else if (is_bounded_init(x))
    {
      process_expression left = bounded_init(x).left();
      process_expression right = bounded_init(x).right();
      result = visit_bounded_init(x, left, right);
      if (!is_finished(result))
      {
        result = bounded_init(visit(left), visit(right));
      }
    }
    else if (is_merge(x))
    {
      process_expression left = merge(x).left();
      process_expression right = merge(x).right();
      result = visit_merge(x, left, right);
      if (!is_finished(result))
      {
        result = merge(visit(left), visit(right));
      }
    }
    else if (is_left_merge(x))
    {
      process_expression left = left_merge(x).left();
      process_expression right = left_merge(x).right();
      result = visit_left_merge(x, left, right);
      if (!is_finished(result))
      {
        result = left_merge(visit(left), visit(right));
      }
    }
    else if (is_choice(x))
    {
      process_expression left = choice(x).left();
      process_expression right = choice(x).right();
      result = visit_choice(x, left, right);
      if (!is_finished(result))
      {
        result = choice(visit(left), visit(right));
      }
    }
    
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit result>" << pp(result) << std::endl;
#endif
    return result;
  }
};
//--- end generated text ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_BUILDER_H
