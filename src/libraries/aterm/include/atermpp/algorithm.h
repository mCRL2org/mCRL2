// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
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

namespace atermpp
{
  /// INTERNAL ONLY
  // used to abort the recursive find
  struct found_term_exception
  {
    aterm t;

    found_term_exception(aterm t_)
      : t(t_)
    {}
  };
  
  /// INTERNAL ONLY
  template <typename UnaryPredicate>
  void find_if_impl(aterm t, UnaryPredicate op)
  {
    if (op(t))
      throw found_term_exception(t);

    // determine child nodes
    aterm_list children;
    if (t.type() == AT_LIST) {
      children = t;
    }
    else if (t.type() == AT_APPL) {
      children = aterm_appl(t).argument_list();
    }
    else {
      return;
    }

    for (aterm_list::iterator i = children.begin(); i != children.end(); ++i)
    {
      find_if_impl(*i, op);
    }
  }

  /// Finds a subterm of t that matches the predicate op. If no matching subterm is found,
  /// aterm() is returned.
  template <typename UnaryPredicate>
  aterm find_if(aterm t, UnaryPredicate op)
  {
    try {
      find_if_impl(t, op);
    }
    catch (found_term_exception e) {
      return e.t;
    }
    return aterm();
  }

  /// INTERNAL ONLY
  template <typename UnaryPredicate, typename OutputIterator>
  void find_all_if_impl(aterm t, UnaryPredicate op, OutputIterator& destBegin)
  {
    aterm_list arguments;
    if (t.type() == AT_LIST) {
      arguments = t;
    }
    else if (t.type() == AT_APPL) {
      if (op(aterm_appl(t))) // only check elements of type aterm_appl...
        destBegin++ = aterm_appl(t);
      arguments = aterm_appl(t).argument_list();
    }
    else {
      return;
    }
    for (aterm_list::iterator i = arguments.begin(); i != arguments.end(); ++i)
    {
      find_all_if_impl(*i, op, destBegin);
    }
  }

  /// Finds all subterms of t that match the predicate op, and writes the found terms
  /// to the destination range starting with destBegin.
  template <typename UnaryPredicate, typename OutputIterator>
  void find_all_if(aterm t, UnaryPredicate op, OutputIterator destBegin)
  {
    OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
    find_all_if_impl(t, op, i);
  }

} // namespace atermpp

#endif // ATERM_ALGORITHM_H
