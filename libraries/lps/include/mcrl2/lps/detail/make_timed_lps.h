// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/make_timed_lps.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_MAKE_TIMED_LPS_H
#define MCRL2_LPS_DETAIL_MAKE_TIMED_LPS_H

#include "mcrl2/lps/linear_process.h"





namespace mcrl2::lps::detail {

/// \brief Adds a time parameter t to s if needed and returns the result. The time t
/// is chosen such that it doesn't appear in context.
template <typename IdentifierGenerator>
struct make_timed_lps_summand
{
  IdentifierGenerator& m_generator;

  make_timed_lps_summand(IdentifierGenerator& generator)
    : m_generator(generator)
  {}

  /// \brief Adds time to the summand s (either an action or a deadlock summand)
  /// \param s An action summand
  void operator()(deadlock_summand& s) const
  {
    if (!s.deadlock().has_time())
    {
      data::variable t(m_generator("T"), data::sort_real::real_());
      s.deadlock().time() = t;
      s.summation_variables() = push_back(s.summation_variables(), t);
      /* data::variable_vector v = data::variable_vector(s.summation_variables().begin(),s.summation_variables().end());
      v.push_back(t);
      s.summation_variables() = data::variable_list(v.begin(),v.end()); */
    }
  }

  /// \brief Adds time to the summand s (either an action or a deadlock summand)
  /// \param s An action summand
  void operator()(action_summand& s) const
  {
    if (!s.multi_action().has_time())
    {
      data::variable t(m_generator("T"), data::sort_real::real_());
      s.multi_action() = multi_action(s.multi_action().actions(), t);
      s.summation_variables()=push_back(s.summation_variables(), t);
      /* data::variable_vector v = data::variable_vector(s.summation_variables().begin(),s.summation_variables().end());
      v.push_back(t);
      s.summation_variables() = data::variable_list(v.begin(),v.end());
      */
    }
  }
};

/// \brief Adds time parameters to the lps if needed and returns the result.
/// The times are chosen such that they don't appear in context.
/// \param lps A linear process
/// \param context A term
/// \return A timed linear process
template <class LINEAR_PROCESS>
void make_timed_lps(LINEAR_PROCESS& lps,
    const std::set<core::identifier_string>& context,
    std::enable_if_t<std::is_same_v<LINEAR_PROCESS, linear_process>
                     || std::is_same_v<LINEAR_PROCESS, stochastic_linear_process>>* = nullptr)
{
  data::set_identifier_generator generator;
  generator.add_identifiers(context);
  make_timed_lps_summand<data::set_identifier_generator> f(generator);
  for (action_summand& s: lps.action_summands())
  {
    f(s);
  }
  for (deadlock_summand& s: lps.deadlock_summands())
  {
    f(s);
  }
}

} // namespace mcrl2::lps::detail





#endif // MCRL2_LPS_DETAIL_MAKE_TIMED_LPS_H
