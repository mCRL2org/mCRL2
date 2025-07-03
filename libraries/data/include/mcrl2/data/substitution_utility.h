// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitution_utility.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTION_UTILITY_H
#define MCRL2_DATA_SUBSTITUTION_UTILITY_H

#include "mcrl2/data/variable.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"

namespace mcrl2::data {

/// \brief Adds assignments [v := e] to the substitution sigma for each variable in v.
template <typename VariableSequence, typename DataExpressionSequence>
inline
void add_assignments(data::mutable_indexed_substitution<>& sigma, const VariableSequence& v, const DataExpressionSequence& e)
{
  assert(v.size() <= e.size());
  auto vi = v.begin();
  auto ei = e.begin();
  for (; vi != v.end(); ++vi, ++ei)
  {
    sigma[*vi] = *ei;
  }
}

/// \brief Removes assignments to variables in v from the substitution sigma
template <typename VariableSequence>
inline
void remove_assignments(data::mutable_indexed_substitution<>& sigma, const VariableSequence& v)
{
  for (const data::variable& vi: v)
  {
    sigma[vi] = vi;
  }
}

} // namespace mcrl2::data



#endif // MCRL2_DATA_SUBSTITUTION_UTILITY_H
