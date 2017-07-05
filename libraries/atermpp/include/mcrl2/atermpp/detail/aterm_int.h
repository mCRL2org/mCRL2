// Author(s): Wieger Wesselink, Jan Friso Groote, based on the aterm library.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

class _aterm_int:public _aterm
{
  public:
    std::size_t value;
};

_aterm *aterm_int(std::size_t val);

} // namespace detail 

static const std::size_t TERM_SIZE_INT = sizeof(detail::_aterm_int)/sizeof(std::size_t);

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_INT_H
