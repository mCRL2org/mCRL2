// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/substitute.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_SUBSTITUTE_H
#define MCRL2_ATERMPP_SUBSTITUTE_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/algorithm.h"

namespace atermpp
{

/// Utility class for applying a substitution to a term.
struct substitution
{
  aterm m_src;
  aterm m_dest;
  
  substitution(aterm src, aterm dest)
    : m_src(src), m_dest(dest)
  {}
  
  aterm operator()(aterm t) const
  {
    return atermpp::replace(t, m_src, m_dest);
  }
};

/// Creation function for substitutions.
template <typename Src, typename Dest>
inline
substitution make_substitution(Src src, Dest dest)
{
  return substitution(aterm_traits<Src>::term(src), aterm_traits<Dest>::term(dest));
}

/// Utility class for applying a substitution to a term.
template <typename Src, typename Dest>
struct list_substitution
{
  Src m_src;
  Dest m_dest;
  
  list_substitution(Src src, Dest dest)
    : m_src(src), m_dest(dest)
  {
    assert(src.size() == dest.size());
  }
  
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

/// Creation function for a list of substitutions.
template <typename Src, typename Dest>
list_substitution<Src, Dest> make_list_substitution(Src src, Dest dest)
{
  return list_substitution<Src, Dest>(src, dest);
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_SUBSTITUTE_H
