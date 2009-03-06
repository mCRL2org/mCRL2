// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/linear_process_expression_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LINEAR_PROCESS_EXPRESSION_VISITOR_H
#define MCRL2_LPS_DETAIL_LINEAR_PROCESS_EXPRESSION_VISITOR_H

#include "mcrl2/core/term_traits.h"
#include "mcrl2/lps/process.h"
#include "mcrl2/lps/process_expression_visitor.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Checks if a process equation is linear.
  /// Use the is_linear() member function for this.
  struct linear_process_expression_visitor: public process_expression_visitor<void>
  {
    /// \brief The traits class for process expressions.
    typedef core::term_traits<process_expression> tr;

    /// \brief The process equation that is checked.
    process_equation eqn;

    /// \brief Exception that is thrown by linear_process_expression_visitor.
    struct non_linear_process
    {};

    /// \brief These names can be used as return types of the visit functions, to make
    /// the code more readible.
    enum return_type
    {
      stop_recursion = false,
      continue_recursion = true
    };

    /// \brief Returns true if the process assignment a matches with the
    /// process equation eqn.
    bool check_process_assignment(const process_assignment& a) const
    {
      if (a.identifier() != eqn.name())
      {
        return false;
      }
      new_data::assignment_list a1 = a.assignments();
      new_data::variable_list v = eqn.variables1();
      if (a1.size() != v.size())
      {
        return false;
      }
      new_data::assignment_list::iterator i;
      new_data::variable_list::iterator j;
      for (i = a1.begin(), j = v.begin(); i != a1.end(); i++, j++)
      {
        if (i->lhs() != *j)
        {
          return false;
        }
      }
      return true;
    }

    bool is_process_variable(const process_expression& x)
    {
      return tr::is_process(x)
          || tr::is_process_assignment(x)
          ;
    }

    bool is_timed_deadlock(const process_expression& x)
    {
      return tr::is_delta(x)
          || tr::is_at_time(x)
          ;
    }

    bool is_multiaction(const process_expression& x)
    {
      return tr::is_tau(x)
          || tr::is_sync(x)
          || tr::is_action(x)
          ;
    }

    bool is_timed_multiaction(const process_expression& x)
    {
      return tr::is_at_time(x)
          || is_multiaction(x);
    }

    bool is_action_prefix(const process_expression& x)
    {
      return tr::is_seq(x)
          || is_timed_multiaction(x);
    }

    bool is_conditional_deadlock(const process_expression& x)
    {
      return tr::is_if_then(x)
          || is_timed_deadlock(x);
    }

    bool is_conditional_action_prefix(const process_expression& x)
    {
      return tr::is_if_then(x)
          || is_action_prefix(x);
    }

    bool is_alternative(const process_expression& x)
    {
      return tr::is_sum(x)
          || is_conditional_action_prefix(x)
          || is_conditional_deadlock(x)
          ;
    }

    bool is_linear_process_term(const process_expression& x)
    {
      return tr::is_choice(x)
          || is_alternative(x)
          ;
    }

    /// \brief Visit process node
    /// \return The result of visiting the node
    bool visit_process(const process_expression& x, const process_identifier pi, const new_data::data_expression_list& v)
    {
      return continue_recursion;
    }

    /// \brief Visit process_assignment node
    /// \return The result of visiting the node
    bool visit_process_assignment(const process_expression& x, const process_identifier& pi, const new_data::assignment_list& v)
    {
      process_assignment a = x;
      if (!check_process_assignment(a))
      {
        throw non_linear_process();
      }
      return continue_recursion;
    }

    /// \brief Visit sum node
    /// \return The result of visiting the node
    bool visit_sum(const process_expression& x, const new_data::variable_list& v, const process_expression& right)
    {
      if (!is_alternative(right))
      {
        throw non_linear_process();
      }
      return continue_recursion;
    }

    /// \brief Visit block node
    /// \return The result of visiting the node
    bool visit_block(const process_expression& x, const core::identifier_string_list& s, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Visit hide node
    /// \return The result of visiting the node
    bool visit_hide(const process_expression& x, const core::identifier_string_list& s, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Visit rename node
    /// \return The result of visiting the node
    bool visit_rename(const process_expression& x, const rename_expression_list& r, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Visit comm node
    /// \return The result of visiting the node
    bool visit_comm(const process_expression& x, const communication_expression_list& c, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Visit allow node
    /// \return The result of visiting the node
    bool visit_allow(const process_expression& x, const multi_action_name_list& s, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Visit sync node
    /// \return The result of visiting the node
    bool visit_sync(const process_expression& x, const process_expression& left, const process_expression& right)
    {
      if (!is_multiaction(left) || !is_multiaction(right))
      {
        throw non_linear_process();
      }
      return continue_recursion;
    }

    /// \brief Visit at_time node
    /// \return The result of visiting the node
    bool visit_at_time(const process_expression& x, const process_expression& left, const new_data::data_expression& d)
    {
      if (!is_multiaction(left) && !tr::is_delta(left))
      {
        throw non_linear_process();
      }
      return continue_recursion;
    }

    /// \brief Visit seq node
    /// \return The result of visiting the node
    bool visit_seq(const process_expression& x, const process_expression& left, const process_expression& right)
    {
      if (!is_timed_multiaction(left) || !is_process_variable(right))
      {
        throw non_linear_process();
      }
      if (!tr::is_process(right))
      {
        throw std::runtime_error("unexpected error in visit_seq");
      }
      process q = right;
      if (q.identifier() != eqn.name())
      {
        throw non_linear_process();
      }
      return continue_recursion;
    }

    /// \brief Visit if_then node
    /// \return The result of visiting the node
    bool visit_if_then(const process_expression& x, const new_data::data_expression& d, const process_expression& right)
    {
      if (!is_action_prefix(right) && !is_timed_deadlock(right))
      {
        throw non_linear_process();
      }
      return continue_recursion;
    }

    /// \brief Visit if_then_else node
    /// \return The result of visiting the node
    bool visit_if_then_else(const process_expression& x, const new_data::data_expression& d, const process_expression& left, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Visit binit node
    /// \return The result of visiting the node
    bool visit_binit(const process_expression& x, const process_expression& left, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Visit merge node
    /// \return The result of visiting the node
    bool visit_merge(const process_expression& x, const process_expression& left, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Visit lmerge node
    /// \return The result of visiting the node
    bool visit_lmerge(const process_expression& x, const process_expression& left, const process_expression& right)
    {
      throw non_linear_process();
      return continue_recursion;
    }

    /// \brief Returns true if the process equation e is linear.
    bool is_linear(const process_equation& e)
    {
      eqn = e;
      try
      {
        visit(e.expression());
      }
      catch(non_linear_process&)
      {
        return false;
      }
      return true;
    }

    /// \brief Returns true if the process specification is linear.
    bool is_linear(const process_specification& p)
    {
      if (p.equations().size() != 1)
      {
        return false;
      }
      if (!is_linear(*p.equations().begin()))
      {
        return false;
      }
      if (!tr::is_process(p.init().expression()))
      {
        return false;
      }
      return true;
    }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LINEAR_PROCESS_EXPRESSION_VISITOR_H
