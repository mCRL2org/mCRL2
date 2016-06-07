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

namespace detail
{

// helper function for linear_process_base::linear_process_base(const atermpp::aterm_appl& lps)
template <typename Summand>
Summand make_action_summand(const data::variable_list&,
                            const data::data_expression&,
                            const multi_action&,
                            const data::assignment_list&,
                            const stochastic_distribution&
                           )
{
  throw mcrl2::runtime_error("make_action_summand is not defined!");
}

template <>
inline
action_summand make_action_summand<action_summand>(const data::variable_list& summation_variables,
                                                   const data::data_expression& condition,
                                                   const multi_action& a,
                                                   const data::assignment_list& assignments,
                                                   const stochastic_distribution& distribution
                                                  )
{
  assert(!distribution.is_defined());
  return action_summand(summation_variables, condition, a, assignments);
}

} // namespace detail

class linear_process; // prototype declaration
bool check_well_typedness(const linear_process& proc);

class stochastic_linear_process; // prototype declaration
bool check_well_typedness(const stochastic_linear_process& proc);

template <typename ActionSummand>
class linear_process_base
{
  protected:
    /// \brief The process parameters of the process
    data::variable_list m_process_parameters;

    /// \brief The deadlock summands of the process
    deadlock_summand_vector m_deadlock_summands;

    /// \brief The action summands of the process
    std::vector<ActionSummand> m_action_summands;

  public:
    /// \brief The action summand type
    typedef ActionSummand action_summand_type;

    /// \brief Constructor.
    linear_process_base()
    { }

    /// \brief Copy constructor.
    linear_process_base(const linear_process_base<ActionSummand> &other) :
      m_process_parameters(other.m_process_parameters),
      m_deadlock_summands(other.m_deadlock_summands),
      m_action_summands(other.m_action_summands)
    { }

    /// \brief Constructor.
    linear_process_base(const data::variable_list& process_parameters,
                        const deadlock_summand_vector& deadlock_summands,
                        const std::vector<ActionSummand>& action_summands
                       )
       :
      m_process_parameters(process_parameters),
      m_deadlock_summands(deadlock_summands),
      m_action_summands(action_summands)
    { }

    /// \brief Constructor.
    /// \param lps A term
    linear_process_base(const atermpp::aterm_appl& lps, bool stochastic_distributions_allowed = true)
    {
      using atermpp::down_cast;
      assert(core::detail::check_term_LinearProcess(lps));
      m_process_parameters = down_cast<data::variable_list>(lps[0]);
      const atermpp::aterm_list& summands = atermpp::down_cast<atermpp::aterm_list>(lps[1]);
      for (const atermpp::aterm& summand: summands)
      {
        assert(core::detail::check_rule_LinearProcessSummand(summand));
        const atermpp::aterm_appl& t = down_cast<atermpp::aterm_appl>(summand);

        const data::variable_list& summation_variables = down_cast<data::variable_list>(t[0]);
        const data::data_expression& condition = down_cast<data::data_expression>(t[1]);
        const data::data_expression& time = down_cast<data::data_expression>(t[3]);
        const data::assignment_list& assignments = down_cast<data::assignment_list>(t[4]);
        const stochastic_distribution& distribution = down_cast<stochastic_distribution>(t[5]);
        if (!stochastic_distributions_allowed && distribution.is_defined())
        {
          throw mcrl2::runtime_error("Summand with stochastic distribution encountered, while this tool is not yet able to deal with stochastic distributions.");
        }
        if (down_cast<atermpp::aterm_appl>(t[2]).function() == core::detail::function_symbols::Delta)
        {
          m_deadlock_summands.push_back(deadlock_summand(summation_variables, condition, deadlock(time)));
        }
        else
        {
          assert(lps::is_multi_action(down_cast<atermpp::aterm_appl>(t[2])));
          process::action_list actions(down_cast<atermpp::aterm_list>(down_cast<atermpp::aterm_appl>(t[2])[0]));
          m_action_summands.push_back(detail::make_action_summand<ActionSummand>(summation_variables, condition, multi_action(actions, time), assignments, distribution));
        }
      }
    }

    /// \brief Returns the number of LPS summands.
    /// \return The number of LPS summands.
    size_t summand_count() const
    {
      return m_deadlock_summands.size() + m_action_summands.size();
    }

    /// \brief Returns the sequence of action summands.
    /// \return The sequence of action summands.
    const std::vector<ActionSummand>& action_summands() const
    {
      return m_action_summands;
    }

    /// \brief Returns the sequence of action summands.
    /// \return The sequence of action summands.
    std::vector<ActionSummand>& action_summands()
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
      for (auto i = m_action_summands.begin(); i != m_action_summands.end(); ++i)
      {
        if (i->has_time())
        {
          return true;
        }
      }
      for (auto i = m_deadlock_summands.begin(); i != m_deadlock_summands.end(); ++i)
      {
        if (i->deadlock().has_time())
        {
          return true;
        }
      }
      return false;
    }
};

/// \brief linear process.
class linear_process: public linear_process_base<action_summand>
{
  typedef linear_process_base<action_summand> super;

  public:
    /// \brief Constructor.
    linear_process()
    { }

    /// \brief Copy constructor.
    linear_process(const linear_process& other)
      : super(other)
    { }

    /// \brief Constructor.
    linear_process(const data::variable_list& process_parameters,
                   const deadlock_summand_vector& deadlock_summands,
                   const action_summand_vector& action_summands
                  )
      : super(process_parameters, deadlock_summands, action_summands)
    { }

    /// \brief Constructor.
    /// \param lps A term
    linear_process(const atermpp::aterm_appl& lps, bool = false)
      : super(lps, false)
    { }
};

/// \brief Conversion to aterm_appl.
/// \return The action summand converted to aterm format.
template <typename ActionSummand>
atermpp::aterm_appl linear_process_to_aterm(const linear_process_base<ActionSummand>& p)
{
  atermpp::term_list<atermpp::aterm_appl> summands;
  for (auto i = p.deadlock_summands().rbegin(); i != p.deadlock_summands().rend(); ++i)
  {
    atermpp::aterm_appl s = deadlock_summand_to_aterm(*i);
    summands.push_front(s);
  }
  for (auto i = p.action_summands().rbegin(); i != p.action_summands().rend(); ++i)
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
