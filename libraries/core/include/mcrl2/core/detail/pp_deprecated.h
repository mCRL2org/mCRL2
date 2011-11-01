// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/pp_deprecated.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_PP_DEPRECATED_H
#define MCRL2_CORE_DETAIL_PP_DEPRECATED_H

#include <string>
#include "mcrl2/atermpp/aterm_appl.h"

namespace mcrl2 {

namespace core {

std::string pp_deprecated(const atermpp::aterm_appl& x);

std::string pp_deprecated(const atermpp::aterm_list& x);

inline
std::string pp_deprecated(aterm::ATerm x)
{
  if (aterm::ATgetType(x) == aterm::AT_APPL)
  {
    pp_deprecated((aterm::ATermAppl) x);
  }
  else if (aterm::ATgetType(x) == aterm::AT_LIST)
  {
    pp_deprecated((aterm::ATermList) x);
  }
  // TODO: MultAct([Action(ActId("a",[]),[])]) is not recognized as an ATermAppl, so we force it...
  return pp_deprecated((ATermAppl) x);
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_PP_DEPRECATED_H
