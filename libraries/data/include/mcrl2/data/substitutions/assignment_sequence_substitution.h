// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/assignment_sequence_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_ASSIGNMENT_SEQUENCE_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_ASSIGNMENT_SEQUENCE_SUBSTITUTION_H

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

/// \brief Substitution that maps data variables to data expressions. The substitution is stored as an
/// assignment_list.
struct assignment_sequence_substitution: public std::unary_function<variable, data_expression>
{
  typedef variable variable_type;
  typedef data_expression expression_type;

  const assignment_list& assignments;

  assignment_sequence_substitution(const assignment_list& assignments_)
    : assignments(assignments_)
  {}

  const data_expression& operator()(const variable& v) const
  {
    for (auto i = assignments.begin(); i != assignments.end(); ++i)
    {
      if (i->lhs() == v)
      {
        return i->rhs();
      }
    }
    return v;
  }
};

template <>
inline
bool is_simple_substitution(const assignment_sequence_substitution& sigma)
{
  for (auto i = sigma.assignments.begin(); i != sigma.assignments.end(); ++i)
  {
    if (!is_simple_substitution(i->lhs(), i->rhs()))
    {
      return false;
    }
  }
  return true;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTIONS_ASSIGNMENT_SEQUENCE_SUBSTITUTION_H
