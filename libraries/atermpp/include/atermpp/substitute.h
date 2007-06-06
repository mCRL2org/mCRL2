// ======================================================================
//
// Copyright (c) 2004, 2005, 2006 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/substitute.h
// date          : 01-10-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file substitute.h

#ifndef ATERM_SUBSTITUTE_H
#define ATERM_SUBSTITUTE_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_list.h"
#include "atermpp/algorithm.h"

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

#endif // ATERM_SUBSTITUTE_H
