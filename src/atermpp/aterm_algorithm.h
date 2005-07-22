// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_algorithm.h
// date          : 04/25/05
// version       : 0.3
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef ATERM_ALGORITHM_H
#define ATERM_ALGORITHM_H

/** @file
  */

#include <iostream>
#include <vector>
#include "atermpp/aterm.h"

namespace atermpp
{

// returns true if s is a subterm of t
inline
bool is_subterm(aterm_appl s, aterm_appl t)
{
  if (s == t)
    return true;

  if (t.function().arity() == 0)
    return false;

  aterm_list args = t.argument_list();
  for (aterm_list::iterator i = args.begin(); i != args.end(); ++i)
  {
    if (is_subterm(s, i->to_appl()))
      return true;
  }

  return false;
}

// recursively replaces src by dest in term t
inline
aterm_appl replace(aterm_appl t, aterm_appl src, aterm_appl dest)
{
  if (t == src)
    return dest;

  if (t.function().arity() == 0)
    return t;

  aterm_list args = t.argument_list();
  std::vector<aterm> v;
  for (aterm_list::iterator i = args.begin(); i != args.end(); ++i)
  {
    v.push_back(replace(i->to_appl(), src, dest));
  }

  aterm_appl result(t.function(), aterm_list(v.begin(), v.end()));
  if (result == src)
    return dest;
  else
    return result;
}

// non-recursively replaces src by dest in term t
inline
aterm_appl replace_non_recursive(aterm_appl t, aterm_appl src, aterm_appl dest)
{
  if (t == src)
    return dest;

  if (t.function().arity() == 0)
    return t;

  aterm_list args = t.argument_list();
  std::vector<aterm> v;
  for (aterm_list::iterator i = args.begin(); i != args.end(); ++i)
  {
    v.push_back(replace(i->to_appl(), src, dest));
  }

  aterm_appl result(t.function(), aterm_list(v.begin(), v.end()));
  return result;
}

// non-recursive multiple replace
template <typename Iter>
aterm_appl replace(aterm_appl t, Iter first, Iter last)
{
  for (Iter i = first; i != last; ++i)
  {
    if (t == i.src())
      return i.dest();
  }
  if (t.function().arity() == 0)
    return t;

  aterm_list args = t.argument_list();
  std::vector<aterm> v;
  for (aterm_list::iterator i = args.begin(); i != args.end(); ++i)
  {
    v.push_back(replace(i->to_appl(), first, last));
  }

  return aterm_appl(t.function(), aterm_list(v.begin(), v.end()));
}

} // namespace atermpp

#endif // ATERM_ALGORITHM_H
