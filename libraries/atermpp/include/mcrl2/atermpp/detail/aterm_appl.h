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

template <class Term>
class term_appl;

template <typename Term>
class term_appl_iterator;


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
std::size_t TERM_SIZE_APPL(const std::size_t arity)
{
  return (sizeof(_aterm)/sizeof(std::size_t))+arity;
}


template <class Term, class ForwardIterator>
_aterm* local_term_appl(const function_symbol& sym, const ForwardIterator begin, const ForwardIterator end);

template <class Term, class InputIterator, class ATermConverter>
_aterm* local_term_appl_with_converter(const function_symbol& sym, const InputIterator begin, const InputIterator end, const ATermConverter& convert_to_aterm);

// inline
// _aterm* term_appl0(const function_symbol& sym);

template <class Term>
_aterm* term_appl1(const function_symbol& sym, const Term& arg0);

template <class Term>
_aterm* term_appl2(const function_symbol& sym, const Term& arg0, const Term& arg1);

template <class Term>
_aterm* term_appl3(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2);

template <class Term>
_aterm* term_appl4(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2, const Term& arg3);

template <class Term>
_aterm* term_appl5(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2, const Term& arg3, const Term& arg4);

template <class Term>
_aterm* term_appl6(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2, const Term& arg3, const Term& arg4, const Term& arg5);

template <class Term>
_aterm* term_appl7(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2, const Term& arg3, const Term& arg4, const Term& arg5, const Term& arg6);

template < class Derived, class Base >
term_appl_iterator<Derived> aterm_appl_iterator_cast(term_appl_iterator<Base> a,
                                                                typename std::enable_if<
                                                                     std::is_base_of<aterm, Base>::value &&
                                                                     std::is_base_of<aterm, Derived>::value
                                                                >::type* = nullptr);


} // namespace detail
/// \endcond

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_H
