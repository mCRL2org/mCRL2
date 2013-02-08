#ifndef MCRL2_ATERMPP_DETAIL_ATERM_INT_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_INT_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/aterm.h"
#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/detail/aterm_list.h"
#include "mcrl2/atermpp/detail/aterm_implementation.h"

namespace atermpp
{

namespace detail
{

inline const _aterm* aterm_int(size_t val)
{
  HashNumber hnr = COMBINE(function_adm.AS_INT.number(), val);

  const _aterm* cur = aterm_hashtable[hnr & aterm_table_mask];
  while (cur)
  { if  (cur->function()==function_adm.AS_INT && reinterpret_cast<const _aterm_int*>(cur)->value == val)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = allocate_term(TERM_SIZE_INT);
  /* Delay masking until after allocate */
  hnr &= aterm_table_mask;
  new (&const_cast<_aterm *>(cur)->function()) function_symbol(function_adm.AS_INT);
  reinterpret_cast<_aterm_int*>(const_cast<_aterm *>(cur))->value = val;

  cur->set_next(aterm_hashtable[hnr]);
  aterm_hashtable[hnr] = cur;
  

  assert((hnr & aterm_table_mask) == (hash_number(cur) & aterm_table_mask));
  return cur;
}

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_INT_IMPLEMENTATION_H
