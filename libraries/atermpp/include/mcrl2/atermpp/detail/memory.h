#ifndef MEMORY_H
#define MEMORY_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

namespace detail
{
  extern size_t aterm_table_mask;
  extern const detail::_aterm* * aterm_hashtable;


  extern aterm static_undefined_aterm;  // detail/aterm_implementation.h
  extern aterm static_empty_aterm_list;
 
  void initialise_administration();
  void initialise_aterm_administration();

}
  
inline
const detail::_aterm *aterm::undefined_aterm()
{
  if (detail::static_undefined_aterm.m_term==NULL)
  {
    detail::initialise_administration();
    new (&detail::static_undefined_aterm) aterm(detail::function_adm.AS_DEFAULT); // Use placement new as static_undefined_aterm
                                                                                  // may not have initialised when this is called, 
                                                                                  // causing a problem with reference counting.
  }

  return detail::static_undefined_aterm.m_term;
} 

inline
const detail::_aterm *aterm::empty_aterm_list()
{
  if (detail::static_empty_aterm_list.m_term==NULL)
  {
    detail::initialise_administration();
    new (&detail::static_empty_aterm_list) aterm(detail::function_adm.AS_EMPTY_LIST); // Use placement new as static_empty_atermlist
                                                                                      // may not have initialised when this is called, 
                                                                                      // causing a problem with reference counting.
  }
  else if (detail::static_empty_aterm_list==detail::static_undefined_aterm )
  {
    detail::static_empty_aterm_list=aterm(detail::function_adm.AS_EMPTY_LIST);
  }
  return detail::static_empty_aterm_list.m_term;
} 

namespace detail
{

inline
size_t COMBINE(const HashNumber hnr, const size_t w)
{
  return (hnr<<1) ^ hnr ^ w;
}

inline
size_t COMBINE(const HashNumber hnr, const aterm &w)
{
  return COMBINE(hnr,reinterpret_cast<size_t>(w.address()));
}

inline
void CHECK_TERM(const aterm &
#ifndef NDEBUG 
t
#endif
)
{
  assert(t.defined());
  assert(t.address()->reference_count()>0);
  // assert(aterm_cast<aterm_appl>(t).function().name().size()!=0);
}

inline HashNumber hash_number(const detail::_aterm *t)
{
  const function_symbol &f=t->function();
  HashNumber hnr = f.number();

  const size_t* begin=reinterpret_cast<const size_t*>(t)+TERM_SIZE;
  const size_t* end=begin+f.arity();
  for (const size_t* i=begin; i!=end; ++i)
  {
    hnr = COMBINE(hnr, *i);
  } 

  return hnr;
}

} //namespace detail
} // namespace atermpp

#endif
