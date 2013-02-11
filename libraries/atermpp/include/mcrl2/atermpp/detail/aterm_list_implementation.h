#ifndef MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/stack_alloc.h"
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
  typedef std::vector <typename term_list<Term>::const_iterator, mcrl2::utilities::stack_alloc<typename term_list<Term>::const_iterator,64> > vector_t;
  vector_t buffer;
  const size_t len = l.size();
  buffer.reserve(len);

  /* Collect all elements of list in buffer */
  
  for (typename term_list<Term>::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    buffer.push_back(i);  
  }

  term_list<Term> result;
  result.push_front(el);

  /* Insert elements at the front of the list */
  for (typename vector_t::const_reverse_iterator i=buffer.rbegin(); i!=buffer.rend(); ++i)
  {
    result.push_front(**i);
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
  size_t len=0;
  typename term_list<Term>::const_iterator i=list.begin();
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

  typedef std::vector <typename term_list<Term>::const_iterator, mcrl2::utilities::stack_alloc<typename term_list<Term>::const_iterator,64> > vector_t;
  vector_t buffer;
  buffer.reserve(len);

  term_list<Term> result = list; 
  for(typename term_list<Term>::const_iterator j=list.begin();  j!=i; ++j)
  {
    buffer.push_back(j);
    result.pop_front();
  }
  assert(len==buffer.size());
  assert(result.front()==t);
  result.pop_front(); // skip the element.

  for (typename vector_t::const_reverse_iterator i=buffer.rbegin(); i!=buffer.rend(); ++i)
  {
    result.push_front(**i);
  }

  return result;
}

template <typename Term>
inline
term_list<Term> operator+(const term_list<Term> &l, const term_list<Term> &m)
{

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
  typedef std::vector <typename term_list<Term>::const_iterator, mcrl2::utilities::stack_alloc<typename term_list<Term>::const_iterator,64> > vector_t;
  vector_t buffer;
  buffer.reserve(len);

  for (typename term_list<Term>::iterator i = l.begin(); i != l.end(); ++i)
  {
    buffer.push_back(i); 
  }

  /* Insert elements at the front of the list */
  for (typename vector_t::const_reverse_iterator j=buffer.rbegin(); j!=buffer.rend(); ++j)
  {
    result.push_front(**j);
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
      const typename Iter::value_type &t=*(--last);
      result=term_appl2<aterm>(detail::function_adm.AS_LIST,convert_to_aterm(t),aterm_cast<term_list<Term> >(aterm(result)));
    }
    return result;
  }

  template <class Term, class Iter, class ATermConverter>
  inline const _aterm *make_list_forward(Iter first, Iter last, const ATermConverter &convert_to_aterm)
  {
    BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
    BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(aterm));
    typedef std::vector<Term,mcrl2::utilities::stack_alloc < Term, 64> > vector_type;
    vector_type temporary_store;  
    temporary_store.reserve(64);
    for(; first != last; ++first)
    {
      temporary_store.push_back(convert_to_aterm(*first));
    }

    return make_list_backward<Term,typename vector_type::const_iterator,do_not_convert_term<Term> >
               (temporary_store.begin(),temporary_store.end(),do_not_convert_term<Term>());
  }
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H
