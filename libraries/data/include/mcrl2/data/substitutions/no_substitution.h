// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/no_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_NO_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_NO_SUBSTITUTION_H

#include <iostream>

namespace mcrl2::data
{

/// \brief An empty struct that is used to denote the absence of a substitution.
/// Used for rewriters.
struct no_substitution
{
};

inline
std::ostream& operator<<(std::ostream& out, const no_substitution&)
{
  return out << "[]";
}

} // namespace mcrl2::data

#endif // MCRL2_DATA_SUBSTITUTIONS_NO_SUBSTITUTION_H
