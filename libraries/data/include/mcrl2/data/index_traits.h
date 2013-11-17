// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/index_traits.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_INDEX_TRAITS_H
#define MCRL2_DATA_INDEX_TRAITS_H

#include <functional>
#include <utility>

#include "mcrl2/core/index_traits.h"
#include "mcrl2/data/variable.h"

#ifdef MCRL2_INDEX_TRAITS_USE_UNORDERED_MAP

namespace std {

/// \brief hash specialization
template<>
struct hash<std::pair<mcrl2::core::identifier_string, mcrl2::data::sort_expression> >
{
  std::size_t operator()(const std::pair<mcrl2::core::identifier_string, mcrl2::data::sort_expression>& x) const
  {
    return mcrl2::core::hash_value(x.first, x.second);
  }
};

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

}

#endif

namespace mcrl2 {

namespace data {

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INDEX_TRAITS_H
