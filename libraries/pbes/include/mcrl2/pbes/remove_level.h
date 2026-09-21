// Author(s): Jan Friso Groote, XIAO Qi
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file remove_level.h
/// \brief BES variable remove level when generating a BES from a PBES.

#ifndef MCRL2_PBES_REMOVE_LEVEL_H
#define MCRL2_PBES_REMOVE_LEVEL_H

#include <string>
#include "mcrl2/utilities/exception.h"



namespace mcrl2::pbes_system
{

/// \brief BES variable remove level when generating a BES from a PBES.
enum class remove_level
{
  none,   // Do not remove bes variables.
  some,   // Remove bes variables that are not used, and of which
          // the rhs of its equation is not equal to true and false.
  all     // Remove all bes variables whenever they are not used in
          // any other equation.
};

inline
remove_level parse_remove_level(const std::string& s)
{
  if (s == "none")
  {
    return remove_level::none;
  }
  else if (s == "some")
  {
    return remove_level::some;
  }
  else if (s == "all")
  {
    return remove_level::all;
  }
  else
  {
    throw mcrl2::runtime_error("unknown bes variables remove level " + s);
  }
}

inline
std::string print_remove_level(const remove_level s)
{
  switch(s)
  {
    case remove_level::none: return "none";
    case remove_level::some: return "some";
    case remove_level::all: return "all";
  }
  throw mcrl2::runtime_error("unknown remove_level");
}

inline
std::istream& operator>>(std::istream& is, remove_level& level)
{
  try
  {
    std::string s;
    is >> s;
    level = parse_remove_level(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const remove_level s)
{
  os << print_remove_level(s);
  return os;
}

inline
std::string description(const remove_level s)
{
  switch(s)
  {
    case remove_level::none: return "never remove a generated bes variable and its equation. This can lead to excessive"
        " memory usage.";
    case remove_level::some: return "remove generated bes variables that do not occur anymore in the generated BES, except if"
        " the right hand side of its equation is true or false. The rhss of removed variables"
        " must have to be recalculated, when this bes variable is encountered again.";
    case remove_level::all: return "remove the equation for bes variables that do not occur anymore in generated boolean equation system."
        " This is quite memory efficient, but it can be very time consuming as the rhss of removed bes"
        " variables may have to be recalculated quite often.";
  }
  throw mcrl2::runtime_error("unknown remove level");
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_REMOVE_LEVEL_H
