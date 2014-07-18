// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linear_process.h
/// \brief The class linear_process.

#ifndef MCRL2_LPS_LINEAR_PROCESS_H
#define MCRL2_LPS_LINEAR_PROCESS_H

#include <string>
#include <cassert>
#include <algorithm>
#include <functional>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/action_summand.h"
#include "mcrl2/lps/deadlock_summand.h"
#include "mcrl2/lps/process_initializer.h"

namespace mcrl2
{

namespace lps
{

class linear_process; // prototype declaration
bool is_well_typed(const linear_process& proc);

///////////////////////////////////////////////////////////////////////////////
// linear_process
/// \brief linear process.
// <LinearProcess> ::= LinearProcess(<DataVarId>*, <LinearProcessSummand>*)
class linear_process
{
  friend atermpp::aterm_appl linear_process_to_aterm(const linear_process& p);
  protected:
    /// \brief The process parameters of the process
    data::variable_list m_process_parameters;

    /// \brief The deadlock summands of the process
    deadlock_summand_vector m_deadlock_summands;

    /// \brief The action summands of the process
    action_summand_vector m_action_summands;

  public:
    /// \brief Constructor.
    linear_process()
    {
    }

    /// \brief Copy constructor.
    linear_process(const linear_process &other) :
      m_process_parameters(other.m_process_parameters),
      m_deadlock_summands(other.m_deadlock_summands),
      m_action_summands(other.m_action_summands)
    {
    }

    /// \brief Constructor.
    linear_process(const data::variable_list& process_parameters,
                   const deadlock_summand_vector& deadlock_summands,
                   const action_summand_vector& action_summands
                  )
      :
      m_process_parameters(process_parameters),
      m_deadlock_summands(deadlock_summands),
      m_action_summands(action_summands)
    {
    }

    /// \brief Constructor.
    /// \param lps A term
    linear_process(const atermpp::aterm_appl& lps)
    {
      assert(core::detail::check_term_LinearProcess(lps));
      atermpp::aterm_appl::iterator i = lps.begin();
      m_process_parameters = data::variable_list(*i++);
      atermpp::aterm_list summands = atermpp::down_cast<atermpp::aterm_list>(*i);
      for (atermpp::aterm_list::iterator j = summands.begin(); j != summands.end(); ++j)
      {
        assert(core::detail::check_rule_LinearProcessSummand(*j));
        atermpp::aterm_appl t = atermpp::down_cast<atermpp::aterm_appl>(*j);

        data::variable_list summation_variables(atermpp::down_cast<atermpp::aterm_list>(t[0]));
        data::data_expression condition         = data::data_expression(t[1]);
        data::data_expression time              = data::data_expression(t[3]);
        data::assignment_list assignments(atermpp::down_cast<atermpp::aterm_list>(t[4]));
        if (atermpp::down_cast<atermpp::aterm_appl>(t[2]).function() == core::detail::function_symbols::Delta)
        {
          m_deadlock_summands.push_back(deadlock_summand(summation_variables, condition, deadlock(time)));
        }
        else
        {
          assert(lps::is_multi_action(atermpp::down_cast<const atermpp::aterm_appl>(t[2])));
          process::action_list actions(atermpp::down_cast<atermpp::aterm_list>(atermpp::down_cast<atermpp::aterm_appl>(t[2])[0]));
          m_action_summands.push_back(action_summand(summation_variables, condition, multi_action(actions, time), assignments));
        }
      }
    }

    /// \brief Destructor
    ~linear_process()
    {
    }

    /// \brief Returns the number of LPS summands.
    /// \return The number of LPS summands.
    size_t summand_count() const
    {
      return m_deadlock_summands.size() + m_action_summands.size();
    }

    /// \brief Returns the sequence of action summands.
    /// \return The sequence of action summands.
    const action_summand_vector& action_summands() const
    {
      return m_action_summands;
    }

    /// \brief Returns the sequence of action summands.
    /// \return The sequence of action summands.
    action_summand_vector& action_summands()
    {
      return m_action_summands;
    }

    /// \brief Returns the sequence of deadlock summands.
    /// \return The sequence of deadlock summands.
    const deadlock_summand_vector& deadlock_summands() const
    {
      return m_deadlock_summands;
    }

    /// \brief Returns the sequence of deadlock summands.
    /// \return The sequence of deadlock summands.
    deadlock_summand_vector& deadlock_summands()
    {
      return m_deadlock_summands;
    }

    /// \brief Returns the sequence of process parameters.
    /// \return The sequence of process parameters.
    const data::variable_list& process_parameters() const
    {
      return m_process_parameters;
    }

    /// \brief Returns the sequence of process parameters.
    /// \return The sequence of process parameters.
    data::variable_list& process_parameters()
    {
      return m_process_parameters;
    }

    /// \brief Returns true if time is available in at least one of the summands.
    /// \return True if time is available in at least one of the summands.
    bool has_time() const
    {
      for (action_summand_vector::const_iterator i = m_action_summands.begin(); i != m_action_summands.end(); ++i)
      {
        if (i->has_time())
        {
          return true;
        }
      }
      for (deadlock_summand_vector::const_iterator i = m_deadlock_summands.begin(); i != m_deadlock_summands.end(); ++i)
      {
        if (i->deadlock().has_time())
        {
          return true;
        }
      }
      return false;
    }
};

/// \brief Conversion to aterm_appl.
/// \return The action summand converted to aterm format.
inline
atermpp::aterm_appl linear_process_to_aterm(const linear_process& p)
{
  atermpp::term_list<atermpp::aterm_appl> summands;
  for (deadlock_summand_vector::const_reverse_iterator i = p.deadlock_summands().rbegin(); i != p.deadlock_summands().rend(); ++i)
  {
    atermpp::aterm_appl s = deadlock_summand_to_aterm(*i);
    summands.push_front(s);
  }
  for (action_summand_vector::const_reverse_iterator i = p.action_summands().rbegin(); i != p.action_summands().rend(); ++i)
  {
    atermpp::aterm_appl s = action_summand_to_aterm(*i);
    summands.push_front(s);
  }

  return atermpp::aterm_appl(core::detail::function_symbol_LinearProcess(),
           p.process_parameters(),
           summands
         );
}

//--- start generated class linear_process ---//
// prototype declaration
std::string pp(const linear_process& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const linear_process& x)
{
  return out << lps::pp(x);
}
//--- end generated class linear_process ---//

/// \brief Test for a linear_process expression
/// \param x A term
/// \return True if \a x is a linear process expression
inline
bool is_linear_process(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::LinearProcess;
}

// template function overloads
std::set<data::variable> find_all_variables(const lps::linear_process& x);
std::set<data::variable> find_free_variables(const lps::linear_process& x);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LINEAR_PROCESS_H
