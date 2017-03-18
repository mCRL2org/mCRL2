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

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_LIST_H
#define MCRL2_ATERMPP_DETAIL_ATERM_LIST_H

#include "mcrl2/atermpp/detail/aterm.h"

namespace atermpp
{
namespace detail
{

  template <class Term>
  class _term_list;

  template <class Term>
  struct do_not_convert_term
  {
    const Term& operator()(const Term& t) const
    {
      return t;
    }

    Term& operator()(Term& t) const
    {
      return t;
    }
  };

  template <class Term, class Iter, class ATermConverter>
  _aterm *make_list_backward(Iter first, Iter last, const ATermConverter& convert_to_aterm);

  template <class Term, class Iter, class ATermConverter>
  _aterm *make_list_forward(Iter first, Iter last, const ATermConverter& convert_to_aterm);

} // namespace detail 

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_INT_H
