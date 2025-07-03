// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/assignment_sequence_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_ASSIGNMENT_SEQUENCE_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_ASSIGNMENT_SEQUENCE_SUBSTITUTION_H

#include "mcrl2/data/assignment.h"
#include "mcrl2/data/is_simple_substitution.h"

namespace mcrl2::data {

/// \brief Substitution that maps data variables to data expressions. The substitution is stored as an
/// assignment_list.
struct assignment_sequence_substitution
{
  using variable_type = variable;
  using expression_type = data_expression;
  using argument_type = variable;
  using result_type = data_expression;

  const assignment_list& assignments;

  explicit assignment_sequence_substitution(const assignment_list& assignments_)
    : assignments(assignments_)
  {}

  const data_expression& operator()(const variable& v) const
  {
    for (const assignment& a: assignments)
    {
      if (a.lhs() == v)
      {
        return a.rhs();
      }
    }
    return v;
  }
};

template <>
inline
bool is_simple_substitution(const assignment_sequence_substitution& sigma)
{
  for (const assignment& a: sigma.assignments)
  {
    if (!is_simple_substitution(a.lhs(), a.rhs()))
    {
      return false;
    }
  }
  return true;
}

} // namespace mcrl2::data



#endif // MCRL2_DATA_SUBSTITUTIONS_ASSIGNMENT_SEQUENCE_SUBSTITUTION_H
