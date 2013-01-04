// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/substitute.h
/// \brief Substitution of terms.

#ifndef MCRL2_ATERMPP_SUBSTITUTE_H
#define MCRL2_ATERMPP_SUBSTITUTE_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/algorithm.h"

namespace atermpp
{

/// \brief Utility class for applying a substitution to a term.
struct substitution
{
  /// A value.
  aterm m_src;

  /// A replacement.
  aterm m_dest;

  /// \brief Constructor.
  /// \param src A value.
  /// \param dest A replacement.
  substitution(const aterm &src, const aterm &dest)
    : m_src(src), m_dest(dest)
  {}

  /// \brief Function call operator.
  /// \param t A term.
  /// \return The term to which the substitution has been applied.
  aterm operator()(aterm t) const
  {
    return atermpp::replace(t, m_src, m_dest);
  }
};

/// Utility class for applying a substitution to a term.
template <typename Src, typename Dest>
struct list_substitution
{
  /// A sequence of values.
  Src m_src;

  /// A sequence of replacements.
  Dest m_dest;

  /// \brief Constructor.
  /// \param src A sequence of values.
  /// \param dest A sequence of replacements.
  list_substitution(Src src, Dest dest)
    : m_src(src), m_dest(dest)
  {
    assert(src.size() == dest.size());
  }

  /// \brief Function call operator.
  /// \param t A term.
  /// \return The term to which all substitutions have been applied.
  aterm operator()(aterm t) const
  {
    typename Src::const_iterator i;
    typename Dest::const_iterator j;
    for (i = m_src.begin(), j = m_dest.begin(); i != m_src.end(); ++i, ++j)
    {
      t = atermpp::replace(t, *i, *j);
    }
    return t;
  }
};

/// \brief Creation function for a list of substitutions.
/// \param src A sequence of values.
/// \param dest A sequence of replacements.
/// \return The corresponding list substitution.
template <typename Src, typename Dest>
list_substitution<Src, Dest> make_list_substitution(Src src, Dest dest)
{
  return list_substitution<Src, Dest>(src, dest);
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_SUBSTITUTE_H
