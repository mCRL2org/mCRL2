// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/multi_action_name.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_MULTI_ACTION_NAME_H
#define MCRL2_PROCESS_MULTI_ACTION_NAME_H

#include "mcrl2/atermpp/aterm_io_text.h"
#include "mcrl2/core/identifier_string.h"

namespace mcrl2::process
{

/// \brief Represents the name of a multi action
struct multi_action_name: public std::multiset<core::identifier_string>
{
  using super = std::multiset<core::identifier_string>;

  multi_action_name() = default;

  template <typename InputIterator>
  multi_action_name(InputIterator first, InputIterator last)
    : super(first, last)
  {}
};

/// \brief Represents a set of multi action names
using multi_action_name_set = std::set<multi_action_name>;

/// \brief Pretty print function for a multi action name
inline
std::string pp(const multi_action_name& x)
{
  std::ostringstream out;
  if (x.empty())
  {
    out << "tau";
  }
  else
  {
    for (auto i = x.begin(); i != x.end(); ++i)
    {
      if (i != x.begin())
      {
        out << " | ";
      }
      out << *i;
    }
  }
  return out.str();
}

inline
std::ostream& operator<<(std::ostream& out, const multi_action_name& alpha)
{
  return out << pp(alpha);
}

/// \brief Pretty print function for a set of multi action names
inline
std::string pp(const multi_action_name_set& A)
{
  std::ostringstream out;
  out << "{";
  for (auto i = A.begin(); i != A.end(); ++i)
  {
    if (i != A.begin())
    {
      out << ", ";
    }
    out << pp(*i);
  }
  out << "}";
  return out.str();
}

inline
std::ostream& operator<<(std::ostream& out, const multi_action_name_set& A)
{
  return out << pp(A);
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_MULTI_ACTION_NAME_H
