// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_ALGORITHM_H
#define MCRL2_LPS_DETAIL_ALGORITHM_H

#include <string>
#include <iterator>
#include <vector>
#include <set>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/linear_process.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{

/// \brief Adds a time parameter t to s if needed and returns the result. The time t
/// is chosen such that it doesn't appear in context.
template <typename IdentifierGenerator>
struct make_timed_lps_summand
{
  IdentifierGenerator& m_generator;

  make_timed_lps_summand(IdentifierGenerator& generator)
    : m_generator(generator)
  {}

  /// \brief Function call operator
  /// \param summand_ A linear process summand
  /// \return The result of the function
  deprecated::summand operator()(deprecated::summand summand_) const
  {
    if (!summand_.has_time())
    {
      data::variable v(m_generator("T"), data::sort_real::real_());
      summand_ = set_time(summand_, data::data_expression(v));
      data::variable_list V(summand_.summation_variables());
      V = push_front(V, v);
      summand_ = set_summation_variables(summand_, V);
    }
    return summand_;
  }
};

/// \brief Adds time parameters to the lps if needed and returns the result.
/// The times are chosen such that they don't appear in context.
/// \param lps A linear process
/// \param context A term
/// \return A timed linear process
inline
linear_process make_timed_lps(const linear_process& lps, const std::set<core::identifier_string>& context)
{
  data::set_identifier_generator generator;
  generator.add_identifiers(context);
  deprecated::summand_list new_summands = atermpp::apply(deprecated::linear_process_summands(lps), make_timed_lps_summand<data::set_identifier_generator>(generator));
  linear_process result = lps;
  deprecated::set_linear_process_summands(result, new_summands);
  return result;
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_ALGORITHM_H
