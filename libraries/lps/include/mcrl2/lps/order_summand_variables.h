// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/order_summand_variables.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_ORDER_SUMMAND_VARIABLES_H
#define MCRL2_LPS_ORDER_SUMMAND_VARIABLES_H

#include "mcrl2/lps/specification.h"

namespace mcrl2::lps
{

/// \brief Order summand variables to make enumeration over these variables more efficient.
template <typename Specification>
void order_summand_variables(Specification& lpsspec)
{
  for (auto& summand: lpsspec.process().action_summands())
  {
    summand.summation_variables() = data::order_variables_to_optimise_enumeration(summand.summation_variables(), lpsspec.data());
  }
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_ORDER_SUMMAND_VARIABLES_H
