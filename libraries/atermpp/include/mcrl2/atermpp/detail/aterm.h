// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm.h
/// \brief This file contains the _aterm class, which is the
///        class to which an aterm points. Each _aterm consists
///        of a function symbol, a reference count, used for garbage
///        collection and a next pointer used in the hash tables to
///        locate a term on the basis of the function symbol and 
///        arguments. Each _aterm contains an arbitrary number of
///        arguments after m_next, as indicated in the function symbol.
///        These arguments are not listed explicitly in the class 
///        below, but room is reserved for them when creating this
///        term. 

#ifndef DETAIL_ATERM_H
#define DETAIL_ATERM_H

#include <cstddef>
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/function_symbol_constants.h"
#include "mcrl2/atermpp/function_symbol.h"

namespace atermpp
{

class aterm;

namespace detail
{

static const size_t IN_FREE_LIST(-1);

class _aterm
{
  protected:
    function_symbol m_function_symbol;
    size_t  m_reference_count;
    _aterm* m_next;

  public:
    function_symbol& function() 
    {
      return m_function_symbol;
    }

    const function_symbol& function() const
    {
      return m_function_symbol;
    }

    void decrease_reference_count()
    {
      assert(!reference_count_indicates_is_in_freelist());
      assert(!reference_count_is_zero());
      --m_reference_count;
    } 

    void increase_reference_count()
    {
      assert(!reference_count_indicates_is_in_freelist());
      ++m_reference_count;
    } 

    void reset_reference_count(const bool check=true)
    {
      if (check) assert(reference_count_indicates_is_in_freelist());
      m_reference_count=0;
    } 

    bool reference_count_is_zero() const
    {
      return m_reference_count==0;
    } 

    size_t reference_count() const 
    {
      return m_reference_count;
    }

    void set_reference_count_indicates_in_freelist(const bool check=true)
    {
      if (check) assert(!reference_count_indicates_is_in_freelist());
      m_reference_count=IN_FREE_LIST;
    }

    bool reference_count_indicates_is_in_freelist() const
    {
      return m_reference_count==IN_FREE_LIST;
    }

    _aterm* next() const
    {
      return m_next;
    }

    void set_next(_aterm* n)
    {
      m_next=n;
    }
};

static const size_t TERM_SIZE=sizeof(_aterm)/sizeof(size_t);

detail::_aterm* allocate_term(const size_t size);
void remove_from_hashtable(_aterm *t);
void free_term(detail::_aterm* t);
void free_term_aux(detail::_aterm* t, detail::_aterm*& terms_to_be_removed);
void initialise_aterm_administration();
inline HashNumber hash_number(_aterm *t);

template <class Term, class Iter, class ATermConverter>
_aterm *make_list_backward(Iter first, Iter last, const ATermConverter& convert_to_aterm);
template <class Term, class Iter, class ATermConverter, class ATermFilter>
_aterm *make_list_backward(Iter first, Iter last, const ATermConverter& convert_to_aterm, const ATermFilter& aterm_filter);
template <class Term, class Iter, class ATermConverter>
_aterm *make_list_forward(Iter first, Iter last, const ATermConverter& convert_to_aterm);
template <class Term, class Iter, class ATermConverter, class ATermFilter>
_aterm *make_list_forward(Iter first, Iter last, const ATermConverter& convert_to_aterm, const ATermFilter& aterm_filter);

// Provides the address where the data belonging to this aterm is stored.
inline _aterm* address(const aterm& t); 

} // namespace detail
} // namespace atermpp

#endif /* DETAIL_ATERM_H */
