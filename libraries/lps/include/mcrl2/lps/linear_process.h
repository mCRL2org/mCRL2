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
#include "mcrl2/lps/summand.h"
#include "mcrl2/lps/process_initializer.h"

namespace mcrl2
{

namespace lps
{

class linear_process; // prototype declaration

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

    /// \brief Set the summands of the linear process
    /// \deprecated
    void set_summands(const deprecated::summand_list& summands)
    {
      m_deadlock_summands.clear();
      m_action_summands  .clear();
      for (deprecated::summand_list::iterator j = summands.begin(); j != summands.end(); ++j)
      {
        if (j->is_delta())
        {
          m_deadlock_summands.push_back(deadlock_summand(j->summation_variables(), j->condition(), j->deadlock()));
        }
        else
        {
          m_action_summands.push_back(action_summand(j->summation_variables(), j->condition(), j->multi_action(), j->assignments()));
        }
      }
    }

    /// \brief Returns the sequence of LPS summands.
    /// \return The sequence of LPS summands.
    /// \deprecated
    deprecated::summand_list summands() const
    {
      deprecated::summand_list result;
      for (deadlock_summand_vector::const_reverse_iterator i = m_deadlock_summands.rbegin(); i != m_deadlock_summands.rend(); ++i)
      {
        deprecated::summand s = atermpp::aterm_appl(deadlock_summand_to_aterm(*i));
        result = atermpp::push_front(result, s);
      }
      for (action_summand_vector::const_reverse_iterator i = m_action_summands.rbegin(); i != m_action_summands.rend(); ++i)
      {
        deprecated::summand s = atermpp::aterm_appl(action_summand_to_aterm(*i));
        result = atermpp::push_front(result, s);
      }
      return result;
    }

  public:
    /// \brief Constructor.
    linear_process()
    {
      m_process_parameters.protect();
    }

    /// \brief Copy constructor.
    linear_process(const linear_process &other) :
      m_process_parameters(other.m_process_parameters),
      m_deadlock_summands(other.m_deadlock_summands),
      m_action_summands(other.m_action_summands)
    {
      m_process_parameters.protect();
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
      m_process_parameters.protect();
    }

    /// \brief Constructor.
    /// \param lps A term
    linear_process(atermpp::aterm_appl lps)
    {
      assert(core::detail::check_term_LinearProcess(lps));

      // unpack LPS(.,.,.) term
      atermpp::aterm_appl::iterator i = lps.begin();
      m_process_parameters = *i++;
      m_process_parameters.protect();
      set_summands(*i);
    }

    /// \brief Destructor
    ~linear_process()
    {
      m_process_parameters.unprotect();
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

/// \brief Conversion to ATermAppl.
/// \return The action summand converted to ATerm format.
inline
atermpp::aterm_appl linear_process_to_aterm(const linear_process& p)
{
  return core::detail::gsMakeLinearProcess(
           p.process_parameters(),
           p.summands()
         );
}

namespace deprecated
{
/// \brief Set the summands of the linear process
/// \deprecated
inline
void set_linear_process_summands(linear_process& p, const summand_list& summands)
{
  p.deadlock_summands().clear();
  p.action_summands().clear();
  for (summand_list::iterator j = summands.begin(); j != summands.end(); ++j)
  {
    if (j->is_delta())
    {
      p.deadlock_summands().push_back(deadlock_summand(j->summation_variables(), j->condition(), j->deadlock()));
    }
    else
    {
      p.action_summands().push_back(action_summand(j->summation_variables(), j->condition(), j->multi_action(), j->assignments()));
    }
  }
}

/// \brief Returns the sequence of LPS summands.
/// \return The sequence of LPS summands.
/// \deprecated
inline
summand_list linear_process_summands(const linear_process& p)
{
  summand_list result;
  for (deadlock_summand_vector::const_reverse_iterator i = p.deadlock_summands().rbegin(); i != p.deadlock_summands().rend(); ++i)
  {
    summand s = atermpp::aterm_appl(deadlock_summand_to_aterm(*i));
    result = atermpp::push_front(result, s);
  }
  for (action_summand_vector::const_reverse_iterator i = p.action_summands().rbegin(); i != p.action_summands().rend(); ++i)
  {
    summand s = atermpp::aterm_appl(action_summand_to_aterm(*i));
    result = atermpp::push_front(result, s);
  }
  return result;
}

} // namespace deprecated

// template function overloads
std::string pp(const linear_process& x);
std::set<data::variable> find_variables(const lps::linear_process& x);
std::set<data::variable> find_free_variables(const lps::linear_process& x);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LINEAR_PROCESS_H
