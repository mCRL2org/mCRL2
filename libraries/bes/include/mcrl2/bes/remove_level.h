// Author(s): Jan Friso Groote, XIAO Qi
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file remove_level.h
/// \brief BES variable remove level when generating a BES from a PBES.

#ifndef MCRL2_BES_REMOVE_LEVEL_H
#define MCRL2_BES_REMOVE_LEVEL_H

#include <string>
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{

namespace bes
{

/// \brief BES variable remove level when generating a BES from a PBES.
enum remove_level
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
  if (s == "none") return none;
  else if (s == "some") return some;
  else if (s == "all") return all;
  else throw mcrl2::runtime_error("unknown bes variables remove level " + s);
}

inline
std::string print_remove_level(const remove_level s)
{
  switch(s)
  {
    case none: return "none";
    case some: return "some";
    case all: return "all";
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
    case none: return "do not remove generated bes variables. This can lead to excessive"
        " usage of memory.";
    case some: return "remove generated bes variables that are not used, except if"
        " the right hand side of its equation is true or false. The rhss of variables"
        " must have to be recalculated, if encountered again, which is quite normal.";
    case all: return "remove every bes variable that is not used anymore in any equation."
        " This is quite memory efficient, but it can be very time consuming as the rhss of removed bes"
        " variables may have to be recalculated quite often.";
  }
  throw mcrl2::runtime_error("unknown remove level");
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_REMOVE_LEVEL_H
