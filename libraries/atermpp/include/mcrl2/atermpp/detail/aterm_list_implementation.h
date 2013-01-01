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
void term_list<Term>::push_front(const Term &el)
{
  // This call can be made slightly more efficient by not using the assignment. Now the old value
  // of *this is first protected by increasing its reference count by one, but only if a new term is constructed, 
  // and this value is decreased again when assigned.
  *this=aterm_cast<const term_list<Term> > (term_appl<aterm> (detail::function_adm.AS_LIST,el,*this));
  return;
} 

// This alternative below has problems with reference counting. 
// template <class Term>
// void term_list<Term>::push_front(const Term &el)
// {
// //   The effect of this command is the same as the line below, but it has been slightly optimised by
// //   using a constant number, and by not unnecesarily increasing and decreasing a reference count.
// //   *this=aterm_cast<const term_list<Term> > (term_appl<aterm> (detail::function_adm.AS_LIST,el,*this));
//   using namespace detail;
//   assert(detail::function_adm.AS_LIST.arity()==detail::AS_LIST_NUMBER);
// 
//   detail::CHECK_TERM(el);
//   detail::CHECK_TERM(*this);
//   assert(detail::function_adm.AS_LIST.number()==detail::AS_LIST_NUMBER);
//   HashNumber hnr = COMBINE(COMBINE(detail::AS_LIST_NUMBER, el),*this);
// 
//   const detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
//   while (cur)
//   {
//     if (cur->function().number()==detail::AS_LIST_NUMBER &&
//         reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[0] == el &&
//         reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[1] == *this)
//     {
//       /* The list to be made already exists. Return it */
//       // assert(cur->reference_count()>0);
//       // cur->increase_reference_count();
//       // decrease_reference_count();
//       // m_term=cur;
//       *this=aterm_cast<const term_list<Term> > (cur); // This works, the alternative and more efficient lines above do not.
//       return;
//     }
//     cur = cur->next();
//   }
// 
//   cur = detail::allocate_term(detail::TERM_SIZE_APPL(2));
//   /* Delay masking until after allocate_term */
//   hnr &= detail::aterm_table_mask;
//   new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(detail::function_adm.AS_LIST);
//   new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(el);
//   // Do not increase the garbage collection count of *this, as it is not increased below (*).
//   // new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(*this);
//   const _aterm** addr_of_tail=reinterpret_cast<const _aterm**>
//                    (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1]));
//   *addr_of_tail =this->address();
// 
//   cur->set_next(detail::aterm_hashtable[hnr]);
//   detail::aterm_hashtable[hnr] = cur;
// 
//   // The garbage collection count is intentionally not increased, as it was not decreased at (*) above.
//   m_term=cur; // Also here something goes wrong with reference counting.
// }


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
