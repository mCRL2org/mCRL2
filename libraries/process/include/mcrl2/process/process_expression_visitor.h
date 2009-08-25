// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_expression_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_EXPRESSION_VISITOR_H
#define MCRL2_PROCESS_PROCESS_EXPRESSION_VISITOR_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2 {

namespace process {

//--- start generated text ---//
/// \brief Visitor class for expressions.
///
/// There is a visit_<node> and a leave_<node>
/// function for each type of node. By default these functions do nothing, so they
/// must be overridden to add behavior. If the visit_<node> function returns true,
/// the recursion is continued in the children of the node.
template <typename Arg=void>
struct process_expression_visitor
{
  /// \brief The type of the additional argument for the recursion
  typedef Arg argument_type;

  /// \brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \brief Destructor.
  virtual ~process_expression_visitor()
  { }

  /// \brief Visit action node
  /// \return The result of visiting the node
  virtual bool visit_action(const process_expression& x, const lps::action_label& l, const data::data_expression_list& v, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave action node
  virtual void leave_action()
  {}

  /// \brief Visit process_instance node
  /// \return The result of visiting the node
  virtual bool visit_process_instance(const process_expression& x, const process_identifier pi, const data::data_expression_list& v, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave process_instance node
  virtual void leave_process_instance()
  {}

  /// \brief Visit process_instance_assignment node
  /// \return The result of visiting the node
  virtual bool visit_process_instance_assignment(const process_expression& x, const process_identifier& pi, const data::assignment_list& v, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave process_instance_assignment node
  virtual void leave_process_instance_assignment()
  {}

  /// \brief Visit delta node
  /// \return The result of visiting the node
  virtual bool visit_delta(const process_expression& x, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave delta node
  virtual void leave_delta()
  {}

  /// \brief Visit tau node
  /// \return The result of visiting the node
  virtual bool visit_tau(const process_expression& x, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave tau node
  virtual void leave_tau()
  {}

  /// \brief Visit sum node
  /// \return The result of visiting the node
  virtual bool visit_sum(const process_expression& x, const data::variable_list& v, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave sum node
  virtual void leave_sum()
  {}

  /// \brief Visit block node
  /// \return The result of visiting the node
  virtual bool visit_block(const process_expression& x, const core::identifier_string_list& s, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave block node
  virtual void leave_block()
  {}

  /// \brief Visit hide node
  /// \return The result of visiting the node
  virtual bool visit_hide(const process_expression& x, const core::identifier_string_list& s, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave hide node
  virtual void leave_hide()
  {}

  /// \brief Visit rename node
  /// \return The result of visiting the node
  virtual bool visit_rename(const process_expression& x, const rename_expression_list& r, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave rename node
  virtual void leave_rename()
  {}

  /// \brief Visit comm node
  /// \return The result of visiting the node
  virtual bool visit_comm(const process_expression& x, const communication_expression_list& c, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave comm node
  virtual void leave_comm()
  {}

  /// \brief Visit allow node
  /// \return The result of visiting the node
  virtual bool visit_allow(const process_expression& x, const action_name_multiset_list& s, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave allow node
  virtual void leave_allow()
  {}

  /// \brief Visit sync node
  /// \return The result of visiting the node
  virtual bool visit_sync(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave sync node
  virtual void leave_sync()
  {}

  /// \brief Visit at node
  /// \return The result of visiting the node
  virtual bool visit_at(const process_expression& x, const process_expression& left, const data::data_expression& d, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave at node
  virtual void leave_at()
  {}

  /// \brief Visit seq node
  /// \return The result of visiting the node
  virtual bool visit_seq(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave seq node
  virtual void leave_seq()
  {}

  /// \brief Visit if_then node
  /// \return The result of visiting the node
  virtual bool visit_if_then(const process_expression& x, const data::data_expression& d, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave if_then node
  virtual void leave_if_then()
  {}

  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  virtual bool visit_if_then_else(const process_expression& x, const data::data_expression& d, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave if_then_else node
  virtual void leave_if_then_else()
  {}

  /// \brief Visit bounded_init node
  /// \return The result of visiting the node
  virtual bool visit_bounded_init(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave bounded_init node
  virtual void leave_bounded_init()
  {}

  /// \brief Visit merge node
  /// \return The result of visiting the node
  virtual bool visit_merge(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave merge node
  virtual void leave_merge()
  {}

  /// \brief Visit left_merge node
  /// \return The result of visiting the node
  virtual bool visit_left_merge(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave left_merge node
  virtual void leave_left_merge()
  {}

  /// \brief Visit choice node
  /// \return The result of visiting the node
  virtual bool visit_choice(const process_expression& x, const process_expression& left, const process_expression& right, Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave choice node
  virtual void leave_choice()
  {}

  /// \brief Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  /// \param a An additional argument for the recursion
  void visit(const process_expression& x, Arg& a)
  {
    if (is_action(x))
    {
      lps::action_label l = action(x).label();
      data::data_expression_list v = action(x).arguments();
      visit_action(x, l, v, a);
      leave_action();
    }
    else if (is_process_instance(x))
    {
      process_identifier pi = process_instance(x).identifier();
      data::data_expression_list v = process_instance(x).actual_parameters();
      visit_process_instance(x, pi, v, a);
      leave_process_instance();
    }
    else if (is_process_instance_assignment(x))
    {
      process_identifier pi = process_instance_assignment(x).identifier();
      data::assignment_list v = process_instance_assignment(x).assignments();
      visit_process_instance_assignment(x, pi, v, a);
      leave_process_instance_assignment();
    }
    else if (is_delta(x))
    {
      visit_delta(x, a);
      leave_delta();
    }
    else if (is_tau(x))
    {
      visit_tau(x, a);
      leave_tau();
    }
    else if (is_sum(x))
    {
      data::variable_list v = sum(x).bound_variables();
      process_expression right = sum(x).operand();
      bool result = visit_sum(x, v, right, a);
      if (result) {
        visit(right, a);
      }
      leave_sum();
    }
    else if (is_block(x))
    {
      core::identifier_string_list s = block(x).block_set();
      process_expression right = block(x).operand();
      bool result = visit_block(x, s, right, a);
      if (result) {
        visit(right, a);
      }
      leave_block();
    }
    else if (is_hide(x))
    {
      core::identifier_string_list s = hide(x).hide_set();
      process_expression right = hide(x).operand();
      bool result = visit_hide(x, s, right, a);
      if (result) {
        visit(right, a);
      }
      leave_hide();
    }
    else if (is_rename(x))
    {
      rename_expression_list r = rename(x).rename_set();
      process_expression right = rename(x).operand();
      bool result = visit_rename(x, r, right, a);
      if (result) {
        visit(right, a);
      }
      leave_rename();
    }
    else if (is_comm(x))
    {
      communication_expression_list c = comm(x).comm_set();
      process_expression right = comm(x).operand();
      bool result = visit_comm(x, c, right, a);
      if (result) {
        visit(right, a);
      }
      leave_comm();
    }
    else if (is_allow(x))
    {
      action_name_multiset_list s = allow(x).allow_set();
      process_expression right = allow(x).operand();
      bool result = visit_allow(x, s, right, a);
      if (result) {
        visit(right, a);
      }
      leave_allow();
    }
    else if (is_sync(x))
    {
      process_expression left = sync(x).left();
      process_expression right = sync(x).right();
      bool result = visit_sync(x, left, right, a);
      if (result) {
        visit(left, a);
        visit(right, a);
      }
      leave_sync();
    }
    else if (is_at(x))
    {
      process_expression left = at(x).operand();
      data::data_expression d = at(x).time_stamp();
      bool result = visit_at(x, left, d, a);
      if (result) {
        visit(left, a);
      }
      leave_at();
    }
    else if (is_seq(x))
    {
      process_expression left = seq(x).left();
      process_expression right = seq(x).right();
      bool result = visit_seq(x, left, right, a);
      if (result) {
        visit(left, a);
        visit(right, a);
      }
      leave_seq();
    }
    else if (is_if_then(x))
    {
      data::data_expression d = if_then(x).condition();
      process_expression right = if_then(x).then_case();
      bool result = visit_if_then(x, d, right, a);
      if (result) {
        visit(right, a);
      }
      leave_if_then();
    }
    else if (is_if_then_else(x))
    {
      data::data_expression d = if_then_else(x).condition();
      process_expression left = if_then_else(x).then_case();
      process_expression right = if_then_else(x).else_case();
      bool result = visit_if_then_else(x, d, left, right, a);
      if (result) {
        visit(left, a);
        visit(right, a);
      }
      leave_if_then_else();
    }
    else if (is_bounded_init(x))
    {
      process_expression left = bounded_init(x).left();
      process_expression right = bounded_init(x).right();
      bool result = visit_bounded_init(x, left, right, a);
      if (result) {
        visit(left, a);
        visit(right, a);
      }
      leave_bounded_init();
    }
    else if (is_merge(x))
    {
      process_expression left = merge(x).left();
      process_expression right = merge(x).right();
      bool result = visit_merge(x, left, right, a);
      if (result) {
        visit(left, a);
        visit(right, a);
      }
      leave_merge();
    }
    else if (is_left_merge(x))
    {
      process_expression left = left_merge(x).left();
      process_expression right = left_merge(x).right();
      bool result = visit_left_merge(x, left, right, a);
      if (result) {
        visit(left, a);
        visit(right, a);
      }
      leave_left_merge();
    }
    else if (is_choice(x))
    {
      process_expression left = choice(x).left();
      process_expression right = choice(x).right();
      bool result = visit_choice(x, left, right, a);
      if (result) {
        visit(left, a);
        visit(right, a);
      }
      leave_choice();
    }
    
  }
};

/// \brief Visitor class for expressions.
///
/// If a visit_<node> function returns true, the recursion is continued
/// in the children of this node, otherwise not.
template <>
struct process_expression_visitor<void>
{
  /// \brief The type of the additional argument for the recursion
  typedef void argument_type;

  /// \brief These names can be used as return types of the visit functions, to make
  /// the code more readible.
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };

  /// \brief Destructor.
  virtual ~process_expression_visitor()
  { }

  /// \brief Visit action node
  /// \return The result of visiting the node
  virtual bool visit_action(const process_expression& x, const lps::action_label& l, const data::data_expression_list& v)
  {
    return continue_recursion;
  }

  /// \brief Leave action node
  virtual void leave_action()
  {}

  /// \brief Visit process_instance node
  /// \return The result of visiting the node
  virtual bool visit_process_instance(const process_expression& x, const process_identifier pi, const data::data_expression_list& v)
  {
    return continue_recursion;
  }

  /// \brief Leave process_instance node
  virtual void leave_process_instance()
  {}

  /// \brief Visit process_instance_assignment node
  /// \return The result of visiting the node
  virtual bool visit_process_instance_assignment(const process_expression& x, const process_identifier& pi, const data::assignment_list& v)
  {
    return continue_recursion;
  }

  /// \brief Leave process_instance_assignment node
  virtual void leave_process_instance_assignment()
  {}

  /// \brief Visit delta node
  /// \return The result of visiting the node
  virtual bool visit_delta(const process_expression& x)
  {
    return continue_recursion;
  }

  /// \brief Leave delta node
  virtual void leave_delta()
  {}

  /// \brief Visit tau node
  /// \return The result of visiting the node
  virtual bool visit_tau(const process_expression& x)
  {
    return continue_recursion;
  }

  /// \brief Leave tau node
  virtual void leave_tau()
  {}

  /// \brief Visit sum node
  /// \return The result of visiting the node
  virtual bool visit_sum(const process_expression& x, const data::variable_list& v, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave sum node
  virtual void leave_sum()
  {}

  /// \brief Visit block node
  /// \return The result of visiting the node
  virtual bool visit_block(const process_expression& x, const core::identifier_string_list& s, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave block node
  virtual void leave_block()
  {}

  /// \brief Visit hide node
  /// \return The result of visiting the node
  virtual bool visit_hide(const process_expression& x, const core::identifier_string_list& s, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave hide node
  virtual void leave_hide()
  {}

  /// \brief Visit rename node
  /// \return The result of visiting the node
  virtual bool visit_rename(const process_expression& x, const rename_expression_list& r, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave rename node
  virtual void leave_rename()
  {}

  /// \brief Visit comm node
  /// \return The result of visiting the node
  virtual bool visit_comm(const process_expression& x, const communication_expression_list& c, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave comm node
  virtual void leave_comm()
  {}

  /// \brief Visit allow node
  /// \return The result of visiting the node
  virtual bool visit_allow(const process_expression& x, const action_name_multiset_list& s, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave allow node
  virtual void leave_allow()
  {}

  /// \brief Visit sync node
  /// \return The result of visiting the node
  virtual bool visit_sync(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave sync node
  virtual void leave_sync()
  {}

  /// \brief Visit at node
  /// \return The result of visiting the node
  virtual bool visit_at(const process_expression& x, const process_expression& left, const data::data_expression& d)
  {
    return continue_recursion;
  }

  /// \brief Leave at node
  virtual void leave_at()
  {}

  /// \brief Visit seq node
  /// \return The result of visiting the node
  virtual bool visit_seq(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave seq node
  virtual void leave_seq()
  {}

  /// \brief Visit if_then node
  /// \return The result of visiting the node
  virtual bool visit_if_then(const process_expression& x, const data::data_expression& d, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave if_then node
  virtual void leave_if_then()
  {}

  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  virtual bool visit_if_then_else(const process_expression& x, const data::data_expression& d, const process_expression& left, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave if_then_else node
  virtual void leave_if_then_else()
  {}

  /// \brief Visit bounded_init node
  /// \return The result of visiting the node
  virtual bool visit_bounded_init(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave bounded_init node
  virtual void leave_bounded_init()
  {}

  /// \brief Visit merge node
  /// \return The result of visiting the node
  virtual bool visit_merge(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave merge node
  virtual void leave_merge()
  {}

  /// \brief Visit left_merge node
  /// \return The result of visiting the node
  virtual bool visit_left_merge(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave left_merge node
  virtual void leave_left_merge()
  {}

  /// \brief Visit choice node
  /// \return The result of visiting the node
  virtual bool visit_choice(const process_expression& x, const process_expression& left, const process_expression& right)
  {
    return continue_recursion;
  }

  /// \brief Leave choice node
  virtual void leave_choice()
  {}


  /// \brief Visits the nodes of the expression and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  /// \param x A term
  void visit(const process_expression& x)
  {
    if (is_action(x))
    {
      lps::action_label l = action(x).label();
      data::data_expression_list v = action(x).arguments();
      visit_action(x, l, v);
      leave_action();
    }
    else if (is_process_instance(x))
    {
      process_identifier pi = process_instance(x).identifier();
      data::data_expression_list v = process_instance(x).actual_parameters();
      visit_process_instance(x, pi, v);
      leave_process_instance();
    }
    else if (is_process_instance_assignment(x))
    {
      process_identifier pi = process_instance_assignment(x).identifier();
      data::assignment_list v = process_instance_assignment(x).assignments();
      visit_process_instance_assignment(x, pi, v);
      leave_process_instance_assignment();
    }
    else if (is_delta(x))
    {
      visit_delta(x);
      leave_delta();
    }
    else if (is_tau(x))
    {
      visit_tau(x);
      leave_tau();
    }
    else if (is_sum(x))
    {
      data::variable_list v = sum(x).bound_variables();
      process_expression right = sum(x).operand();
      bool result = visit_sum(x, v, right);
      if (result) {
        visit(right);
      }
      leave_sum();
    }
    else if (is_block(x))
    {
      core::identifier_string_list s = block(x).block_set();
      process_expression right = block(x).operand();
      bool result = visit_block(x, s, right);
      if (result) {
        visit(right);
      }
      leave_block();
    }
    else if (is_hide(x))
    {
      core::identifier_string_list s = hide(x).hide_set();
      process_expression right = hide(x).operand();
      bool result = visit_hide(x, s, right);
      if (result) {
        visit(right);
      }
      leave_hide();
    }
    else if (is_rename(x))
    {
      rename_expression_list r = rename(x).rename_set();
      process_expression right = rename(x).operand();
      bool result = visit_rename(x, r, right);
      if (result) {
        visit(right);
      }
      leave_rename();
    }
    else if (is_comm(x))
    {
      communication_expression_list c = comm(x).comm_set();
      process_expression right = comm(x).operand();
      bool result = visit_comm(x, c, right);
      if (result) {
        visit(right);
      }
      leave_comm();
    }
    else if (is_allow(x))
    {
      action_name_multiset_list s = allow(x).allow_set();
      process_expression right = allow(x).operand();
      bool result = visit_allow(x, s, right);
      if (result) {
        visit(right);
      }
      leave_allow();
    }
    else if (is_sync(x))
    {
      process_expression left = sync(x).left();
      process_expression right = sync(x).right();
      bool result = visit_sync(x, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_sync();
    }
    else if (is_at(x))
    {
      process_expression left = at(x).operand();
      data::data_expression d = at(x).time_stamp();
      bool result = visit_at(x, left, d);
      if (result) {
        visit(left);
      }
      leave_at();
    }
    else if (is_seq(x))
    {
      process_expression left = seq(x).left();
      process_expression right = seq(x).right();
      bool result = visit_seq(x, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_seq();
    }
    else if (is_if_then(x))
    {
      data::data_expression d = if_then(x).condition();
      process_expression right = if_then(x).then_case();
      bool result = visit_if_then(x, d, right);
      if (result) {
        visit(right);
      }
      leave_if_then();
    }
    else if (is_if_then_else(x))
    {
      data::data_expression d = if_then_else(x).condition();
      process_expression left = if_then_else(x).then_case();
      process_expression right = if_then_else(x).else_case();
      bool result = visit_if_then_else(x, d, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_if_then_else();
    }
    else if (is_bounded_init(x))
    {
      process_expression left = bounded_init(x).left();
      process_expression right = bounded_init(x).right();
      bool result = visit_bounded_init(x, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_bounded_init();
    }
    else if (is_merge(x))
    {
      process_expression left = merge(x).left();
      process_expression right = merge(x).right();
      bool result = visit_merge(x, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_merge();
    }
    else if (is_left_merge(x))
    {
      process_expression left = left_merge(x).left();
      process_expression right = left_merge(x).right();
      bool result = visit_left_merge(x, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_left_merge();
    }
    else if (is_choice(x))
    {
      process_expression left = choice(x).left();
      process_expression right = choice(x).right();
      bool result = visit_choice(x, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_choice();
    }
    
  }
};
//--- end generated text ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_VISITOR_H
