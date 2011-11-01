// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print.cpp

#include "mcrl2/exception.h"
#include "mcrl2/core/detail/pp_deprecated.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/process/print.h"

namespace mcrl2 {

namespace core {

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
  else if (lps::is_multi_action(x))
  {
    return lps::pp(lps::multi_action(x));
  }
  else if (process::is_process_expression(x))
  {
    return process::pp(process::process_expression(x));
  }
  else if (process::is_process_identifier(x))
  {
    return process::pp(process::process_identifier(x));
  }
  else if (core::is_identifier_string(x))
  {
    return core::pp(x);
  }
  else if (core::detail::gsIsDataEqn(x))
  {
    // print as ATerm, since pretty printing does not work before type checking...
    return core::pp(x);
  }
  std::cerr << "Warning: encountered unknown term in pp_deprecated " << x << std::endl;
  return core::pp(x);
}

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
  else if (process::is_process_identifier(x.front()))
  {
    return process::pp(process::process_identifier_list(x));
  }
  std::cerr << "Warning: encountered unknown term in pp_deprecated " << x << std::endl;
  return core::pp(x);
}

} // namespace core

} // namespace mcrl2
