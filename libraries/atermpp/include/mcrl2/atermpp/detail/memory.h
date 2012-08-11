#ifndef MEMORY_H
#define MEMORY_H

#include "mcrl2/exception.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/architecture.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_int.h"

namespace atermpp
{

namespace detail
{
  extern size_t aterm_table_mask;
  extern detail::_aterm* * aterm_hashtable;


  extern aterm static_undefined_aterm;
  extern aterm static_empty_aterm_list;
 
  void initialise_administration();
}
  
inline
detail::_aterm *aterm::undefined_aterm()
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
detail::_aterm *aterm::empty_aterm_list()
{
  if (detail::static_empty_aterm_list.m_term==NULL || detail::static_empty_aterm_list==detail::static_undefined_aterm )
  {
    detail::initialise_administration();
    new (&detail::static_empty_aterm_list) aterm(detail::function_adm.AS_EMPTY_LIST); // Use placement new as static_empty_atermlist
                                                                              // may not have initialised when this is called, 
                                                                              // causing a problem with reference counting.
  }
  return detail::static_empty_aterm_list.m_term;
} 

namespace detail
{

inline
size_t TERM_SIZE_APPL(const size_t arity)
{
  return (sizeof(detail::_aterm)/sizeof(size_t))+arity;
}

inline
size_t COMBINE(const HashNumber hnr, const size_t w)
{
  return ((hnr)<<1 ^(hnr)>>1 ^ w);
}

inline
size_t COMBINE(const HashNumber hnr, const aterm &w)
{
  return COMBINE(hnr,reinterpret_cast<size_t>(&*w));
}

inline
void CHECK_TERM(const aterm &
#ifndef NDEBUG 
t
#endif
)
{
  assert(t != aterm());
  assert(t->reference_count()>0);
  assert(t->function().name().size()!=0);
}

inline size_t detail::term_size(const detail::_aterm *t)
{
  if (t->function().number()==detail::function_adm.AS_INT.number())
  {
    return TERM_SIZE_INT;
  }
  return TERM_SIZE_APPL(t->function().arity());
}

inline HashNumber detail::hash_number(const detail::_aterm *t, const size_t size)
{
  HashNumber hnr = t->function().number();
  for (size_t i=TERM_SIZE_APPL(0); i<size; i++)
  {
    hnr = COMBINE(hnr, *(reinterpret_cast<const size_t *>(t) + i));
  }

  return hnr;
}

template <class Term, class InputIterator, class ATermConverter>
_aterm* local_term_appl_with_converter(const function_symbol &sym, const InputIterator begin, const InputIterator end, ATermConverter convert_to_aterm)
{
  const size_t arity = sym.arity();

  HashNumber hnr = sym.number();
  // It is assumed that the aterm array is not initialised with terms.
  // It is not clear whether this holds for all compilers on all platforms.
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(arguments, aterm, arity); 
  // std::vector <aterm> arguments(arity);
  
  size_t j=0;
  for (InputIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    new (&arguments[j]) aterm(convert_to_aterm(*i));
    const aterm &arg = arguments[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, arg);
  }
  assert(j==arity);

  detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i] != arguments[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next();
  }

  if (!cur)
  {
    cur = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    cur->function() = sym;
    
    for (size_t i=0; i<arity; i++)
    {
      new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term(arguments[i]);
    }
    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }
  

  for (size_t j=0; j!=arity; ++j)
  {
    using namespace atermpp;
    arguments[j].~aterm();
  }
  
  return cur;
}

template <class Term, class ForwardIterator>
_aterm* local_term_appl(const function_symbol &sym, const ForwardIterator begin, const ForwardIterator end)
{
  const size_t arity = sym.arity();
  HashNumber hnr = sym.number();
  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    CHECK_TERM(*i);
    hnr = COMBINE(hnr, reinterpret_cast<size_t>(&* *i));
  }
  assert(j==arity);

  detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      ForwardIterator i=begin;
      for (size_t j=0; j<arity; ++i,++j)
      {
        if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[j] != *i)
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next();
  }

  if (!cur)
  {
    cur = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    cur->function() = sym;
    
    ForwardIterator i=begin;
    for (size_t j=0; j<arity; ++i, ++j)
    {
      new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[j])) Term(*i);
    }
    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }
  
  return cur;
}

template <class Term>
_aterm* term_appl1(const function_symbol &sym, const Term &arg0)
{
  detail::_aterm* cur, *prev, **hashspot;
  assert(sym.arity()==1);
  CHECK_TERM(arg0);

  HashNumber hnr = COMBINE(sym.number(), arg0);

  prev = NULL;
  hashspot = &(detail::aterm_hashtable[hnr & detail::aterm_table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if ((sym==cur->function()) && 
         reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next() = cur->next();
        cur->next() = *hashspot;
        *hashspot = cur;
      }
      return cur;
    }
    prev = cur;
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(1));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  cur->function() = sym;
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  cur->next() = detail::aterm_hashtable[hnr];
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

template <class Term>
_aterm* term_appl2(const function_symbol &sym, const Term &arg0, const Term &arg1)
{
  detail::_aterm* cur, *prev, **hashspot;

  assert(sym.arity()==2);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  HashNumber hnr = COMBINE(COMBINE(sym.number(), arg0),arg1);

  prev = NULL;
  hashspot = &(detail::aterm_hashtable[hnr & detail::aterm_table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->function()==sym && 
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 && 
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next() = cur->next();
        cur->next() = *hashspot;
        *hashspot = cur;
      }
      return cur;
    }
    prev = cur;
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(2));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  cur->function() = sym;
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);

  cur->next() = detail::aterm_hashtable[hnr];
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

template <class Term>
_aterm* term_appl3(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2)
{

  assert(sym.arity()==3);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  HashNumber hnr = COMBINE(COMBINE(COMBINE(sym.number(), arg0), arg1), arg2);

  detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur && (cur->function()!=sym ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2))
  {
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(3));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    cur->function()=sym;
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);

    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return cur;
}

template <class Term>
_aterm *term_appl4(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2, const Term &arg3)
{
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  assert(sym.arity()==4);

  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(sym.number(), arg0), arg1), arg2), arg3);

  detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur && (cur->function()!=sym ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3))
  {
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(4));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    cur->function() = sym;
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);

    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return cur;
}

template <class Term>
_aterm* term_appl5(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4)
{

  assert(sym.arity()==5);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);


  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(sym.number(), arg0), arg1), arg2), arg3), arg4);

  detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur && (cur->function()!=sym ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] != arg4))
  {
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(5));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    cur->function() = sym;
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4])) Term(arg4);

    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return cur;
}

template <class Term>
_aterm *term_appl6(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4, const Term &arg5)
{
  assert(sym.arity()==6);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);
  CHECK_TERM(arg5);

  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(sym.number(), arg0), arg1), arg2), arg3), arg4), arg5);

  detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur && (cur->function()!=sym ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] != arg4 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5] != arg5))
  {
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(6));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    cur->function() = sym;
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4])) Term(arg4);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5])) Term(arg5);

    cur->next() =detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return cur;
}
} //namespace detail

template <class Term>
term_appl<Term> term_appl<Term>::set_argument(const Term &arg, const size_t n) 
{
  bool found;

  size_t arity = function().arity();
  assert(n < arity);

  HashNumber hnr = (*this)->function().number();
  for (size_t i=0; i<arity; i++)
  {
    if (i!=n)
    {
      hnr = detail::COMBINE(hnr, ((*this)->arg[i]));
    }
    else
    {
      hnr = detail::COMBINE(hnr, arg);
    }
  }


  detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==(*this)->function())
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (i!=n)
        {
          if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i]!=(*this)->arg[i])  
          {
            found = false;
            break;
          }
        }
        else
        {
          if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i]!=arg)
          {
            found = false;
            break;
          }
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(detail::TERM_SIZE_APPL(arity));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    cur->function() = (*this)->function();
    for (size_t i=0; i<arity; i++)
    {
      if (i!=n)
      {
        new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term((*this)->arg[i]);
      }
      else
      {
        new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term(arg);
      }
    }
    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return reinterpret_cast<detail::_aterm_appl<Term>*>(cur);
}


template <class Term>
term_list<Term> push_front(const term_list<Term> &tail, const Term &el)
{
  return aterm_cast<const term_list<Term> > (term_appl<aterm> (detail::function_adm.AS_LIST,el,tail));
}


} // namespace atermpp

#endif
