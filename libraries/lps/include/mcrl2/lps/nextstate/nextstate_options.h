// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/nextstate/nextstate_options.h

#ifndef MCRL2_LPS_NEXTSTATE_NEXTSTATE_OPTIONS_H
#define MCRL2_LPS_NEXTSTATE_NEXTSTATE_OPTIONS_H

#include <iostream>
#include <string>
#include "mcrl2/exception.h"

namespace mcrl2
{
namespace lps
{

/** \brief Internal NextState state storage method **/
typedef enum { GS_STATE_VECTOR  /** \brief Store state as vector (ATermAppl) **/
               , GS_STATE_TREE    /** \brief Store states in a binary tree **/
             } NextStateFormat;

inline
NextStateFormat parse_nextstate_format(const std::string& s)
{
  if(s == "vector")
  {
    return GS_STATE_VECTOR;
  }
  else if (s == "tree")
  {
    return GS_STATE_TREE;
  }
  else
  {
    throw mcrl2::runtime_error("unkown nextstate format " + s);
  }
}

inline
std::string print_nextstate_format(const NextStateFormat f)
{
  switch(f)
  {
    case GS_STATE_VECTOR:
      return "vector";
    case GS_STATE_TREE:
      return "tree";
    default:
    throw mcrl2::runtime_error("unknown nextstate format");
  }
}

inline
std::istream& operator>>(std::istream& is, NextStateFormat& f)
{
  try
  {
    std::string s;
    is >> s;
    f = parse_nextstate_format(s);
  }
  catch (mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const NextStateFormat f)
{
  os << print_nextstate_format(f);
  return os;
}

inline
std::string description(const NextStateFormat f)
{
  switch(f)
  {
    case GS_STATE_VECTOR:
      return "a vector (slightly faster, often far less memory efficient)";
    case GS_STATE_TREE:
      return "a tree (memory efficient)";
    default:
    throw mcrl2::runtime_error("unknown nextstate format");
  }
}

/** \brief Strategies for exploring the next states. **/
typedef enum { nsStandard } NextStateStrategy;

} // namespace lps
} // namespace mcrl2


#endif // MCRL2_LPS_NEXTSTATE_NEXTSTATE_OPTIONS_H
