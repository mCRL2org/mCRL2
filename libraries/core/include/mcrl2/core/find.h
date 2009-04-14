// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/find.h
/// \brief Search functions of the core library.

#ifndef MCRL2_CORE_FIND_H
#define MCRL2_CORE_FIND_H

#include <set>
#include <iterator>
#include <functional>
#include <boost/bind.hpp>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/identifier_string.h"

namespace mcrl2 {

namespace core {

/// \brief Returns the set of all identifier strings occurring in the term t
/// \param t A term
/// \return The set of all identifier strings occurring in the term t
template <typename Term>
std::set<core::identifier_string> find_identifiers(Term t)
{
  std::set<core::identifier_string> result;
  atermpp::find_all_if(atermpp::aterm_traits<Term>::term(t), core::is_identifier_string, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns the set of all identifier strings occurring in the vector v
template <typename Term>
std::set<core::identifier_string> find_identifiers(atermpp::vector<Term> v)
{
  std::set<core::identifier_string> result;
  for(typename atermpp::vector<Term>::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    std::set<core::identifier_string> intermediate = find_identifiers(*i);
    for(std::set<core::identifier_string>::const_iterator j = intermediate.begin(); j != intermediate.end(); ++j)
    {
      result.insert(*j);
    }
  }
  return result;
}

} // namespace mcrl2

} // namespace core

#endif //MCRL2_CORE_FIND_H

