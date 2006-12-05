///////////////////////////////////////////////////////////////////////////////
/// \file lpe_algorithm.h
/// Contains algorithms on LPEs.

#ifndef LPE_ALGORITHM_H
#define LPE_ALGORITHM_H

#include <string>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "lpe/lpe.h"
#include "lpe/data.h"
#include "lpe/data_utility.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_list;

/// Adds a time parameter t to s if needed and returns the result. The time t
/// is chosen such that it doesn't appear in context.
struct make_timed_lpe_summand
{
  fresh_variable_generator& m_generator;

  make_timed_lpe_summand(fresh_variable_generator& generator)
    : m_generator(generator)
  {}

  LPE_summand operator()(LPE_summand summand) const
  {
    if (!summand.has_time())
    {
      data_variable v = m_generator();
      summand = set_time(summand, data_expression(v));
      summand = set_summation_variables(summand, summand.summation_variables() + v);
    }
    return summand;
  }
};

/// Adds time parameters to the lpe if needed and returns the result. The times
/// are chosen such that they don't appear in context.
inline
LPE make_timed_lpe(LPE lpe, aterm context)
{
  fresh_variable_generator generator(context);
  summand_list new_summands = apply(lpe.summands(), make_timed_lpe_summand(generator));
  return set_summands(lpe, new_summands);
};

} // namespace lpe

#endif // LPE_ALGORITHM_H
