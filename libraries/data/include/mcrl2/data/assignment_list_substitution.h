// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/assignment_list_substitution.h
/// \brief The class data_assignment.

#ifndef MCRL2_NEWDATA_DATA_ASSIGNMENT_H
#define MCRL2_NEWDATA_DATA_ASSIGNMENT_H

#include <utility>

#include "mcrl2/data/variable.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2 {

namespace data {

///////////////////////////////////////////////////////////////////////////////
// assignment_list_substitution
/// \brief Sequence of data assignments.
// A linear search is done in the list of assignments.
// Note that a data_assigment_list doesn't allow for an efficient implementation.
class assignment_list_substitution: public substitution< assignment_list_substitution >
{
  friend class substitution< assignment_list_substitution, variable_type, expression_type >;

  private:

    /// \brief An assignment list.
    const assignment_list& m_assignments;

    /// \brief Applies the assignments to a variable v and returns the result.
    /// \param v A term
    /// \return The application of the assignments to the term.
    data_expression apply(variable const& v) const
    {
      for (assignment_list::const_iterator i = m_assignments.begin(); i != m_assignments.end(); ++i)
      {
        if (i->lhs() == v)
        {
          return i->rhs();
        }
      }

      return v;
    }

    assignment_list_substitution& operator=(const assignment_list_substitution&)
    {
      return *this;
    }

  public:

    /// \brief Constructor.
    /// \param assignments A sequence of assignments to data variables
    assignment_list_substitution(const assignment_list& assignments)
      : m_assignments(assignments)
    {}
};

} // namespace data

} // namespace mcrl2

#endif

