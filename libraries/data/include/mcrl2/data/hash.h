// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/hash.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_HASH_H
#define MCRL2_DATA_HASH_H

#ifdef MCRL2_INDEX_TRAITS_USE_UNORDERED_MAP

#include "mcrl2/core/hash.h"
#include "mcrl2/data/variable.h"

namespace std {

/// \brief hash specialization
template<>
struct hash<std::pair<mcrl2::core::identifier_string, mcrl2::data::variable_list> >
{
  std::size_t operator()(const std::pair<mcrl2::core::identifier_string, mcrl2::data::variable_list>& x) const
  {
    return mcrl2::core::hash_value(x.first, x.second);
  }
};

/// \brief hash specialization
template<>
struct hash<std::pair<mcrl2::core::identifier_string, mcrl2::data::data_expression_list> >
{
  std::size_t operator()(const std::pair<mcrl2::core::identifier_string, mcrl2::data::data_expression_list>& x) const
  {
    return mcrl2::core::hash_value(x.first, x.second);
  }
};

} // namespace std

#endif // MCRL2_INDEX_TRAITS_USE_UNORDERED_MAP

#endif // MCRL2_DATA_HASH_H
