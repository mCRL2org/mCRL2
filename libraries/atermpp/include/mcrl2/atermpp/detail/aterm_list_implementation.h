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

template <class Term>
void term_list<Term>::push_front(const Term &el)
{
   *this=aterm_cast<const term_list<Term> > (term_appl<aterm> (detail::function_adm.AS_LIST,el,*this));
}


template <typename Term>
inline
term_list<Term> push_back(const term_list<Term> &l, const Term &el)
{
  typedef typename term_list<Term>::const_iterator const_iterator;
  
  const size_t len = l.size();
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,const_iterator, len);

  /* Collect all elements of list in buffer */
  
  size_t j=0;
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


template <typename Term>
inline
term_list<Term> reverse(const term_list<Term> &l)
{
  term_list<Term> result;
  for(typename term_list<Term>::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    result.push_front(*i);
  }
  return result;
}


template <typename Term>
inline
term_list<Term> remove_one_element(const term_list<Term> &list, const Term &t)
{
  typedef typename term_list<Term>::const_iterator const_iterator;
  
  size_t len=0;
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

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,const_iterator, len);

  term_list<Term> result = list; 
  size_t k=0;
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

template <typename Term>
inline
term_list<Term> operator+(const term_list<Term> &l, const term_list<Term> &m)
{
  typedef typename term_list<Term>::const_iterator const_iterator;

  if (m.empty())
  {
    return l;
  }

  size_t len = l.size();

  if (len == 0)
  {
    return m;
  }

  term_list<Term> result = m;

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,const_iterator, len);

  size_t j=0;
  for (const_iterator i = l.begin(); i != l.end(); ++i, ++j)
  {
    buffer[j]=i;
  }
  assert(j=len);

  // Insert elements at the front of the list
  while (j>0)
  {
    j=j-1;
    result.push_front(*buffer[j]);
  }

  return result;
}



namespace detail
{
  template <class Term, class Iter, class ATermConverter>
  inline const _aterm *make_list_backward(Iter first, Iter last, const ATermConverter &convert_to_aterm)
  {
    BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
    BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(aterm));
    const _aterm* result=aterm::empty_aterm_list();
    while (first != last)
    {
      result=term_appl2<aterm>(detail::function_adm.AS_LIST,convert_to_aterm(*(--last)),aterm_cast<term_list<Term> >(aterm(result)));
    }
    return result;
  }


  template <class Term, class Iter, class ATermConverter>
  inline const _aterm *make_list_forward(Iter first, Iter last, const ATermConverter &convert_to_aterm)
  {
    BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
    BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(aterm));

    const size_t len=std::distance(first,last);
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,Term, len);
    Term *const buffer_begin=buffer;
    Term* i=buffer_begin;
    for(; first != last; ++first,++i)
    {
      // Placement new; The buffer is not properly initialised.
      new (i) Term(convert_to_aterm(*first));
    }

    const _aterm* result=aterm::empty_aterm_list();
    for( ; i!=buffer_begin ; )
    {
      --i;
      result=term_appl2<aterm>(detail::function_adm.AS_LIST,*i,aterm_cast<term_list<Term> >(aterm(result)));
      (*i).~Term(); // Destroy the elements in the buffer explicitly.
    }
    return result; 
  }

}


} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H
