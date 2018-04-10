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

static const std::size_t IN_FREE_LIST(-1);

class _aterm
{
  protected:
    function_symbol m_function_symbol;
    std::size_t  m_reference_count;
    _aterm* m_next;

  public:
    _aterm()=delete;
    _aterm(const _aterm &)=delete;
    _aterm(_aterm &&)=delete;
    _aterm& operator=(const _aterm&)=delete;
    _aterm& operator=(_aterm&&)=delete;

    function_symbol& function() noexcept
    {
      return m_function_symbol;
    }

    const function_symbol& function() const noexcept
    {
      return m_function_symbol;
    }

    void decrease_reference_count() noexcept
    {
      assert(!reference_count_indicates_is_in_freelist());
      assert(!reference_count_is_zero());
      --m_reference_count;
    } 

    void increase_reference_count() noexcept
    {
      assert(!reference_count_indicates_is_in_freelist());
      ++m_reference_count;
    } 

    void reset_reference_count(const bool check=true) noexcept
    {
      if (check) assert(reference_count_indicates_is_in_freelist());
      m_reference_count=0;
    } 

    bool reference_count_is_zero() const noexcept
    {
      return m_reference_count==0;
    } 

    std::size_t reference_count() const noexcept
    {
      return m_reference_count;
    }

    void set_reference_count_indicates_in_freelist(const bool check=true) noexcept
    {
      if (check) assert(!reference_count_indicates_is_in_freelist());
      m_reference_count=IN_FREE_LIST;
    }

    bool reference_count_indicates_is_in_freelist() const noexcept
    {
      return m_reference_count==IN_FREE_LIST;
    }

    _aterm* next() const noexcept
    {
      return m_next;
    }

    void set_next(_aterm* n) noexcept
    {
      m_next=n;
    }
};

static const std::size_t TERM_SIZE=sizeof(_aterm)/sizeof(std::size_t);

detail::_aterm* allocate_term(const std::size_t size);
void remove_from_hashtable(_aterm *t);
void free_term(detail::_aterm* t);
void free_term_aux(detail::_aterm* t, detail::_aterm*& terms_to_be_removed);

extern detail::_aterm* * aterm_hashtable;
extern std::size_t aterm_table_mask;
extern std::size_t total_nodes_in_hashtable;

void call_creation_hook(_aterm*);

inline void insert_in_hashtable(_aterm *t, const std::size_t hnr)
{
  t->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = t;
  total_nodes_in_hashtable++;
}

inline _aterm* term_appl0(const function_symbol& sym)
{
  assert(sym.arity()==0);

  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = function_symbol_hasher(sym);

  _aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];

  while (cur)
  {
    if (cur->function()==sym)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(detail::TERM_SIZE);
  /* Delay masking until after allocate */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}


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

inline std::size_t hash_value_aterm_int(const std::size_t val)
{
  return val;
}

} // namespace detail
} // namespace atermpp

namespace std
{

// \brief specialisation of the hash function.
template<>
struct hash<atermpp::detail::_aterm*>
{
  // Default constructor, required for const qualified hash functions. 
  hash()
  {}

  std::size_t operator()(const atermpp::detail::_aterm* t) const
  {
    return reinterpret_cast<std::size_t>(t)>>4;
  }
};

} // namespace detail

#endif /* DETAIL_ATERM_H */
