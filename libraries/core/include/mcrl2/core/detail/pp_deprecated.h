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

#include "mcrl2/exception.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/process/print.h"

namespace mcrl2 {

namespace core {

inline
std::string pp_deprecated(const atermpp::aterm_appl& x)
{
  if (data::is_data_expression(x))
  {
    return data::pp(data::data_expression(x));
  }
  else if (data::is_sort_expression(x))
  {
    return data::pp(data::sort_expression(x));
  }
  else if (lps::is_action(x))
  {
    return lps::pp(lps::action(x));
  }
  else if (lps::is_action_label(x))
  {
    return lps::pp(lps::action_label(x));
  }
  else if (process::is_process_expression(x))
  {
    return process::pp(process::process_expression(x));
  }
  throw mcrl2::runtime_error("pp_deprecated: encountered unknown term " + x.to_string());
  return "";
}

inline
std::string pp_deprecated(const atermpp::aterm_list& x)
{
  if (x.empty())
  {
    return "[]";
  }
  if (data::is_data_expression(x.front()))
  {
    return data::pp(data::data_expression_list(x));
  }
  else if (data::is_sort_expression(x.front()))
  {
    return data::pp(data::sort_expression_list(x));
  }
  else if (lps::is_action(x.front()))
  {
    return lps::pp(lps::action_list(x));
  }
  else if (lps::is_action_label(x.front()))
  {
    return lps::pp(lps::action_label_list(x));
  }
  else if (process::is_process_expression(x.front()))
  {
    return process::pp(process::process_expression_list(x));
  }
  throw mcrl2::runtime_error("pp_deprecated: encountered unknown term " + x.to_string());
  return "";
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_PP_DEPRECATED_H
