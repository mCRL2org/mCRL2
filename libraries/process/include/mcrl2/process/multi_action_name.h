// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/multi_action_name.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_MULTI_ACTION_NAME_H
#define MCRL2_PROCESS_MULTI_ACTION_NAME_H

#include <algorithm>
#include <set>
#include <sstream>
#include "mcrl2/core/identifier_string.h"

namespace mcrl2 {

namespace process {

/// \brief Represents the name of a multi action
struct multi_action_name: public std::multiset<core::identifier_string>
{
  typedef std::multiset<core::identifier_string> super;

  multi_action_name()
  {}

  template <typename InputIterator>
  multi_action_name(InputIterator first, InputIterator last)
    : super(first, last)
  {}
};

/// \brief Represents a set of multi action names
typedef std::set<multi_action_name> multi_action_name_set;

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
std::ostream& operator<<(std::ostream& out, multi_action_name& alpha)
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
std::ostream& operator<<(std::ostream& out, multi_action_name_set& A)
{
  return out << pp(A);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_MULTI_ACTION_NAME_H
