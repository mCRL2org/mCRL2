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
    data_variable m_variable;
  
    compare_assignment_lhs(const data_variable& variable)
      : m_variable(variable)
    {}
    
    /// \brief FUNCTION_DESCRIPTION
    /// \param a PARAM_DESCRIPTION
    /// \return RETURN_DESCRIPTION
    bool operator()(const std::pair<data_variable, data_variable>& a) const
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
    
    /// \brief FUNCTION_DESCRIPTION
    /// \param t A term
    /// \return RETURN_DESCRIPTION
    std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
    {
      if (!is_data_variable(t))
      {
        return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
      }

      data_variable v(t);
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
  
  /// \brief FUNCTION_DESCRIPTION
  /// Applies the assignments to the term t and returns the result.
  /// \param t PARAM_DESCRIPTION
  /// \return RETURN_DESCRIPTION
  atermpp::aterm operator()(atermpp::aterm t) const
  {
    return atermpp::partial_replace(t, sequence_substitution_helper(m_assignments));
  }
};

/// \brief FUNCTION_DESCRIPTION
/// \param c PARAM_DESCRIPTION
/// \return RETURN_DESCRIPTION
template <typename Container>
sequence_substitution<Container> make_sequence_substitution(const Container& c)
{
  return sequence_substitution<Container>(c);
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_SEQUENCE_SUBSTITUTION_H
