// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_access.h
/// \brief Convenience functions for accessing the child nodes of aterm_appl and aterm_list. 

#ifndef MCRL2_ATERMPP_ATERM_ACCESS_H
#define MCRL2_ATERMPP_ATERM_ACCESS_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{
  /// \brief Returns the first child of t casted to an aterm_appl.
  /// \param t A term
  /// \return The first child of t casted to an aterm_appl.
  inline
  aterm_appl arg1(ATermAppl t)
  {
    return aterm_appl(t)(0);
  }
  
  /// \brief Returns the second child of t casted to an aterm_appl.
  /// \param t A term
  /// \return The second child of t casted to an aterm_appl.
  inline
  aterm_appl arg2(ATermAppl t)
  {
    return aterm_appl(t)(1);
  }
  
  /// \brief Returns the third child of t casted to an aterm_appl.
  /// \param t A term
  /// \return The third child of t casted to an aterm_appl.
  inline
  aterm_appl arg3(ATermAppl t)
  {
    return aterm_appl(t)(2);
  }
  
  /// \brief Returns the first child of t casted to an aterm_list.
  /// \param t A term
  /// \return The first child of t casted to an aterm_list.
  inline
  ATermList list_arg1(ATermAppl t)
  {
    return aterm_list(aterm_appl(t)(0));
  }
  
  /// \brief Returns the second child of t casted to an aterm_list.
  /// \param t A term
  /// \return The second child of t casted to an aterm_list.
  inline
  ATermList list_arg2(ATermAppl t)
  {
    return aterm_list(aterm_appl(t)(1));
  }
  
  /// \brief Returns the third child of t casted to an aterm_list.
  /// \param t A term
  /// \return The third child of t casted to an aterm_list.
  inline
  ATermList list_arg3(ATermAppl t)
  {
    return aterm_list(aterm_appl(t)(2));
  }

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_ACCESS_H
