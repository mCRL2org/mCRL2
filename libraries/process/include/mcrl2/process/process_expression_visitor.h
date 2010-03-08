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

//--- start generated visitor ---//
/// \brief Visitor class for process_expressions.
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
  virtual bool visit_action(const lps::action& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave action node
  virtual void leave_action()
  {}

  /// \brief Visit process_instance node
  /// \return The result of visiting the node
  virtual bool visit_process_instance(const process_instance& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave process_instance node
  virtual void leave_process_instance()
  {}

  /// \brief Visit process_instance_assignment node
  /// \return The result of visiting the node
  virtual bool visit_process_instance_assignment(const process_instance_assignment& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave process_instance_assignment node
  virtual void leave_process_instance_assignment()
  {}

  /// \brief Visit delta node
  /// \return The result of visiting the node
  virtual bool visit_delta(const delta& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave delta node
  virtual void leave_delta()
  {}

  /// \brief Visit tau node
  /// \return The result of visiting the node
  virtual bool visit_tau(const tau& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave tau node
  virtual void leave_tau()
  {}

  /// \brief Visit sum node
  /// \return The result of visiting the node
  virtual bool visit_sum(const sum& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave sum node
  virtual void leave_sum()
  {}

  /// \brief Visit block node
  /// \return The result of visiting the node
  virtual bool visit_block(const block& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave block node
  virtual void leave_block()
  {}

  /// \brief Visit hide node
  /// \return The result of visiting the node
  virtual bool visit_hide(const hide& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave hide node
  virtual void leave_hide()
  {}

  /// \brief Visit rename node
  /// \return The result of visiting the node
  virtual bool visit_rename(const rename& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave rename node
  virtual void leave_rename()
  {}

  /// \brief Visit comm node
  /// \return The result of visiting the node
  virtual bool visit_comm(const comm& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave comm node
  virtual void leave_comm()
  {}

  /// \brief Visit allow node
  /// \return The result of visiting the node
  virtual bool visit_allow(const allow& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave allow node
  virtual void leave_allow()
  {}

  /// \brief Visit sync node
  /// \return The result of visiting the node
  virtual bool visit_sync(const sync& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave sync node
  virtual void leave_sync()
  {}

  /// \brief Visit at node
  /// \return The result of visiting the node
  virtual bool visit_at(const at& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave at node
  virtual void leave_at()
  {}

  /// \brief Visit seq node
  /// \return The result of visiting the node
  virtual bool visit_seq(const seq& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave seq node
  virtual void leave_seq()
  {}

  /// \brief Visit if_then node
  /// \return The result of visiting the node
  virtual bool visit_if_then(const if_then& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave if_then node
  virtual void leave_if_then()
  {}

  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  virtual bool visit_if_then_else(const if_then_else& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave if_then_else node
  virtual void leave_if_then_else()
  {}

  /// \brief Visit bounded_init node
  /// \return The result of visiting the node
  virtual bool visit_bounded_init(const bounded_init& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave bounded_init node
  virtual void leave_bounded_init()
  {}

  /// \brief Visit merge node
  /// \return The result of visiting the node
  virtual bool visit_merge(const merge& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave merge node
  virtual void leave_merge()
  {}

  /// \brief Visit left_merge node
  /// \return The result of visiting the node
  virtual bool visit_left_merge(const left_merge& /* x */ , Arg& /* a */)
  {
    return continue_recursion;
  }

  /// \brief Leave left_merge node
  virtual void leave_left_merge()
  {}

  /// \brief Visit choice node
  /// \return The result of visiting the node
  virtual bool visit_choice(const choice& /* x */ , Arg& /* a */)
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
      visit_action(action(x), a);
      leave_action();
    }
    else if (is_process_instance(x))
    {
      visit_process_instance(process_instance(x), a);
      leave_process_instance();
    }
    else if (is_process_instance_assignment(x))
    {
      visit_process_instance_assignment(process_instance_assignment(x), a);
      leave_process_instance_assignment();
    }
    else if (is_delta(x))
    {
      visit_delta(delta(x), a);
      leave_delta();
    }
    else if (is_tau(x))
    {
      visit_tau(tau(x), a);
      leave_tau();
    }
    else if (is_sum(x))
    {
      bool result = visit_sum(sum(x), a);
      if (result) {
        visit(sum(x).operand(), a);
      }
      leave_sum();
    }
    else if (is_block(x))
    {
      bool result = visit_block(block(x), a);
      if (result) {
        visit(block(x).operand(), a);
      }
      leave_block();
    }
    else if (is_hide(x))
    {
      bool result = visit_hide(hide(x), a);
      if (result) {
        visit(hide(x).operand(), a);
      }
      leave_hide();
    }
    else if (is_rename(x))
    {
      bool result = visit_rename(rename(x), a);
      if (result) {
        visit(rename(x).operand(), a);
      }
      leave_rename();
    }
    else if (is_comm(x))
    {
      bool result = visit_comm(comm(x), a);
      if (result) {
        visit(comm(x).operand(), a);
      }
      leave_comm();
    }
    else if (is_allow(x))
    {
      bool result = visit_allow(allow(x), a);
      if (result) {
        visit(allow(x).operand(), a);
      }
      leave_allow();
    }
    else if (is_sync(x))
    {
      bool result = visit_sync(sync(x), a);
      if (result) {
        visit(sync(x).left(), a);
        visit(sync(x).right(), a);
      }
      leave_sync();
    }
    else if (is_at(x))
    {
      bool result = visit_at(at(x), a);
      if (result) {
        visit(at(x).operand(), a);
      }
      leave_at();
    }
    else if (is_seq(x))
    {
      bool result = visit_seq(seq(x), a);
      if (result) {
        visit(seq(x).left(), a);
        visit(seq(x).right(), a);
      }
      leave_seq();
    }
    else if (is_if_then(x))
    {
      bool result = visit_if_then(if_then(x), a);
      if (result) {
        visit(if_then(x).then_case(), a);
      }
      leave_if_then();
    }
    else if (is_if_then_else(x))
    {
      bool result = visit_if_then_else(if_then_else(x), a);
      if (result) {
        visit(if_then_else(x).then_case(), a);
        visit(if_then_else(x).else_case(), a);
      }
      leave_if_then_else();
    }
    else if (is_bounded_init(x))
    {
      bool result = visit_bounded_init(bounded_init(x), a);
      if (result) {
        visit(bounded_init(x).left(), a);
        visit(bounded_init(x).right(), a);
      }
      leave_bounded_init();
    }
    else if (is_merge(x))
    {
      bool result = visit_merge(merge(x), a);
      if (result) {
        visit(merge(x).left(), a);
        visit(merge(x).right(), a);
      }
      leave_merge();
    }
    else if (is_left_merge(x))
    {
      bool result = visit_left_merge(left_merge(x), a);
      if (result) {
        visit(left_merge(x).left(), a);
        visit(left_merge(x).right(), a);
      }
      leave_left_merge();
    }
    else if (is_choice(x))
    {
      bool result = visit_choice(choice(x), a);
      if (result) {
        visit(choice(x).left(), a);
        visit(choice(x).right(), a);
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
  virtual bool visit_action(const lps::action& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave action node
  virtual void leave_action()
  {}

  /// \brief Visit process_instance node
  /// \return The result of visiting the node
  virtual bool visit_process_instance(const process_instance& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave process_instance node
  virtual void leave_process_instance()
  {}

  /// \brief Visit process_instance_assignment node
  /// \return The result of visiting the node
  virtual bool visit_process_instance_assignment(const process_instance_assignment& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave process_instance_assignment node
  virtual void leave_process_instance_assignment()
  {}

  /// \brief Visit delta node
  /// \return The result of visiting the node
  virtual bool visit_delta(const delta& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave delta node
  virtual void leave_delta()
  {}

  /// \brief Visit tau node
  /// \return The result of visiting the node
  virtual bool visit_tau(const tau& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave tau node
  virtual void leave_tau()
  {}

  /// \brief Visit sum node
  /// \return The result of visiting the node
  virtual bool visit_sum(const sum& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave sum node
  virtual void leave_sum()
  {}

  /// \brief Visit block node
  /// \return The result of visiting the node
  virtual bool visit_block(const block& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave block node
  virtual void leave_block()
  {}

  /// \brief Visit hide node
  /// \return The result of visiting the node
  virtual bool visit_hide(const hide& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave hide node
  virtual void leave_hide()
  {}

  /// \brief Visit rename node
  /// \return The result of visiting the node
  virtual bool visit_rename(const rename& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave rename node
  virtual void leave_rename()
  {}

  /// \brief Visit comm node
  /// \return The result of visiting the node
  virtual bool visit_comm(const comm& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave comm node
  virtual void leave_comm()
  {}

  /// \brief Visit allow node
  /// \return The result of visiting the node
  virtual bool visit_allow(const allow& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave allow node
  virtual void leave_allow()
  {}

  /// \brief Visit sync node
  /// \return The result of visiting the node
  virtual bool visit_sync(const sync& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave sync node
  virtual void leave_sync()
  {}

  /// \brief Visit at node
  /// \return The result of visiting the node
  virtual bool visit_at(const at& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave at node
  virtual void leave_at()
  {}

  /// \brief Visit seq node
  /// \return The result of visiting the node
  virtual bool visit_seq(const seq& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave seq node
  virtual void leave_seq()
  {}

  /// \brief Visit if_then node
  /// \return The result of visiting the node
  virtual bool visit_if_then(const if_then& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave if_then node
  virtual void leave_if_then()
  {}

  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  virtual bool visit_if_then_else(const if_then_else& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave if_then_else node
  virtual void leave_if_then_else()
  {}

  /// \brief Visit bounded_init node
  /// \return The result of visiting the node
  virtual bool visit_bounded_init(const bounded_init& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave bounded_init node
  virtual void leave_bounded_init()
  {}

  /// \brief Visit merge node
  /// \return The result of visiting the node
  virtual bool visit_merge(const merge& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave merge node
  virtual void leave_merge()
  {}

  /// \brief Visit left_merge node
  /// \return The result of visiting the node
  virtual bool visit_left_merge(const left_merge& /* x */ )
  {
    return continue_recursion;
  }

  /// \brief Leave left_merge node
  virtual void leave_left_merge()
  {}

  /// \brief Visit choice node
  /// \return The result of visiting the node
  virtual bool visit_choice(const choice& /* x */ )
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
      visit_action(action(x));
      leave_action();
    }
    else if (is_process_instance(x))
    {
      visit_process_instance(process_instance(x));
      leave_process_instance();
    }
    else if (is_process_instance_assignment(x))
    {
      visit_process_instance_assignment(process_instance_assignment(x));
      leave_process_instance_assignment();
    }
    else if (is_delta(x))
    {
      visit_delta(delta(x));
      leave_delta();
    }
    else if (is_tau(x))
    {
      visit_tau(tau(x));
      leave_tau();
    }
    else if (is_sum(x))
    {
      bool result = visit_sum(sum(x));
      if (result) {
        visit(sum(x).operand());
      }
      leave_sum();
    }
    else if (is_block(x))
    {
      bool result = visit_block(block(x));
      if (result) {
        visit(block(x).operand());
      }
      leave_block();
    }
    else if (is_hide(x))
    {
      bool result = visit_hide(hide(x));
      if (result) {
        visit(hide(x).operand());
      }
      leave_hide();
    }
    else if (is_rename(x))
    {
      bool result = visit_rename(rename(x));
      if (result) {
        visit(rename(x).operand());
      }
      leave_rename();
    }
    else if (is_comm(x))
    {
      bool result = visit_comm(comm(x));
      if (result) {
        visit(comm(x).operand());
      }
      leave_comm();
    }
    else if (is_allow(x))
    {
      bool result = visit_allow(allow(x));
      if (result) {
        visit(allow(x).operand());
      }
      leave_allow();
    }
    else if (is_sync(x))
    {
      bool result = visit_sync(sync(x));
      if (result) {
        visit(sync(x).left());
        visit(sync(x).right());
      }
      leave_sync();
    }
    else if (is_at(x))
    {
      bool result = visit_at(at(x));
      if (result) {
        visit(at(x).operand());
      }
      leave_at();
    }
    else if (is_seq(x))
    {
      bool result = visit_seq(seq(x));
      if (result) {
        visit(seq(x).left());
        visit(seq(x).right());
      }
      leave_seq();
    }
    else if (is_if_then(x))
    {
      bool result = visit_if_then(if_then(x));
      if (result) {
        visit(if_then(x).then_case());
      }
      leave_if_then();
    }
    else if (is_if_then_else(x))
    {
      bool result = visit_if_then_else(if_then_else(x));
      if (result) {
        visit(if_then_else(x).then_case());
        visit(if_then_else(x).else_case());
      }
      leave_if_then_else();
    }
    else if (is_bounded_init(x))
    {
      bool result = visit_bounded_init(bounded_init(x));
      if (result) {
        visit(bounded_init(x).left());
        visit(bounded_init(x).right());
      }
      leave_bounded_init();
    }
    else if (is_merge(x))
    {
      bool result = visit_merge(merge(x));
      if (result) {
        visit(merge(x).left());
        visit(merge(x).right());
      }
      leave_merge();
    }
    else if (is_left_merge(x))
    {
      bool result = visit_left_merge(left_merge(x));
      if (result) {
        visit(left_merge(x).left());
        visit(left_merge(x).right());
      }
      leave_left_merge();
    }
    else if (is_choice(x))
    {
      bool result = visit_choice(choice(x));
      if (result) {
        visit(choice(x).left());
        visit(choice(x).right());
      }
      leave_choice();
    }
    
  }
};
//--- end generated visitor ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_VISITOR_H
