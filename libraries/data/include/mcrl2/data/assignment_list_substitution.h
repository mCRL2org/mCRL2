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
#include "mcrl2/data/sequence_substitution.h"

namespace mcrl2 {

namespace data {

/// \cond INTERNAL_DOCS
namespace detail {
  template< >
  struct assignment_helper< assignment >
  {
    typedef variable        left_type;
    typedef data_expression right_type;

    static variable left(assignment const& a)
    {
      return a.lhs();
    }

    static data_expression right(assignment const& a)
    {
      return a.rhs();
    }
  };
}

///////////////////////////////////////////////////////////////////////////////
// assignment_list_substitution
/// \brief Sequence of data assignments.
// A linear search is done in the list of assignments.
// Note that a data_assigment_list doesn't allow for an efficient implementation.
class assignment_list_substitution : public sequence_substitution< atermpp::term_list< assignment >, textual_substitution >
{
  friend class sequence_substitution< atermpp::term_list< assignment >, textual_substitution >;

  private:

    assignment_list_substitution& operator=(const assignment_list_substitution&)
    {
      return *this;
    }

  public:

    /// \brief Constructor.
    /// \param assignments A sequence of assignments to data variables
    assignment_list_substitution(const assignment_list& assignments)
      : sequence_substitution< atermpp::term_list< assignment >, textual_substitution >(assignments)
    {}
};

} // namespace data

} // namespace mcrl2

#endif

