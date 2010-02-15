// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/linear_process_expression_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_LINEAR_PROCESS_EXPRESSION_VISITOR_H
#define MCRL2_PROCESS_DETAIL_LINEAR_PROCESS_EXPRESSION_VISITOR_H

#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/process_expression_visitor.h"
#include "mcrl2/process/detail/is_linear.h"

namespace mcrl2 {

namespace process {

namespace detail {

  /// \brief Checks if a process equation is linear.
  /// Use the is_linear() member function for this.
  struct linear_process_expression_visitor: public process_expression_visitor<void>
  {
    /// \brief The process equation that is checked.
    process_equation eqn;

    /// \brief Exception that is thrown by linear_process_expression_visitor.
    struct non_linear_process
    {
      std::string msg;
        
      non_linear_process(const std::string& s)
        : msg(s)
      {}
    };

    /// \brief These names can be used as return types of the visit functions, to make
    /// the code more readible.
    enum return_type
    {
      stop_recursion = false,
      continue_recursion = true
    };

    /// \brief Returns true if the argument is a process instance
    /// \param x A process expression
    /// \return True if the argument is a process instance
    bool is_process(const process_expression& x)
    {
      return is_process_instance(x)
          || is_process_instance_assignment(x)
          ;
    }

    /// \brief Returns true if the argument is a deadlock
    /// \param x A process expression
    /// \return True if the argument is a deadlock
    bool is_timed_deadlock(const process_expression& x)
    {
      return is_delta(x)
          || is_at(x)
          ;
    }

    /// \brief Returns true if the argument is a multi-action
    /// \param x A process expression
    /// \return True if the argument is a multi-action
    bool is_multiaction(const process_expression& x)
    {
      return is_tau(x)
          || is_sync(x)
          || is_action(x)
          ;
    }

    /// \brief Returns true if the argument is a multi-action
    /// \param x A process expression
    /// \return True if the argument is a multi-action
    bool is_timed_multiaction(const process_expression& x)
    {
      return is_at(x)
          || is_multiaction(x);
    }

    /// \brief Returns true if the argument is an action prefix
    /// \param x A process expression
    /// \return True if the argument is an action prefix
    bool is_action_prefix(const process_expression& x)
    {
      return is_seq(x)
          || is_timed_multiaction(x);
    }

    /// \brief Returns true if the argument is a conditional deadlock
    /// \param x A process expression
    /// \return True if the argument is a conditional deadlock
    bool is_conditional_deadlock(const process_expression& x)
    {
      return is_if_then(x)
          || is_timed_deadlock(x);
    }

    /// \brief Returns true if the argument is a conditional action prefix.
    /// \param x A process expression
    /// \return True if the argument is a conditional action prefix.
    bool is_conditional_action_prefix(const process_expression& x)
    {
      return is_if_then(x)
          || is_action_prefix(x);
    }

    /// \brief Returns true if the argument is an alternative composition
    /// \param x A process expression
    /// \return True if the argument is an alternative composition
    bool is_alternative(const process_expression& x)
    {
      return is_sum(x)
          || is_conditional_action_prefix(x)
          || is_conditional_deadlock(x)
          ;
    }

    /// \brief Returns true if the argument is a linear process
    /// \param x A process expression
    /// \return True if the argument is a linear process
    bool is_linear_process_term(const process_expression& x)
    {
      return is_choice(x)
          || is_alternative(x)
          ;
    }

    // TODO: check if this function is needed
    /// \brief Visit process_instance node
    /// \return The result of visiting the node
    /// \param x A process instance
    bool visit_process_instance(const process_instance& x)
    {
      if (!detail::check_process_instance(eqn, x))
      {
        throw non_linear_process(core::pp(x) + " is not a valid process instance");
      }
      return continue_recursion;
    }

    /// \brief Visit process_instance_assignment node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param pi A process identifier
    /// \param v A sequence of assignments to data variables
    bool visit_process_instance_assignment(const process_instance_assignment& x)
    {
      if (!detail::check_process_instance_assignment(eqn, x))
      {
        throw non_linear_process(core::pp(x) + " is not a valid process instance assignment");
      }
      return continue_recursion;
    }

    /// \brief Visit sum node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param v A sequence of data variables
    /// \param right A process expression
    bool visit_sum(const sum& x)
    {
      if (!is_alternative(x.operand()))
      {
        throw non_linear_process(core::pp(x.operand()) + " is not an alternative expression");
      }
      return continue_recursion;
    }

    /// \brief Visit block node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param s A sequence of identifiers
    /// \param right A process expression
    bool visit_block(const block& x)
    {
      throw non_linear_process("block expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Visit hide node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param s A sequence of identifiers
    /// \param right A process expression
    bool visit_hide(const hide& x)
    {
      throw non_linear_process("hide expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Visit rename node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param r A sequence of rename expressions
    /// \param right A process expression
    bool visit_rename(const rename& x)
    {
      throw non_linear_process("rename expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Visit comm node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param c A sequence of communication expressions
    /// \param right A process expression
    bool visit_comm(const comm& x)
    {
      throw non_linear_process("comm expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Visit allow node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param s A sequence of multi-action names
    /// \param right A process expression
    bool visit_allow(const allow& x)
    {
      throw non_linear_process("allow expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Visit sync node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param left A process expression
    /// \param right A process expression
    bool visit_sync(const sync& x)
    {
      if (!is_multiaction(x.left()) || !is_multiaction(x.right()))
      {
        if (!is_multiaction(x.left()))
        {
          throw non_linear_process(core::pp(x.left()) + " is not a multi action");
        }
        else
        {
          throw non_linear_process(core::pp(x.right()) + " is not a multi action");
        }
      }
      return continue_recursion;
    }

    /// \brief Visit at node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param left A process expression
    /// \param d A data expression
    bool visit_at(const at& x)
    {
      if (!is_multiaction(x.operand()) && !is_delta(x.operand()))
      {
        throw non_linear_process(core::pp(x.operand()) + " is not a multi action and not a deadlock");
      }
      return continue_recursion;
    }

    /// \brief Visit seq node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param left A process expression
    /// \param right A process expression
    bool visit_seq(const seq& x)
    {
      if (!is_timed_multiaction(x.left()) || !is_process(x.right()))
      {
        throw non_linear_process(core::pp(x.left()) + " is not a timed multi action and not a process");
      }
      if (is_process_instance(x.right()))
      {
        process_instance q = x.right();
        if (q.identifier() != eqn.identifier())
        {
          throw non_linear_process(core::pp(q) + " has an unexpected identifier");
        }
      }
      else if (is_process_instance_assignment(x.right()))
      {
        process_instance_assignment q = x.right();
        if (q.identifier() != eqn.identifier())
        {
          throw non_linear_process(core::pp(q) + " has an unexpected identifier");
        }
      }
      else
      {
        std::cerr << "seq right hand side: " << core::pp(x.right()) << std::endl;
        throw std::runtime_error("unexpected error in visit_seq");
      }
      return continue_recursion;
    }

    /// \brief Visit if_then node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param d A data expression
    /// \param right A process expression
    bool visit_if_then(const if_then& x)
    {
      if (!is_action_prefix(x.then_case()) && !is_timed_deadlock(x.then_case()))
      {
        throw non_linear_process(core::pp(x) + " is not an action prefix and not a timed deadlock");
      }
      return continue_recursion;
    }

    /// \brief Visit if_then_else node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param d A data expression
    /// \param left A process expression
    /// \param right A process expression
    bool visit_if_then_else(const if_then_else& x)
    {
      throw non_linear_process("if then else expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Visit bounded_init node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param left A process expression
    /// \param right A process expression
    bool visit_bounded_init(const bounded_init& x)
    {
      throw non_linear_process("bounded init expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Visit merge node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param left A process expression
    /// \param right A process expression
    bool visit_merge(const merge& x)
    {
      throw non_linear_process("merge expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Visit left_merge node
    /// \return The result of visiting the node
    /// \param x A process expression
    /// \param left A process expression
    /// \param right A process expression
    bool visit_left_merge(const left_merge& x)
    {
      throw non_linear_process("left merge expression " + core::pp(x) + " encountered");
      return continue_recursion;
    }

    /// \brief Returns true if the process equation e is linear.
    /// \param e A process equation
    /// \return True if the process equation e is linear.
    bool is_linear(const process_equation& e, bool verbose = false)
    {
      eqn = e;
      try
      {
        visit(e.expression());
      }
      catch(non_linear_process& p)
      {
        if (verbose)
        {
          std::clog << p.msg << std::endl;
        }
        return false;
      }
      return true;
    }
  };

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_LINEAR_PROCESS_EXPRESSION_VISITOR_H
