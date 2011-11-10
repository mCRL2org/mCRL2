// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/find.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_FIND_H
#define MCRL2_DATA_DETAIL_FIND_H

#include "mcrl2/data/variable.h"

namespace mcrl2 {

namespace data {

namespace detail {

/// \brief Returns the names of a set of data variables.
/// \param variables A set of data variables
inline
std::set<core::identifier_string> variable_names(const std::set<data::variable>& variables)
{
  std::set<core::identifier_string> result;
  for (std::set<variable>::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    result.insert(i->name());
  }
  return result;
}

/// \brief Returns the names of a set of data variables as a set of strings.
/// \param variables A set of data variables
inline
std::set<std::string> variable_name_strings(const std::set<data::variable>& variables)
{
  std::set<std::string> result;
  for (std::set<variable>::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    result.insert(std::string(i->name()));
  }
  return result;
}

/// \brief Returns the names of a set of data variables.
/// \param variables A set of data variables
inline
std::set<std::string> variable_name_strings(const std::set<data::variable>& variables1, const std::set<data::variable>& variables2)
{
  std::set<std::string> result;
  for (std::set<variable>::const_iterator i = variables1.begin(); i != variables1.end(); ++i)
  {
    result.insert(std::string(i->name()));
  }
  for (std::set<variable>::const_iterator i = variables2.begin(); i != variables2.end(); ++i)
  {
    result.insert(std::string(i->name()));
  }
  return result;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_FIND_H
