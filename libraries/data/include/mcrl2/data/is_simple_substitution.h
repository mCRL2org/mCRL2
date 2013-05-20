// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/is_simple_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_IS_SIMPLE_SUBSTITUTION_H
#define MCRL2_DATA_IS_SIMPLE_SUBSTITUTION_H

namespace mcrl2 {

namespace data {

/// \brief Returns true if the substitution sigma satisfies the property that FV(sigma(x)) is included in {x} for all variables x.
/// N.B. The default return value is true, so a template specialization is required to enable this check for substitutions.
template <typename Substitution>
bool is_simple_substitution(const Substitution& sigma)
{
  return true;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_IS_SIMPLE_SUBSTITUTION_H
