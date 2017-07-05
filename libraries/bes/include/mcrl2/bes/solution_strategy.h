// Author(s): Jeroen Keiren.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file solution_strategy.h
/// \brief Strategies to solve a BES.

#ifndef MCRL2_BES_SOLUTION_STRATEGY_H
#define MCRL2_BES_SOLUTION_STRATEGY_H

#include <iostream>
#include <fstream>
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{

namespace bes
{

typedef enum { gauss, small_progr_measures, local_fixed_point } solution_strategy_t;

static
std::string solution_strategy_to_string(const solution_strategy_t s)
{
  switch (s)
  {
    case gauss:
      return "gauss";
      break;
    case small_progr_measures:
      return "spm";
      break;
    case local_fixed_point:
      return "lf";
      break;
  }
  throw mcrl2::runtime_error("unknown solution strategy");
}

static
std::ostream& operator<<(std::ostream& os, const solution_strategy_t s)
{
  os << solution_strategy_to_string(s);
  return os;
}

static
solution_strategy_t parse_solution_strategy(const std::string& s)
{
  if (s == "gauss")
  {
    return gauss;
  }
  else if (s == "spm")
  {
    return small_progr_measures;
  }
  else if (s == "lf")
  {
    return local_fixed_point;
  }
  else
  {
    throw mcrl2::runtime_error("unsupported solution strategy '" + s + "'");
  }
}

static
std::istream& operator>>(std::istream& is, solution_strategy_t& s)
{
  try
  {
    std::string str;
    is >> str;
    s = parse_solution_strategy(str);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

static
std::string description(const solution_strategy_t s)
{
  switch (s)
  {
    case gauss:
      return "Gauss elimination (inefficient; plain implementation)";
      break;
    case small_progr_measures:
      return "Small progress measures";
      break;
    case local_fixed_point:
      return "Local fixpoints (advanced form of Gauss elimination, especially effective without alternating fixed points)";
      break;
  }
  throw mcrl2::runtime_error("unknown solution strategy");
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_SOLUTION_STRATEGY_H
