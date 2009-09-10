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

//--- start generated visitor ---//
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
              
  /// \brief Visit process_action node
  /// \return The result of visiting the node
  virtual process_expression visit_process_action(const process_action& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit process_instance node
  /// \return The result of visiting the node
  virtual process_expression visit_process_instance(const process_instance& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit process_instance_assignment node
  /// \return The result of visiting the node
  virtual process_expression visit_process_instance_assignment(const process_instance_assignment& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit delta node
  /// \return The result of visiting the node
  virtual process_expression visit_delta(const delta& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit tau node
  /// \return The result of visiting the node
  virtual process_expression visit_tau(const tau& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit sum node
  /// \return The result of visiting the node
  virtual process_expression visit_sum(const sum& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit block node
  /// \return The result of visiting the node
  virtual process_expression visit_block(const block& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit hide node
  /// \return The result of visiting the node
  virtual process_expression visit_hide(const hide& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit rename node
  /// \return The result of visiting the node
  virtual process_expression visit_rename(const rename& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit comm node
  /// \return The result of visiting the node
  virtual process_expression visit_comm(const comm& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit allow node
  /// \return The result of visiting the node
  virtual process_expression visit_allow(const allow& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit sync node
  /// \return The result of visiting the node
  virtual process_expression visit_sync(const sync& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit at node
  /// \return The result of visiting the node
  virtual process_expression visit_at(const at& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit seq node
  /// \return The result of visiting the node
  virtual process_expression visit_seq(const seq& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit if_then node
  /// \return The result of visiting the node
  virtual process_expression visit_if_then(const if_then& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  virtual process_expression visit_if_then_else(const if_then_else& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit bounded_init node
  /// \return The result of visiting the node
  virtual process_expression visit_bounded_init(const bounded_init& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit merge node
  /// \return The result of visiting the node
  virtual process_expression visit_merge(const merge& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit left_merge node
  /// \return The result of visiting the node
  virtual process_expression visit_left_merge(const left_merge& x, Arg& /* a */)
  {
    return process_expression();
  }
              
  /// \brief Visit choice node
  /// \return The result of visiting the node
  virtual process_expression visit_choice(const choice& x, Arg& /* a */)
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
      result = visit_process_action(x, a);
      if (!is_finished(result))
      {
        result = process_action(process_action(x).label(), process_action(x).arguments());
      }
    }
    else if (is_process_instance(x))
    {
      result = visit_process_instance(x, a);
      if (!is_finished(result))
      {
        result = process_instance(process_instance(x).identifier(), process_instance(x).actual_parameters());
      }
    }
    else if (is_process_instance_assignment(x))
    {
      result = visit_process_instance_assignment(x, a);
      if (!is_finished(result))
      {
        result = process_instance_assignment(process_instance_assignment(x).identifier(), process_instance_assignment(x).assignments());
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
      result = visit_sum(x, a);
      if (!is_finished(result))
      {
        result = sum(sum(x).bound_variables(), visit(sum(x).operand(), a));
      }
    }
    else if (is_block(x))
    {
      result = visit_block(x, a);
      if (!is_finished(result))
      {
        result = block(block(x).block_set(), visit(block(x).operand(), a));
      }
    }
    else if (is_hide(x))
    {
      result = visit_hide(x, a);
      if (!is_finished(result))
      {
        result = hide(hide(x).hide_set(), visit(hide(x).operand(), a));
      }
    }
    else if (is_rename(x))
    {
      result = visit_rename(x, a);
      if (!is_finished(result))
      {
        result = rename(rename(x).rename_set(), visit(rename(x).operand(), a));
      }
    }
    else if (is_comm(x))
    {
      result = visit_comm(x, a);
      if (!is_finished(result))
      {
        result = comm(comm(x).comm_set(), visit(comm(x).operand(), a));
      }
    }
    else if (is_allow(x))
    {
      result = visit_allow(x, a);
      if (!is_finished(result))
      {
        result = allow(allow(x).allow_set(), visit(allow(x).operand(), a));
      }
    }
    else if (is_sync(x))
    {
      result = visit_sync(x, a);
      if (!is_finished(result))
      {
        result = sync(visit(sync(x).left(), a), visit(sync(x).right(), a));
      }
    }
    else if (is_at(x))
    {
      result = visit_at(x, a);
      if (!is_finished(result))
      {
        result = at(visit(at(x).operand(), a), at(x).time_stamp());
      }
    }
    else if (is_seq(x))
    {
      result = visit_seq(x, a);
      if (!is_finished(result))
      {
        result = seq(visit(seq(x).left(), a), visit(seq(x).right(), a));
      }
    }
    else if (is_if_then(x))
    {
      result = visit_if_then(x, a);
      if (!is_finished(result))
      {
        result = if_then(if_then(x).condition(), visit(if_then(x).then_case(), a));
      }
    }
    else if (is_if_then_else(x))
    {
      result = visit_if_then_else(x, a);
      if (!is_finished(result))
      {
        result = if_then_else(if_then_else(x).condition(), visit(if_then_else(x).then_case(), a), visit(if_then_else(x).else_case(), a));
      }
    }
    else if (is_bounded_init(x))
    {
      result = visit_bounded_init(x, a);
      if (!is_finished(result))
      {
        result = bounded_init(visit(bounded_init(x).left(), a), visit(bounded_init(x).right(), a));
      }
    }
    else if (is_merge(x))
    {
      result = visit_merge(x, a);
      if (!is_finished(result))
      {
        result = merge(visit(merge(x).left(), a), visit(merge(x).right(), a));
      }
    }
    else if (is_left_merge(x))
    {
      result = visit_left_merge(x, a);
      if (!is_finished(result))
      {
        result = left_merge(visit(left_merge(x).left(), a), visit(left_merge(x).right(), a));
      }
    }
    else if (is_choice(x))
    {
      result = visit_choice(x, a);
      if (!is_finished(result))
      {
        result = choice(visit(choice(x).left(), a), visit(choice(x).right(), a));
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
              
  /// \brief Visit process_action node
  /// \return The result of visiting the node
  virtual process_expression visit_process_action(const process_action& x)
  {
    return process_expression();
  }
              
  /// \brief Visit process_instance node
  /// \return The result of visiting the node
  virtual process_expression visit_process_instance(const process_instance& x)
  {
    return process_expression();
  }
              
  /// \brief Visit process_instance_assignment node
  /// \return The result of visiting the node
  virtual process_expression visit_process_instance_assignment(const process_instance_assignment& x)
  {
    return process_expression();
  }
              
  /// \brief Visit delta node
  /// \return The result of visiting the node
  virtual process_expression visit_delta(const delta& x)
  {
    return process_expression();
  }
              
  /// \brief Visit tau node
  /// \return The result of visiting the node
  virtual process_expression visit_tau(const tau& x)
  {
    return process_expression();
  }
              
  /// \brief Visit sum node
  /// \return The result of visiting the node
  virtual process_expression visit_sum(const sum& x)
  {
    return process_expression();
  }
              
  /// \brief Visit block node
  /// \return The result of visiting the node
  virtual process_expression visit_block(const block& x)
  {
    return process_expression();
  }
              
  /// \brief Visit hide node
  /// \return The result of visiting the node
  virtual process_expression visit_hide(const hide& x)
  {
    return process_expression();
  }
              
  /// \brief Visit rename node
  /// \return The result of visiting the node
  virtual process_expression visit_rename(const rename& x)
  {
    return process_expression();
  }
              
  /// \brief Visit comm node
  /// \return The result of visiting the node
  virtual process_expression visit_comm(const comm& x)
  {
    return process_expression();
  }
              
  /// \brief Visit allow node
  /// \return The result of visiting the node
  virtual process_expression visit_allow(const allow& x)
  {
    return process_expression();
  }
              
  /// \brief Visit sync node
  /// \return The result of visiting the node
  virtual process_expression visit_sync(const sync& x)
  {
    return process_expression();
  }
              
  /// \brief Visit at node
  /// \return The result of visiting the node
  virtual process_expression visit_at(const at& x)
  {
    return process_expression();
  }
              
  /// \brief Visit seq node
  /// \return The result of visiting the node
  virtual process_expression visit_seq(const seq& x)
  {
    return process_expression();
  }
              
  /// \brief Visit if_then node
  /// \return The result of visiting the node
  virtual process_expression visit_if_then(const if_then& x)
  {
    return process_expression();
  }
              
  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  virtual process_expression visit_if_then_else(const if_then_else& x)
  {
    return process_expression();
  }
              
  /// \brief Visit bounded_init node
  /// \return The result of visiting the node
  virtual process_expression visit_bounded_init(const bounded_init& x)
  {
    return process_expression();
  }
              
  /// \brief Visit merge node
  /// \return The result of visiting the node
  virtual process_expression visit_merge(const merge& x)
  {
    return process_expression();
  }
              
  /// \brief Visit left_merge node
  /// \return The result of visiting the node
  virtual process_expression visit_left_merge(const left_merge& x)
  {
    return process_expression();
  }
              
  /// \brief Visit choice node
  /// \return The result of visiting the node
  virtual process_expression visit_choice(const choice& x)
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
      result = visit_process_action(x);
      if (!is_finished(result))
      {
        result = process_action(process_action(x).label(), process_action(x).arguments());
      }
    }
    else if (is_process_instance(x))
    {
      result = visit_process_instance(x);
      if (!is_finished(result))
      {
        result = process_instance(process_instance(x).identifier(), process_instance(x).actual_parameters());
      }
    }
    else if (is_process_instance_assignment(x))
    {
      result = visit_process_instance_assignment(x);
      if (!is_finished(result))
      {
        result = process_instance_assignment(process_instance_assignment(x).identifier(), process_instance_assignment(x).assignments());
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
      result = visit_sum(x);
      if (!is_finished(result))
      {
        result = sum(sum(x).bound_variables(), visit(sum(x).operand()));
      }
    }
    else if (is_block(x))
    {
      result = visit_block(x);
      if (!is_finished(result))
      {
        result = block(block(x).block_set(), visit(block(x).operand()));
      }
    }
    else if (is_hide(x))
    {
      result = visit_hide(x);
      if (!is_finished(result))
      {
        result = hide(hide(x).hide_set(), visit(hide(x).operand()));
      }
    }
    else if (is_rename(x))
    {
      result = visit_rename(x);
      if (!is_finished(result))
      {
        result = rename(rename(x).rename_set(), visit(rename(x).operand()));
      }
    }
    else if (is_comm(x))
    {
      result = visit_comm(x);
      if (!is_finished(result))
      {
        result = comm(comm(x).comm_set(), visit(comm(x).operand()));
      }
    }
    else if (is_allow(x))
    {
      result = visit_allow(x);
      if (!is_finished(result))
      {
        result = allow(allow(x).allow_set(), visit(allow(x).operand()));
      }
    }
    else if (is_sync(x))
    {
      result = visit_sync(x);
      if (!is_finished(result))
      {
        result = sync(visit(sync(x).left()), visit(sync(x).right()));
      }
    }
    else if (is_at(x))
    {
      result = visit_at(x);
      if (!is_finished(result))
      {
        result = at(visit(at(x).operand()), at(x).time_stamp());
      }
    }
    else if (is_seq(x))
    {
      result = visit_seq(x);
      if (!is_finished(result))
      {
        result = seq(visit(seq(x).left()), visit(seq(x).right()));
      }
    }
    else if (is_if_then(x))
    {
      result = visit_if_then(x);
      if (!is_finished(result))
      {
        result = if_then(if_then(x).condition(), visit(if_then(x).then_case()));
      }
    }
    else if (is_if_then_else(x))
    {
      result = visit_if_then_else(x);
      if (!is_finished(result))
      {
        result = if_then_else(if_then_else(x).condition(), visit(if_then_else(x).then_case()), visit(if_then_else(x).else_case()));
      }
    }
    else if (is_bounded_init(x))
    {
      result = visit_bounded_init(x);
      if (!is_finished(result))
      {
        result = bounded_init(visit(bounded_init(x).left()), visit(bounded_init(x).right()));
      }
    }
    else if (is_merge(x))
    {
      result = visit_merge(x);
      if (!is_finished(result))
      {
        result = merge(visit(merge(x).left()), visit(merge(x).right()));
      }
    }
    else if (is_left_merge(x))
    {
      result = visit_left_merge(x);
      if (!is_finished(result))
      {
        result = left_merge(visit(left_merge(x).left()), visit(left_merge(x).right()));
      }
    }
    else if (is_choice(x))
    {
      result = visit_choice(x);
      if (!is_finished(result))
      {
        result = choice(visit(choice(x).left()), visit(choice(x).right()));
      }
    }
    
#ifdef MCRL2_PROCESS_EXPRESSION_BUILDER_DEBUG
  std::cerr << "<visit result>" << pp(result) << std::endl;
#endif
    return result;
  }
};
//--- end generated visitor ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_BUILDER_H
