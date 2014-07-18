// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/convert.h
/// \brief Conversion utilities for converting between the containers with
///    expressions and term lists that contain expressions

#ifndef MCRL2_ATERMPP_CONVERT_H
#define MCRL2_ATERMPP_CONVERT_H

#include <type_traits>

#include "mcrl2/atermpp/container_utility.h"

namespace atermpp
{

/// \brief Convert container with expressions to a new container with expressions

template < typename TargetContainer, typename SourceContainer >
const TargetContainer& convert(const SourceContainer &l,
      typename std::enable_if<std::is_base_of<atermpp::aterm, TargetContainer>::value>::type* = 0,
      typename std::enable_if<std::is_base_of<atermpp::aterm, SourceContainer>::value>::type* = 0)
{
  return down_cast<const TargetContainer>(l);
}

/// \brief Convert container with expressions to a new container with expressions
template < typename TargetContainer, typename SourceContainer >
TargetContainer convert(const SourceContainer& l,
      typename std::enable_if<std::is_base_of<atermpp::aterm, TargetContainer>::value>::type* = 0,
      typename std::enable_if< !std::is_base_of<atermpp::aterm, SourceContainer>::value>::type* = 0)
{
  return TargetContainer(l.begin(), l.end());
}

/// \brief Convert container with expressions to a new container with expressions
template < typename TargetContainer, typename SourceContainer >
TargetContainer convert(const SourceContainer& l,
      typename std::enable_if< !std::is_base_of<atermpp::aterm, TargetContainer>::value>::type* = 0)
{
  return TargetContainer(l.begin(), l.end());
}
} // namespace atermpp

#endif // MCRL2_ATERMPP_CONVERT_H

