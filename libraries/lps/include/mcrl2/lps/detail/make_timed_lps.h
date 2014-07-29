// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/make_timed_lps.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_MAKE_TIMED_LPS_H
#define MCRL2_LPS_DETAIL_MAKE_TIMED_LPS_H

#include <string>
#include "mcrl2/data/real.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/linear_process.h"

namespace mcrl2 {

namespace lps {

namespace detail {

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
      data::variable_vector v = data::variable_vector(s.summation_variables().begin(),s.summation_variables().end());
      v.push_back(t);
      s.summation_variables() = data::variable_list(v.begin(),v.end());
    }
  }

  /// \brief Adds time to the summand s (either an action or a deadlock summand)
  /// \param s An action summand
  void operator()(action_summand& s) const
  {
    if (!s.multi_action().has_time())
    {
      data::variable t(m_generator("T"), data::sort_real::real_());
      s.multi_action().time() = t;
      data::variable_vector v = data::variable_vector(s.summation_variables().begin(),s.summation_variables().end());
      v.push_back(t);
      s.summation_variables() = data::variable_list(v.begin(),v.end());
    }
  }
};

/// \brief Adds time parameters to the lps if needed and returns the result.
/// The times are chosen such that they don't appear in context.
/// \param lps A linear process
/// \param context A term
/// \return A timed linear process
inline
void make_timed_lps(linear_process& lps, const std::set<core::identifier_string>& context)
{
  data::set_identifier_generator generator;
  generator.add_identifiers(context);
  make_timed_lps_summand<data::set_identifier_generator> f(generator);
  for (action_summand_vector::iterator i = lps.action_summands().begin(); i != lps.action_summands().end(); ++i)
  {
    f(*i);
  }
  for (deadlock_summand_vector::iterator i = lps.deadlock_summands().begin(); i != lps.deadlock_summands().end(); ++i)
  {
    f(*i);
  }
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_MAKE_TIMED_LPS_H
