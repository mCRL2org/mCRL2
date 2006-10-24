// ======================================================================
//
// Copyright (c) 2006 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/detail/utility.h
// date          : 24-10-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef ATERM_DETAIL_UTILITY_H
#define ATERM_DETAIL_UTILITY_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_list.h"

namespace atermpp {

  namespace detail {
    
    inline
    ATermAppl str2appl(std::string s)
    {
      return ATmakeAppl0(ATmakeAFun(s.c_str(), 0, ATtrue));
    }

  } // namespace detail

} // namespace atermpp

#endif // ATERM_DETAIL_UTILITY_H
