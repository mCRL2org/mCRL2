// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_appl_iterator.h
/// \brief Iterator for term_appl.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_APPL_H
#define MCRL2_ATERMPP_DETAIL_ATERM_APPL_H

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{

template <class Term>
class _aterm_appl:public _aterm
{
  public:
    Term        arg[1000];   /* This value 1000 is completely arbitrary, and should not be used
                                (therefore it is excessive). Using mallocs an array of the
                                appropriate length is declared, where it is possible that
                                the array has size 0, i.e. is absent. If the value is small
                                (it was 1), the clang compiler provides warnings. */
};

inline
size_t TERM_SIZE_APPL(const size_t arity)
{
  return (sizeof(detail::_aterm)/sizeof(size_t))+arity;
}


template <class Term, class ForwardIterator>
const detail::_aterm* local_term_appl(const function_symbol &sym, const ForwardIterator begin, const ForwardIterator end);

template <class Term, class InputIterator, class ATermConverter>
const _aterm* local_term_appl_with_converter(const function_symbol &sym, const InputIterator begin, const InputIterator end, const ATermConverter &convert_to_aterm);

template <class Term>
const detail::_aterm* term_appl1(const function_symbol &sym, const Term &t1);

template <class Term>
const detail::_aterm* term_appl2(const function_symbol &sym, const Term &t1, const Term &t2);

template <class Term>
const detail::_aterm* term_appl3(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3);

template <class Term>
const detail::_aterm* term_appl4(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4);

template <class Term>
const detail::_aterm* term_appl5(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5);

template <class Term>
const detail::_aterm* term_appl6(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5, const Term &t6);


} // namespace detail
/// \endcond

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_H
