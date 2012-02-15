// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/multi_action_print.h
/// \brief Custom print function for multi actions

#ifndef MCRL2_LPS_DETAIL_MULTI_ACTION_PRINT_H
#define MCRL2_LPS_DETAIL_MULTI_ACTION_PRINT_H

#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/print.h"

namespace mcrl2 {

namespace lps {

namespace detail {

/// \brief Returns a string representation of a multi action
inline
std::string multi_action_print(const multi_action& x)
{
  std::string result;
  if (x.actions().size()==0)
  {
    result="tau";
  }
  else
  {
    if (x.has_time() && x.actions().size()>1)
    {
      result="(";
    }
    for (action_list::const_iterator i=x.actions().begin(); i!=x.actions().end(); ++i)
    {
      result = result + lps::pp(*i);
      action_list::const_iterator i_next=i;
      i_next++;
      if (i_next!=x.actions().end())
      {
        result=result + "|";
      }
    }
  }
  if (x.has_time())
  {
    if (x.actions().size()>1)
    {
      result=result+")";
    }
    result=result+("@ " + data::pp(x.time()));
  }
  return result;
}


} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_MULTI_ACTION_PRINT_H
