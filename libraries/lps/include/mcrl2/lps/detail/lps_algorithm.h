///////////////////////////////////////////////////////////////////////////////
/// \file lps_algorithm.h
/// Contains algorithms on LPSs.

#ifndef LPS_ALGORITHM_H
#define LPS_ALGORITHM_H

#include <string>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/data.h"
#include "mcrl2/lps/data_utility.h"

namespace lps {
  
namespace detail {

using atermpp::aterm_appl;
using atermpp::aterm_list;

/// Adds a time parameter t to s if needed and returns the result. The time t
/// is chosen such that it doesn't appear in context.
struct make_timed_lps_summand
{
  fresh_variable_generator& m_generator;

  make_timed_lps_summand(fresh_variable_generator& generator)
    : m_generator(generator)
  {}

  summand operator()(summand summand_) const
  {
    if (!summand_.has_time())
    {
      data_variable v = m_generator();
      summand_ = set_time(summand_, data_expression(v));
      summand_ = set_summation_variables(summand_, summand_.summation_variables() + v);
    }
    return summand_;
  }
};

/// Adds time parameters to the lps if needed and returns the result. The times
/// are chosen such that they don't appear in context.
inline
linear_process make_timed_lps(linear_process lps, aterm context)
{
  fresh_variable_generator generator(context);
  summand_list new_summands = apply(lps.summands(), make_timed_lps_summand(generator));
  return set_summands(lps, new_summands);
}

} // namespace detail

} // namespace lps

#endif // LPS_ALGORITHM_H
