// ======================================================================
//
// Copyright (c) 2006 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_access.h
// date          : 05-10-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm_access.h
/// Contains utility functions for retrieving subterms of aterm_appl's.

#ifndef ATERM_ACCESS_H
#define ATERM_ACCESS_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_list.h"

namespace atermpp
{
//  using atermpp::aterm_appl;
//  using atermpp::aterm_list;

  /// Returns the first child of t casted to an aterm_appl.
  inline
  aterm_appl arg1(ATermAppl t)
  {
    return aterm_appl(t)(0);
  }
  
  /// Returns the second child of t casted to an aterm_appl.
  inline
  aterm_appl arg2(ATermAppl t)
  {
    return aterm_appl(t)(1);
  }
  
  /// Returns the third child of t casted to an aterm_appl.
  inline
  aterm_appl arg3(ATermAppl t)
  {
    return aterm_appl(t)(2);
  }
  
  /// Returns the first child of t casted to an aterm_list.
  inline
  ATermList list_arg1(ATermAppl t)
  {
    return aterm_list(aterm_appl(t)(0));
  }
  
  /// Returns the second child of t casted to an aterm_list.
  inline
  ATermList list_arg2(ATermAppl t)
  {
    return aterm_list(aterm_appl(t)(1));
  }
  
  /// Returns the third child of t casted to an aterm_list.
  inline
  ATermList list_arg3(ATermAppl t)
  {
    return aterm_list(aterm_appl(t)(2));
  }

} // namespace atermpp

#endif // ATERM_ACCESS_H
