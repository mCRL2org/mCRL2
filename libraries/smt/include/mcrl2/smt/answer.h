// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file answer.h

#ifndef MCRL2_SMT_ANSWER_H
#define MCRL2_SMT_ANSWER_H

#include <iostream>

namespace mcrl2::smt
{

enum answer
{
  UNSAT = 0,
  UNKNOWN = 1,
  SAT = 2
};

inline
std::ostream& operator<<(std::ostream& out, const answer& a)
{
  switch(a)
  {
    case smt::answer::SAT: return out << "sat";
    case smt::answer::UNSAT: return out << "unsat";
    case smt::answer::UNKNOWN: return out << "unknown";
    default: std::abort();
  }
}

} // namespace mcrl2::smt

#endif // MCRL2_SMT_ANSWER_H
