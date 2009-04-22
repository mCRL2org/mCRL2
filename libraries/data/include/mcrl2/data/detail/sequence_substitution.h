// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sequence_substitution.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_SEQUENCE_SUBSTITUTION_H
#define MCRL2_DATA_DETAIL_SEQUENCE_SUBSTITUTION_H

#include <algorithm>
#include <utility>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data.h"

namespace mcrl2 {

namespace data {

namespace detail {

///////////////////////////////////////////////////////////////////////////////
// sequence_substitution
/// \brief Utility class for applying a sequence of replacements of
/// data variables, that are stored as pairs in a container.
/// Can be used in the replace algorithms of the atermpp library.
template <typename Container>
struct sequence_substitution
{
  const Container& m_assignments;

  struct compare_assignment_lhs
  {
    variable m_variable;

    compare_assignment_lhs(const variable& variable)
      : m_variable(variable)
    {}

    /// \brief Function call operator
    /// \param a A pair of data variables
    /// \return The function result
    bool operator()(const std::pair<variable, variable>& a) const
    {
      return m_variable == a.first;
    }
  };

  struct sequence_substitution_helper
  {
    const Container& m_assignments;

    sequence_substitution_helper(const Container& assignments)
      : m_assignments(assignments)
    {}

    /// \brief Function call operator
    /// \param t A term
    /// \return The function result
    std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
    {
      if (!data_expression(t).is_variable())
      {
        return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
      }

      variable v(t);
      typename Container::const_iterator i = std::find_if(m_assignments.begin(), m_assignments.end(), compare_assignment_lhs(v));
      if (i != m_assignments.end())
      {
        return std::pair<atermpp::aterm_appl, bool>(i->second, false); // false means: don't continue the recursion
      }
      else
      {
        return std::pair<atermpp::aterm_appl, bool>(t, false);
      }
    }
  };

  sequence_substitution(const Container& assignments)
    : m_assignments(assignments)
  {}

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  atermpp::aterm operator()(atermpp::aterm t) const
  {
    return atermpp::partial_replace(t, sequence_substitution_helper(m_assignments));
  }
};

/// \brief Convenience function for creating a sequence_substitution
/// \param c A container
/// \return A sequence_substitution
template <typename Container>
sequence_substitution<Container> make_sequence_substitution(const Container& c)
{
  return sequence_substitution<Container>(c);
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_SEQUENCE_SUBSTITUTION_H
