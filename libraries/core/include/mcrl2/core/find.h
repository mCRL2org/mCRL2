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

namespace mcrl2 {

namespace core {

/// \brief Returns the set of all identifier strings occurring in the term t
template <typename Term>
std::set<core::identifier_string> find_identifiers(Term t)
{
  std::set<core::identifier_string> result;
  find_all_if(atermpp::aterm_traits<Term>::term(t), core::is_identifier_string, std::inserter(result, result.end()));
  return result;
}

} // namespace mcrl2

} // namespace core

#endif //MCRL2_CORE_FIND_H

