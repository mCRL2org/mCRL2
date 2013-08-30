#ifndef MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm_appl.h"

namespace atermpp
{

namespace detail
{

/* Free a term, without removing it from the
 *    hashtable, and destroying its function symbol */
inline void simple_free_term(const _aterm *t, const size_t arity)
{
  for(size_t i=0; i<arity; ++i)
  {
    reinterpret_cast<const _aterm_appl<aterm> *>(t)->arg[i].decrease_reference_count();
  }

  assert(t->reference_count()==1); // This term has a temporary protection.
  TermInfo &ti = terminfo[TERM_SIZE_APPL(arity)];
  t->set_next(ti.at_freelist);
  ti.at_freelist = t;
  t->set_reference_count_indicates_in_freelist();
}


template <class Term, class InputIterator, class ATermConverter>
const _aterm* local_term_appl_with_converter(const function_symbol &sym, 
                                       const InputIterator begin, 
                                       const InputIterator end, 
                                       const ATermConverter &convert_to_aterm)
{
  const size_t arity = sym.arity();

  HashNumber hnr = SHIFT(addressf(sym));
  
  /* The term is already partly constructed initially. If
     it turns out that the term already exists, this skeleton is freed
     using simple_free_term. Otherwise, the new_term is finished
     and a it is returned. */ 

  const detail::_aterm* new_term = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));
  new_term->increase_reference_count();  // Protect against premature garbage collection.
  
  size_t j=0;
  for (InputIterator i=begin; i!=end; ++i, ++j)
  {
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(new_term))->arg[j])) Term(convert_to_aterm(*i));
    const aterm &arg = reinterpret_cast<const detail::_aterm_appl<Term>*>(new_term)->arg[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, arg);
  }
  assert(j==arity);

  hnr &= detail::aterm_table_mask;
  const detail::_aterm* cur = detail::aterm_hashtable[hnr];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[i] != 
                  reinterpret_cast<const detail::_aterm_appl<Term>*>(new_term)->arg[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        simple_free_term(new_term,arity);
        return cur;
      }
    }
    cur = cur->next();
  }

  assert(cur==NULL);
  new (&const_cast<detail::_aterm*>(const_cast<detail::_aterm*>(new_term))->function()) function_symbol(sym);
    
  new_term->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = new_term;
  
  new_term->reset_reference_count(false); // Remove temporary protection of this term.
  return new_term;
}

// The functions below are used to obtain an address of objects that due to
// template arguments can have different types.

inline const _aterm* ADDRESS(const _aterm* a)
{
  return a;
}

inline const _aterm* ADDRESS(const aterm &a)
{
  return address(a);
}

template <class Term, class ForwardIterator>
const _aterm* local_term_appl(const function_symbol &sym, const ForwardIterator begin, const ForwardIterator end)
{
  const size_t arity = sym.arity();
  HashNumber hnr = SHIFT(addressf(sym)); 

  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    CHECK_TERM(*i);
    hnr = COMBINE(hnr, reinterpret_cast<size_t>(ADDRESS(*i)));
  }
  assert(j==arity);

  const detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      ForwardIterator i=begin;
      for (size_t j=0; j<arity; ++i,++j)
      {
        if (address(reinterpret_cast<const detail::_aterm_appl<Term>*>(cur)->arg[j]) != detail::ADDRESS(*i)) 
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        return cur;
      }
    }
    cur = cur->next();
  }

  assert(cur==NULL);
  cur = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  
  ForwardIterator i=begin;
  for (size_t j=0; j<arity; ++i, ++j)
  {
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[j])) Term(*i);
  }
  cur->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = cur;
  
  
  return cur;
}

template <class Term>
const _aterm* term_appl1(const function_symbol &sym, const Term &arg0)
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
  cur->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

template <class Term>
const _aterm* term_appl2(const function_symbol &sym, const Term &arg0, const Term &arg1)
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

  cur->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

template <class Term>
const _aterm* term_appl3(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2)
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

  cur->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

template <class Term>
const _aterm *term_appl4(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2, const Term &arg3)
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

  cur->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = cur;
  
  return cur;
}

template <class Term>
const _aterm* term_appl5(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4)
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

  cur->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = cur;
  
  return cur;
}

template <class Term>
const _aterm *term_appl6(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4, const Term &arg5)
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

  cur->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}
} //namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H
