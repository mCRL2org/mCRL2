#ifndef MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/utilities/detail/memory_utility.h"

namespace atermpp
{

namespace detail
{

template <class Term, class InputIterator, class ATermConverter>
const _aterm* local_term_appl_with_converter(const function_symbol& sym, 
                                       const InputIterator begin, 
                                       const InputIterator end, 
                                       const ATermConverter& convert_to_aterm)
{
  const size_t arity = sym.arity();

  HashNumber hnr = SHIFT(addressf(sym));
  
  aterm* temporary_args = MCRL2_SPECIFIC_STACK_ALLOCATOR(aterm, arity);
  size_t j=0;
  for (InputIterator i=begin; i!=end; ++i, ++j)
  {
    new (&(temporary_args[j])) Term(convert_to_aterm(*i));
    CHECK_TERM(temporary_args[j]);
    hnr = COMBINE(hnr, temporary_args[j]);
  }
  assert(j==arity); 


  const detail::_aterm* cur = detail::aterm_hashtable[hnr&  detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[i] != temporary_args[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        for(size_t i=0; i<arity; ++i)
        {
          temporary_args[i].~aterm();
        }
        return cur;
      }
    }
    cur = cur->next();
  }

  assert(cur==nullptr);
  const detail::_aterm* new_term = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));

  // We copy the content of the temporary_args, without destruction/construction and adapting the reference counts.
  for(size_t i=0; i<arity; ++i)
  {
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(new_term))->arg[i])) const detail::_aterm*(detail::address(temporary_args[i]));
  }
  new (&const_cast<detail::_aterm*>(const_cast<detail::_aterm*>(new_term))->function()) function_symbol(sym);

  // Apply the table mask after allocate_term, which may resize the table.
  insert_in_hashtable(new_term,hnr&  detail::aterm_table_mask);
  call_creation_hook(new_term);

  return new_term;
}

template <class Term, class ForwardIterator>
const _aterm* local_term_appl(const function_symbol& sym, const ForwardIterator begin, const ForwardIterator end)
{
  const size_t arity = sym.arity();
  HashNumber hnr = SHIFT(addressf(sym)); 
  
  aterm* temporary_args=MCRL2_SPECIFIC_STACK_ALLOCATOR(aterm, arity);
  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    new (&(temporary_args[j])) Term(*i);
    CHECK_TERM(temporary_args[j]);
    hnr = COMBINE(hnr, temporary_args[j]);
  }
  assert(j==arity);

  const detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      for (size_t i=0; i<arity; ++i)
      {
        if (reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[i] != temporary_args[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        for(size_t i=0; i<arity; ++i)
        {
          temporary_args[i].~aterm();
        }
        return cur;
      }
    }
    cur = cur->next();
  }

  assert(cur==nullptr);
  const detail::_aterm* new_term = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));

  // We copy the content of the temporary_args, without destruction/construction and adapting the reference counts.
  for(size_t i=0; i<arity; ++i)
  {
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(new_term))->arg[i])) const detail::_aterm*(detail::address(temporary_args[i]));
  }

  new (&const_cast<detail::_aterm*>(const_cast<detail::_aterm*>(new_term))->function()) function_symbol(sym);

  // Apply the table_mask after applying allocate_term, which may resize the hash_table.
  insert_in_hashtable(new_term,hnr & detail::aterm_table_mask);
  call_creation_hook(new_term);
  
  return new_term;
}

inline const _aterm* term_appl0(const function_symbol& sym)
{
  assert(sym.arity()==0);

  HashNumber hnr = SHIFT(addressf(sym));

  const detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];

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

template <class Term>
const _aterm* term_appl1(const function_symbol& sym, const Term& arg0)
{
  assert(sym.arity()==1);
  CHECK_TERM(arg0);

  HashNumber hnr = COMBINE(SHIFT(addressf(sym)), arg0);

  const detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if ((sym==cur->function()) &&
         reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(1));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;

  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
const _aterm* term_appl2(const function_symbol& sym, const Term& arg0, const Term& arg1)
{
  assert(sym.arity()==2);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  HashNumber hnr = COMBINE(COMBINE(SHIFT(addressf(sym)), arg0),arg1);

  const detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(2));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
const _aterm* term_appl3(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2)
{

  assert(sym.arity()==3);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  HashNumber hnr = COMBINE(COMBINE(COMBINE(SHIFT(addressf(sym)), arg0),arg1),arg2);

  const detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[2] == arg2)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(3));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
const _aterm *term_appl4(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2, const Term& arg3)
{
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  assert(sym.arity()==4);

  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(SHIFT(addressf(sym)), arg0), arg1), arg2), arg3);

  const detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[2] == arg2 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[3] == arg3)
    {
      return cur;
    }
    cur = cur->next();
  }

  assert(!cur);

  cur = detail::allocate_term(TERM_SIZE_APPL(4));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[3])) Term(arg3);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
const _aterm* term_appl5(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2,
                                      const Term& arg3, const Term& arg4)
{

  assert(sym.arity()==5);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);


  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(SHIFT(addressf(sym)), arg0), arg1), arg2), arg3), arg4);

  const detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[2] == arg2 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[3] == arg3 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[4] == arg4)
    {
      return cur;
    }
    cur = cur->next();
  }

  assert(!cur);

  cur = detail::allocate_term(TERM_SIZE_APPL(5));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[3])) Term(arg3);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[4])) Term(arg4);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
const _aterm *term_appl6(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2,
                                      const Term& arg3, const Term& arg4, const Term& arg5)
{
  assert(sym.arity()==6);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);
  CHECK_TERM(arg5);

  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(SHIFT(addressf(sym)), arg0), arg1), arg2), arg3), arg4), arg5);

  const detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[2] == arg2 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[3] == arg3 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[4] == arg4 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[5] == arg5)
    {
      return cur;
    }
    cur = cur->next();
  }

  assert(!cur);

  cur = detail::allocate_term(TERM_SIZE_APPL(6));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;

  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[3])) Term(arg3);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[4])) Term(arg4);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[5])) Term(arg5);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
const _aterm *term_appl7(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2,
                                      const Term& arg3, const Term& arg4, const Term& arg5, const Term& arg6)
{
  assert(sym.arity()==7);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);
  CHECK_TERM(arg5);
  CHECK_TERM(arg6);

  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(SHIFT(addressf(sym)), arg0), arg1), arg2), arg3), arg4), arg5), arg6);

  const detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[2] == arg2 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[3] == arg3 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[4] == arg4 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[5] == arg5 &&
        reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[6] == arg6)
    {
      return cur;
    }
    cur = cur->next();
  }

  assert(!cur);

  cur = detail::allocate_term(TERM_SIZE_APPL(7));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;

  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[3])) Term(arg3);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[4])) Term(arg4);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[5])) Term(arg5);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[6])) Term(arg6);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}
} //namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H
