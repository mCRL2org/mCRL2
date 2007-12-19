// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_access.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_ATERM_ACCESS_H
#define MCRL2_ATERMPP_ATERM_ACCESS_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"

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

#endif // MCRL2_ATERMPP_ATERM_ACCESS_H
