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

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"

namespace mcrl2 {

namespace data {

/// \brief Returns \c true if the substitution \a sigma satisfies the property that
/// <tt>FV(sigma(x))</tt> is included in <tt>{x}</tt> for all variables x.
///
/// \note The default return value is \c true, so a template specialization is
/// required to enable this check for substitutions.
template <typename Substitution>
bool is_simple_substitution(const Substitution& /*sigma*/)
{
  return true;
}

/// \brief Returns true if FV(rhs) is included in {lhs}.
inline
bool is_simple_substitution(const data::variable& lhs, const data::data_expression& rhs)
{
  std::set<data::variable> v = data::find_free_variables(rhs);
  if (v.empty())
  {
    return true;
  }
  if (v.size() == 1)
  {
    return *(v.begin()) == lhs;
  }
  return false;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_IS_SIMPLE_SUBSTITUTION_H
