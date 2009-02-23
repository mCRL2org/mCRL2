// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/linear_process_conversion_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LINEAR_PROCESS_CONVERSION_VISITOR_H
#define MCRL2_LPS_DETAIL_LINEAR_PROCESS_CONVERSION_VISITOR_H

#include "mcrl2/lps/summand.h"
#include "mcrl2/lps/process.h"
#include "mcrl2/lps/process_expression_visitor.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Converts a process expression into linear process format.
  /// Use the convert() member function for this.
  struct linear_process_conversion_visitor: public process_expression_visitor<void>
  {
    /// \brief The result of the conversion.
    atermpp::vector<summand> result;

    /// \brief The process equation that is checked.
    process_equation m_equation;

    /// \brief Contains intermediary results.
    data::data_assignment_list m_next_state;

    /// \brief Contains intermediary results.
    multi_action m_multi_action;

    /// \brief Contains intermediary results.
    deadlock m_deadlock;
    
    /// \brief Contains intermediary results.
    summand m_summand;

    /// \brief The traits class for process expressions.
    typedef core::term_traits<process_expression> tr;

    /// \brief Exception that is thrown by linear_process_conversion_visitor.
    struct non_linear_process
    {};
  
    summand add_summand_variables(const summand& s, const data::data_variable_list& v)
    {
      return set_summation_variables(s, s.summation_variables() + v);     
    }
    
    /// \brief These names can be used as return types of the visit functions, to make
    /// the code more readible.
    enum return_type
    {
      stop_recursion = false,
      continue_recursion = true
    };

    /// \brief Visit tau node
    /// \return The result of visiting the node
    bool visit_tau(const process_expression& x)
    {
      m_multi_action = multi_action();
      return stop_recursion;
    }

    /// \brief Visit action node
    /// \return The result of visiting the node
    bool visit_action(const process_expression& x, const action_label& l, const data::data_expression_list& v)
    {
      m_multi_action = multi_action(x);
      return stop_recursion;
    }

    /// \brief Visit sum node
    /// \return The result of visiting the node
    bool visit_sum(const process_expression& x, const data::data_variable_list& v, const process_expression& right)
    {
      visit(right);
      m_summand = add_summand_variables(m_summand, v);
      return stop_recursion;
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
      visit(left);
      multi_action l = m_multi_action;
      visit(right);
      multi_action r = m_multi_action;
      m_multi_action = l + r;
      return stop_recursion;
    }

    /// \brief Visit at_time node
    /// \return The result of visiting the node
    bool visit_at_time(const process_expression& x, const process_expression& left, const data::data_expression& d)
    {
      visit(left);
      if (tr::is_delta(x))
      {
        m_deadlock.time() = d;
      }
      else
      {
        m_multi_action.time() = d;
      }
      return stop_recursion;
    }

    /// \brief Visit seq node
    /// \return The result of visiting the node
    bool visit_seq(const process_expression& x, const process_expression& left, const process_expression& right)
    {
      visit(left);
      process p = right;
      m_next_state = data::make_assignment_list(m_equation.variables2(), p.expressions());
      return stop_recursion;
    }

    /// \brief Visit if_then node
    /// \return The result of visiting the node
    bool visit_if_then(const process_expression& x, const data::data_expression& d, const process_expression& right)
    {
      if (tr::is_seq(right))
      {
        visit(right);
        m_summand = summand(data::data_variable_list(), d, m_multi_action, m_next_state);
      }
      else
      { // delta() or at_time(delta(), ...)
        visit(right);
        m_summand = summand(data::data_variable_list(), d, m_deadlock);
      }
      return stop_recursion;
    }

    /// \brief Visit if_then_else node
    /// \return The result of visiting the node
    bool visit_if_then_else(const process_expression& x, const data::data_expression& d, const process_expression& left, const process_expression& right)
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
    
    /// \brief Visit choice node
    /// \return The result of visiting the node
    bool visit_choice(const process_expression& x, const process_expression& left, const process_expression& right)
    {
      visit(left);
      if (!tr::is_choice(left))
      {
        result.push_back(m_summand);
      }
      visit(right);
      if (!tr::is_choice(right))
      {
        result.push_back(m_summand);
      }
      return stop_recursion;
    }

    /// \brief Returns true if the process equation e is linear.
    void convert(const process_equation& e)
    {
      m_equation = e;
      result.clear();
      visit(m_equation.expression());
    }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LINEAR_PROCESS_CONVERSION_VISITOR_H
