// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_CONST_H_
#define MCRL2_UTILITIES_CONST_H_

namespace mcrl2
{
namespace utilities
{

/// \brief A constexpr function that casts an l-value reference to a const l-value reference
/// \details This function is available in <utility> from c++17 onwards. Also would be nicer to use std::add_const (instead of const T&) from c++14 onwards.
template <class T>
constexpr const T& as_const(T& t) noexcept
{
  return t;
}

/// \brief This constructor is deleted to prevent the casting of r-value references.
template <class T>
void as_const(const T&&) = delete;

}
}

#endif //
