// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/hash.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_HASH_H
#define MCRL2_CORE_HASH_H

#ifdef MCRL2_INDEX_TRAITS_USE_UNORDERED_MAP

#include <functional>
#include <utility>
#include <boost/functional/hash.hpp>
#include "mcrl2/core/identifier_string.h"

namespace mcrl2 {

namespace core {

inline
std::size_t hash_value(const atermpp::aterm& x)
{
  return atermpp::detail::hash_number(atermpp::detail::address(x));
}

inline
std::size_t hash_value(const atermpp::aterm& x1, const atermpp::aterm& x2)
{
  return boost::hash_value(std::make_pair(atermpp::detail::hash_number(atermpp::detail::address(x1)), atermpp::detail::hash_number(atermpp::detail::address(x2))));
}

} // namespace core

} // namespace mcrl2

namespace std {

/// \brief hash specialization
template<>
struct hash<mcrl2::core::identifier_string>
{
  std::size_t operator()(const mcrl2::core::identifier_string& x) const
  {
    return mcrl2::core::hash_value(x);
  }
};

#endif MCRL2_INDEX_TRAITS_USE_UNORDERED_MAP

}

#endif // MCRL2_CORE_HASH_H
