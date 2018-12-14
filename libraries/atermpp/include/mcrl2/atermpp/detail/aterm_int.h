// Author(s): Wieger Wesselink, Jan Friso Groote, based on the aterm library.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_int.h
/// \brief Term containing an integer.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_INT_H
#define MCRL2_ATERMPP_DETAIL_ATERM_INT_H

#include "mcrl2/atermpp/detail/aterm.h"

namespace atermpp
{
namespace detail
{

/// \brief The underlying integer term that actually carries the integer data.
class _aterm_int : public _aterm
{
public:
  /// \brief Constructs the underlying term from a given value.
  _aterm_int(std::size_t value);

  std::size_t value() const noexcept
  {
    return m_value;
  }
  
private:
  std::size_t m_value;
};

static_assert(sizeof(_aterm_int) == sizeof(_aterm) + sizeof(std::size_t), "Sanity check: _aterm_int size");

} // namespace detail 

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_INT_H
