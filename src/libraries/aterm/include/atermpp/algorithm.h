// ======================================================================
//
// Copyright (c) 2004, 2005, 2006 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/algorithm.h
// date          : 19-09-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file algorithm.h
/// Contains algorithms on aterm_lists and aterm_appls.

#ifndef ATERM_ALGORITHM_H
#define ATERM_ALGORITHM_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_list.h"
#include "atermpp/detail/algorithm_impl.h"

namespace atermpp
{
  /// Finds a subterm of t that matches the predicate op. If no matching subterm is found,
  /// aterm() is returned.
  template <typename UnaryPredicate>
  aterm find_if(aterm t, UnaryPredicate op)
  {
    try {
      detail::find_if_impl(t, op);
    }
    catch (detail::found_term_exception e) {
      return e.t;
    }
    return aterm();
  }

  /// Finds all subterms of t that match the predicate op, and writes the found terms
  /// to the destination range starting with destBegin.
  template <typename UnaryPredicate, typename OutputIterator>
  void find_all_if(aterm t, UnaryPredicate op, OutputIterator destBegin)
  {
    OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
    detail::find_all_if_impl(t, op, i);
  }

  /// Replaces each subterm in t that is equal to old_value with new_value.
  inline
  aterm replace(aterm t, aterm old_value, aterm new_value, bool recursive = false)
  {
    aterm result = t;
    if (t == old_value)
    {
      result = new_value;
    }
    else if (t.type() == AT_APPL)
    {
      aterm_appl a(t);
      if (a.function().arity() != 0)
      {
        aterm_list args = apply(a.argument_list(), detail::replace_substitution(old_value, new_value, recursive));
        if (recursive && args == old_value)
          args = new_value;
        result = aterm_appl(a.function(), args);
      }
    }
    else if (t.type() == AT_LIST)
    {
      aterm_list l(t);
      if (l.size() != 0)
      {
        result = apply(l, detail::replace_substitution(old_value, new_value, recursive));
      }
    }
    if (recursive && result == old_value)
      result = new_value; 
    return result;
  }

  /// Replaces each subterm in the aterm_appl src for which the unary predicate
  /// op holds with new_value.
  template <typename UnaryPredicate>
  aterm replace_if(aterm t, UnaryPredicate op, aterm new_value, bool recursive = false)
  {
    aterm result = t;   
    if (op(t))
    {
      result = new_value;
    }
    else if (t.type() == AT_APPL)
    {
      aterm_appl a(t);
      if (a.function().arity() != 0)
      {
        aterm_list args = apply(a.argument_list(), detail::replace_if_substitution<UnaryPredicate>(op, new_value, recursive));
        if (recursive && op(args))
          args = new_value;
        result = aterm_appl(a.function(), args);
      }
    }
    else if (t.type() == AT_LIST)
    {
      aterm_list l(t);
      if (l.size() != 0)
      {
        result = apply(l, detail::replace_if_substitution<UnaryPredicate>(op, new_value, recursive));
      }
    }
    if (recursive && op(result))
      result = new_value; 
    return result;
  }

} // namespace atermpp

#endif // ATERM_ALGORITHM_H
