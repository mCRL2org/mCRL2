// Author(s): Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_list_implementation.h
/// \brief This file contains basic operations on term_lists. 

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/detail/aterm_appl_implementation.h"

namespace atermpp
{

const std::size_t max_len_of_short_list=10000;  // The length of a short list. If lists
                                                // are short the stack can be used for temporary data.
                                                // Otherwise the heap must be used to avoid stack overflow.
                                                // The chosen value is rather arbitrary. 

template <class Term>
void term_list<Term>::push_front(const Term& el)
{
   *this = term_list<Term>(detail::term_appl2<aterm>(detail::function_adm.AS_LIST, el, *this));  //TODO This contains one increase of a reference count too many. 
}


template <typename Term>
inline
term_list<Term> push_back(const term_list<Term>& l, const Term& el)
{
  typedef typename term_list<Term>::const_iterator const_iterator;
  
  const std::size_t len = l.size();
  if (len<max_len_of_short_list)
  {
    // The list is short, use the stack for temporal storage.
    const_iterator* buffer = MCRL2_SPECIFIC_STACK_ALLOCATOR(const_iterator, len);

    /* Collect all elements of list in buffer */
    
    std::size_t j=0;
    for (const_iterator i = l.begin(); i != l.end(); ++i, ++j)
    {
      buffer[j]=i;
    }

    term_list<Term> result;
    result.push_front(el);

    /* Insert elements at the front of the list */
    while (j>0)
    {
      j=j-1;
      result.push_front(*buffer[j]);
    }
    return result;
  }
  else 
  {
    // The list is long. Use the heap to store intermediate data.
    std::vector<Term> buffer;
    buffer.reserve(len);

    for (const Term& t: l)
    {
      buffer.push_back(t); 
    }

    term_list<Term> result;
    result.push_front(el);

    /* Insert elements at the front of the list */
    for (typename std::vector<Term>::reverse_iterator i=buffer.rbegin(); i!=buffer.rend(); ++i)
    {
      result.push_front(*i);
    }
    return result;
  }
}


template <typename Term>
inline
term_list<Term> reverse(const term_list<Term>& l)
{
  term_list<Term> result;
  for(const Term& t: l)
  {
    result.push_front(t);
  }
  return result;
}


template <typename Term>
inline
term_list<Term> remove_one_element(const term_list<Term>& list, const Term& t)
{
  typedef typename term_list<Term>::const_iterator const_iterator;
  
  std::size_t len=0;
  const_iterator i = list.begin();
  for( ; i!=list.end(); ++i, ++len)
  {
    if (*i==t)
    {
      break;
    }
  }

  if (i==list.end())
  {
    // Term t not found in list.
    return list;
  }

  const_iterator* buffer = MCRL2_SPECIFIC_STACK_ALLOCATOR(const_iterator, len);

  term_list<Term> result = list; 
  std::size_t k=0;
  for(const_iterator j = list.begin(); j != i; ++j, ++k)
  {
    buffer[k]=j;
    result.pop_front();
  }
  assert(len==k);
  assert(result.front()==t);
  result.pop_front(); // skip the element.

  while (k>0) 
  {
    k=k-1;
    result.push_front(*buffer[k]);
  }

  return result;
}


template <typename Term1, typename Term2>
inline
typename std::conditional<std::is_convertible<Term2,Term1>::value,term_list<Term1>,term_list<Term2>>::type
operator+(const term_list<Term1>& l, const term_list<Term2>& m)
{
  static_assert(std::is_convertible< Term1, Term2 >::value ||
                std::is_convertible< Term2, Term1 >::value,
                       "Concatenated lists must be of convertible types. ");
  static_assert(sizeof(Term1) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added. ");
  static_assert(sizeof(Term2) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added. ");
  typedef typename std::conditional<std::is_convertible<Term2,Term1>::value,Term1,Term2>::type ResultType;
  typedef typename term_list<Term1>::const_iterator const_iterator;

  if (m.empty())
  {
    return reinterpret_cast<const term_list<ResultType>&>(l);
  }

  std::size_t len = l.size();

  if (len == 0)
  {
    return reinterpret_cast<const term_list<ResultType>&>(m);
  }

  term_list<ResultType> result = reinterpret_cast<const term_list<ResultType>&>(m);

  if (len<max_len_of_short_list)
  {
    // The length is short. Use the stack for temporary storage.
    const_iterator* buffer = MCRL2_SPECIFIC_STACK_ALLOCATOR(const_iterator, len);

    std::size_t j=0;
    for (const_iterator i = l.begin(); i != l.end(); ++i, ++j)
    {
      buffer[j]=i;
    }
    assert(j==len);

    // Insert elements at the front of the list
    while (j>0)
    {
      j=j-1;
      result.push_front(*buffer[j]);
    }

    return result;
  }
  else 
  {
    // The length of l is very long. Use the heap for temporary storage.
    std::vector<ResultType> buffer;
    buffer.reserve(len);

    for (const Term1& t: l)
    {
      buffer.push_back(t);
    }

    // Insert elements at the front of the list
    for(typename std::vector<ResultType>::const_reverse_iterator i=buffer.rbegin(); i!=buffer.rend(); ++i) 
    {
      result.push_front(*i);
    }

    return result;
  }
}


namespace detail
{
  // The functions make_list_backward and make_list_forward with three and four arguments are almost the same.
  // The reason for this is that there is a 5% loss of speed of the toolset when merging these two functions.
  // This is caused by storing and protecting the intermediate value of the converted aterm. See Term t=convert_to_aterm(...).
  template <class Term, class Iter, class ATermConverter, class ATermFilter>
  inline _aterm *make_list_backward(Iter first, Iter last, const ATermConverter& convert_to_aterm, const ATermFilter& aterm_filter)
  {
    static_assert(std::is_base_of<aterm, Term>::value,"Term must be derived from an aterm");
    static_assert(sizeof(Term)==sizeof(aterm),"Term derived from an aterm must not have extra fields");
    _aterm* result=aterm::static_empty_aterm_list;
    while (first != last)
    {
      const Term t=convert_to_aterm(*(--last));
      if (aterm_filter(t))
      {
        result=term_appl2<aterm>(detail::function_adm.AS_LIST,t,down_cast<term_list<Term> >(aterm(result)));
      }
    }
    return result;
  }

  template <class Term, class Iter, class ATermConverter>
  inline _aterm *make_list_backward(Iter first, Iter last, const ATermConverter& convert_to_aterm)
  {
    static_assert(std::is_base_of<aterm, Term>::value,"Term must be derived from an aterm");
    static_assert(sizeof(Term)==sizeof(aterm),"Term derived from an aterm must not have extra fields");
    _aterm* result=aterm::static_empty_aterm_list;
    while (first != last)
    {
      result=term_appl2<aterm>(detail::function_adm.AS_LIST,convert_to_aterm(*(--last)),down_cast<term_list<Term> >(aterm(result)));
    }
    return result; 
  } 

  // See the note at make_list_backwards for why there are two almost similar version of make_list_forward.
  template <class Term, class Iter, class ATermConverter, class ATermFilter>
  inline _aterm *make_list_forward(Iter first, Iter last, const ATermConverter& convert_to_aterm, const ATermFilter& aterm_filter)
  {
    static_assert(std::is_base_of<aterm, Term>::value,"Term must be derived from an aterm");
    static_assert(sizeof(Term)==sizeof(aterm),"Term derived from an aterm must not have extra fields");

    const std::size_t len=std::distance(first,last);
    if (len<max_len_of_short_list)  // If the list is sufficiently short, use the stack.
    {
      Term* buffer = MCRL2_SPECIFIC_STACK_ALLOCATOR(Term, len);
      Term *const buffer_begin=buffer;
      Term* i=buffer_begin;
      for(; first != last; ++first)
      {
        const Term t=convert_to_aterm(*first);
        if (aterm_filter(t))
        {
          // Placement new; The buffer is not properly initialised.
          new (i) Term(t);
          ++i;
        }
      }

      _aterm* result=aterm::static_empty_aterm_list;
      for( ; i!=buffer_begin ; )
      {
        --i;
        result=term_appl2<aterm>(detail::function_adm.AS_LIST,*i,down_cast<term_list<Term> >(aterm(result)));
        (*i).~Term(); // Destroy the elements in the buffer explicitly.
      }
      return result; 
    }
    else
    {
      // The list is long. Therefore use the heap for temporary storage.
      std::vector<Term> buffer;
      buffer.reserve(len); 
      for(; first != last; ++first)
      {
        const Term t=convert_to_aterm(*first);
        if (aterm_filter(t))
        {
          // Placement new; The buffer is not properly initialised.
          buffer.push_back(t); 
        }
      }

      _aterm* result=aterm::static_empty_aterm_list;
      for(typename std::vector<Term>::const_reverse_iterator i=buffer.rbegin();  i!=buffer.rend(); ++i)
      {
        result=term_appl2<aterm>(detail::function_adm.AS_LIST,*i,down_cast<term_list<Term> >(aterm(result)));
      }
      return result; 
    }
  }

  template <class Term, class Iter, class ATermConverter>
  inline _aterm *make_list_forward(Iter first, Iter last, const ATermConverter& convert_to_aterm)
  {
    static_assert(std::is_base_of<aterm, Term>::value,"Term must be derived from an aterm");
    static_assert(sizeof(Term)==sizeof(aterm),"Term derived from an aterm must not have extra fields");

    const std::size_t len=std::distance(first,last);
    if (len<max_len_of_short_list) // If the list is sufficiently short, use the stack.
    {
      Term* buffer = MCRL2_SPECIFIC_STACK_ALLOCATOR(Term, len);
      Term *const buffer_begin=buffer;
      Term* i=buffer_begin;
      for(; first != last; ++first,++i)
      {
        // Placement new; The buffer is not properly initialised.
        new (i) Term(convert_to_aterm(*first));
      }

      _aterm* result=aterm::static_empty_aterm_list;
      for( ; i!=buffer_begin ; )
      {
        --i;
        result=term_appl2<aterm>(detail::function_adm.AS_LIST,*i,down_cast<term_list<Term> >(aterm(result)));
        (*i).~Term(); // Destroy the elements in the buffer explicitly.
      }
      return result; 
    }
    else
    {
      // The list is very long. Reserve memory on the heap.
      std::vector<Term> buffer;
      buffer.reserve(len);
      for(; first != last; ++first)
      {
        buffer.push_back(convert_to_aterm(*first));
      }

      _aterm* result=aterm::static_empty_aterm_list;
      for(typename std::vector<Term>::const_reverse_iterator i=buffer.rbegin(); i!=buffer.rend(); ++i)
      {
        result=term_appl2<aterm>(detail::function_adm.AS_LIST,*i,down_cast<term_list<Term> >(aterm(result)));
      }
      return result; 
    }
  }
} // detail



} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H
