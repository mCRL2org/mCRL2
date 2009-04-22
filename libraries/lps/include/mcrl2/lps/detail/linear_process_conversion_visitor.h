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

#include <stdexcept>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/lps/summand.h"
#include "mcrl2/lps/process.h"
#include "mcrl2/lps/process_expression_visitor.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Converts a process expression into linear process format.
  /// Use the \p convert member functions for this.
  struct linear_process_conversion_visitor: public process_expression_visitor<void>
  {
    /// \brief The result of the conversion.
    atermpp::vector<summand> result;

    /// \brief The process equation that is checked.
    process_equation m_equation;

    /// \brief Contains intermediary results.
    data::variable_list m_sum_variables;

    /// \brief Contains intermediary results.
    data::assignment_list m_next_state;

    /// \brief Contains intermediary results.
    multi_action m_multi_action;

    /// \brief Contains intermediary results.
    deadlock m_deadlock;

    /// \brief True if m_deadlock was changed.
    bool m_deadlock_changed;

    /// \brief True if m_multi_action was changed.
    bool m_multi_action_changed;

    /// \brief Contains intermediary results.
    data::data_expression m_condition;

    /// \brief Contains intermediary results.
    summand m_summand;

    /// \brief The traits class for process expressions.
    typedef core::term_traits<process_expression> tr;

    /// \brief Exception that is thrown to denote that the process is not linear.
    struct non_linear_process
    {};

    /// \brief Exception that is thrown to denote that a linear process is encountered
    /// that can not be transformed into LPS format. For example P = a.
    struct unsupported_linear_process
    {};

    void clear_summand()
    {
      m_sum_variables = data::variable_list();
      m_summand = summand();
      m_deadlock = deadlock();
      m_deadlock_changed = false;
      m_multi_action = multi_action();
      m_multi_action_changed = false;
      m_condition = data::sort_bool_::true_();
      m_next_state = data::assignment_list();
    }

    void add_summand()
    {
      if (m_summand == summand())
      {
        if ( (m_multi_action != multi_action()) || m_multi_action_changed)
        {
          if (m_next_state == data::assignment_list())
          {
            throw unsupported_linear_process();
          }
          m_summand = summand(m_sum_variables, m_condition, m_multi_action, m_next_state);
        }
        else if ( (m_deadlock != deadlock()) || m_deadlock_changed)
        {
          m_summand = summand(m_sum_variables, m_condition, m_deadlock);
        }
        else
        {
          return;
        }
      }
      result.push_back(m_summand);
// std::cout << "adding summand" << m_multi_action_changed << m_deadlock_changed << "\n" << core::pp(m_summand) << std::endl;
      clear_summand();
    }
  
    /// \brief These names can be used as return types of the visit functions, to make
    /// the code more readible.
    enum return_type
    {
      stop_recursion = false,
      continue_recursion = true
    };

    /// \brief Visit delta node
    /// \return The result of visiting the node
    bool visit_delta(const process_expression& x)
    {
      m_deadlock = deadlock();
      m_deadlock_changed = true;
// std::cout << "adding deadlock\n" << m_deadlock.to_string() << std::endl;
      return stop_recursion;
    }

    /// \brief Visit tau node
    /// \return The result of visiting the node
    bool visit_tau(const process_expression& x)
    {
      m_multi_action = multi_action();
      m_multi_action_changed = true;
// std::cout << "adding multi action tau\n" << m_multi_action.to_string() << std::endl;
      return stop_recursion;
    }

    /// \brief Visit action node
    /// \return The result of visiting the node
    bool visit_action(const process_expression& x, const action_label& l, const data::data_expression_list& v)
    {
      m_multi_action = multi_action(x);
// std::cout << "adding multi action\n" << m_multi_action.to_string() << std::endl;
      return stop_recursion;
    }

    /// \brief Visit sum node
    /// \return The result of visiting the node
    bool visit_sum(const process_expression& x, const data::variable_list& v, const process_expression& right)
    {
      visit(right);
      m_sum_variables = m_sum_variables + v;
// std::cout << "adding sum variables\n" << core::pp(v) << std::endl;
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
// std::cout << "adding multi action\n" << m_multi_action.to_string() << std::endl;
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
// std::cout << "adding deadlock\n" << m_deadlock.to_string() << std::endl;
      }
      else
      {
        m_multi_action.time() = d;
// std::cout << "adding multi action\n" << m_multi_action.to_string() << std::endl;
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
// std::cout << "adding next state\n" << core::pp(m_next_state) << std::endl;
      return stop_recursion;
    }

    /// \brief Visit if_then node
    /// \return The result of visiting the node
    bool visit_if_then(const process_expression& x, const data::data_expression& d, const process_expression& right)
    {
      visit(right);
      m_condition = d;
// std::cout << "adding condition\n" << core::pp(m_condition) << std::endl;
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
        add_summand();
      }
      visit(right);
      if (!tr::is_choice(right))
      {
        add_summand();
      }
      return stop_recursion;
    }

    /// \brief Returns true if the process equation e is linear.
    void convert(const process_equation& e)
    {
      clear_summand();
      try
      {
        m_equation = e;
        visit(m_equation.expression());
        add_summand(); // needed if it is not a choice
      }
      catch(unsupported_linear_process)
      {
        result.clear();
        std::cerr << "Unsupported linear expression encountered in linear_process_conversion_visitor" << std::endl;
      }
      catch(non_linear_process)
      {
        result.clear();
        std::cerr << "Non-linear expression encountered in linear_process_conversion_visitor" << std::endl;
      }
    }

    /// \brief Converts a process_specification into a specification.
    /// Throws \p unsupported_linear_process if an unsupported linear process expression is encountered.
    /// Throws \p non_linear_process if the process is not linear.
    specification convert(const process_specification& p)
    {
      data::variable_list m_process_parameters;
      result.clear();

      for (process_equation_list::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        data::variable_list parameters = i->variables2();
        if (m_process_parameters != data::variable_list() && m_process_parameters != parameters)
        {
          std::cerr << "fatal error in linear_process_conversion_visitor" << std::endl;
        }
        m_process_parameters = parameters;
        convert(*i);
      }
      linear_process lp(data::variable_list(), m_process_parameters, summand_list(result.begin(), result.end()));
      if (!tr::is_process(p.init().expression()))
      {
        std::cerr << "fatal error in linear_process_conversion_visitor" << std::endl;
      }
      process q = p.init().expression();
      process_initializer init(p.init().variables(), data::make_assignment_list(m_process_parameters, q.expressions()));
      return specification(p.data(), p.action_labels(), lp, init);
    }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LINEAR_PROCESS_CONVERSION_VISITOR_H
