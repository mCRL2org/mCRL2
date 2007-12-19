// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/utility.h
/// \brief Add your file description here.

// ======================================================================
//
// Copyright (c) 2006 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : mcrl2/atermpp/detail/utility.h
// date          : 24-10-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef ATERM_DETAIL_UTILITY_H
#define ATERM_DETAIL_UTILITY_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"

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
