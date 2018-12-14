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

/// \brief Constructs a list starting from first to last. The iterators are traversed backwards and each element is
/// 		   converted using the TermConverter.
/// \details The functions make_list_backward and make_list_forward with three and four arguments are almost the same.
/// 			 The reason for this is that there is a 5% loss of speed of the toolset when merging these two functions.
///          This is caused by storing and protecting the intermediate value of the converted aterm. See Term t = convert_to_aterm(...).
template <typename Term, typename Iter, typename ATermConverter>
aterm make_list_backward(Iter first, Iter last, ATermConverter convert_to_aterm);

/// \brief Constructs a list starting from first to last. The iterators are traversed backwards and each element is
/// 		   converted using the TermConverter and inserted whenever TermFilter yields true for the converted element.
template <typename Term, typename Iter, typename ATermConverter, typename ATermFilter>
aterm make_list_backward(Iter first, Iter last, ATermConverter convert_to_aterm, ATermFilter aterm_filter);

/// \brief Constructs a list starting from first to last. Each element is converted using the TermConverter.
/// \details Will first store the converted elements in an array and then insert them into the list.
template <typename Term, class Iter, class ATermConverter>
aterm make_list_forward(Iter first, Iter last, ATermConverter convert_to_aterm);

/// \brief Constructs a list starting from first to last. Each element is converted using the TermConverter and inserted
/// 		   whenever TermFilter yields true for the converted element.
/// \details Will first store the converted elements in an array and then insert them into the list.
template <typename Term, class Iter, class ATermConverter, class ATermFilter>
aterm make_list_forward(Iter first, Iter last, ATermConverter convert_to_aterm, ATermFilter aterm_filter);

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_INT_H
