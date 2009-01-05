// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/identifier_string.h
/// \brief ATerm representations of identifier strings.

#ifndef MCRL2_BASIC_IDENTIFIER_STRING_H
#define MCRL2_BASIC_IDENTIFIER_STRING_H

#include "mcrl2/atermpp/aterm_string.h"

namespace mcrl2 {
  namespace core {

  /// \brief String type of the LPS library.
  /// Identifier strings are represented internally as ATerms.
  typedef atermpp::aterm_string identifier_string;
  
  /// \brief Tests if a term is an identifier string.
  /// \param t A term
  /// \return Whether t is an identifier string.
  inline
  bool is_identifier_string(atermpp::aterm t)
  {
    return t.type() == AT_APPL && atermpp::aterm_appl(t).size() == 0;
  }

  }
} 

#endif // MCRL2_BASIC_IDENTIFIER_STRING_H
