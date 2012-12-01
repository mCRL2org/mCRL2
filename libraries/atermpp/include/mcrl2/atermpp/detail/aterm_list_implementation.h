#ifndef MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/detail/aterm_appl_implementation.h"

namespace atermpp
{

template <class Term>
term_list<Term> push_front(const term_list<Term> &tail, const Term &el)
{
  return aterm_cast<const term_list<Term> > (term_appl<aterm> (detail::function_adm.AS_LIST,el,tail));
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
    std::vector<Term> temporary_store;  
    for(; first != last; ++first)
    {
      temporary_store.push_back(convert_to_aterm(*first));
    }

    return make_list_backward<Term,typename std::vector<Term>::const_iterator,do_not_convert_term<Term> >
               (temporary_store.begin(),temporary_store.end(),do_not_convert_term<Term>());
  }
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_LIST_IMPLEMENTATION_H
