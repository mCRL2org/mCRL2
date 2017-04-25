// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/print_utility.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_PRINT_UTILITY_H
#define MCRL2_CORE_DETAIL_PRINT_UTILITY_H

#include <iostream>
#include <sstream>
#include <string>

#include "mcrl2/core/print.h"

namespace mcrl2
{

namespace core
{

namespace detail
{

/// \brief Creates a string representation of a container using the pp pretty print function.
/// \param v A container
/// \param message A string
/// \param print_index If true, an index is written in front of each term
template <typename Container>
std::string print_container(const Container& v, const std::string& begin_marker = "(", const std::string& end_marker = ")", const std::string& message = "", bool print_index = false, bool boundary_spaces = true)
{
  std::ostringstream out;
  if (!message.empty())
  {
    out << "--- " << message << "---" << std::endl;
  }
  out << begin_marker;
  if (boundary_spaces)
  {
    out << " ";
  }
  int index = 0;
  for (typename Container::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    if (print_index)
    {
      out << index++ << " ";
      out << *i;
      out << std::endl;
    }
    else
    {
      if (i != v.begin())
      {
        out << ", ";
      }
      out << *i;
    }
  }
  if (boundary_spaces)
  {
    out << " ";
  }
  out << end_marker;
  return out.str();
}

/// \brief Creates a string representation of a container.
/// \param v A container
/// \param message A string
/// \param print_index If true, an index is written in front of each term
template <typename Container>
std::string print_list(const Container& v, const std::string& message = "", bool print_index = false, bool boundary_spaces = true)
{
  return print_container(v, "[", "]", message, print_index, boundary_spaces);
}

/// \brief Creates a string representation of a container.
/// \param v A container
/// \param message A string
/// \param print_index If true, an index is written in front of each term
template <typename Container>
std::string print_set(const Container& v, const std::string& message = "", bool print_index = false, bool boundary_spaces = true)
{
  return print_container(v, "{", "}", message, print_index, boundary_spaces);
}

/// \brief Creates a string representation of a map
/// \param v A map container
/// \param message A string
/// \param print_index If true, an index is written in front of each term
template <typename MapContainer>
std::string print_map(const MapContainer& v, const std::string& message = "")
{
  std::ostringstream out;
  if (!message.empty())
  {
    out << "--- " << message << "---" << std::endl;
  }
  out << "{";
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    if (i != v.begin())
    {
      out << ", ";
    }
    out << i->first << " -> " << i->second;
  }
  out << "}";
  return out.str();
}

/// \brief Prints a comma separated list of the elements of v. If v is empty, the empty string is returned.
/// \param v A container
template <typename Container>
std::string print_arguments(const Container& v)
{
  if (v.empty())
  {
    return "";
  }
  return print_container(v, "(", ")", "", false, false);
}

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_PRINT_UTILITY_H
