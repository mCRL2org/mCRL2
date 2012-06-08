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

inline
size_t TERM_SIZE_APPL(const size_t arity)
{
  return (sizeof(detail::_aterm)/sizeof(size_t))+arity;
}

/* The constants below are not static to prevent some compiler warnings */
const size_t MIN_TERM_SIZE = TERM_SIZE_APPL(0);
const size_t INITIAL_MAX_TERM_SIZE = 256;


/* To change the block size, modify BLOCK_SHIFT only! */
static const size_t BLOCK_SHIFT = 13;

static const size_t BLOCK_SIZE = 1<<BLOCK_SHIFT;

extern size_t total_nodes;

typedef struct Block
{
  /* We need platform alignment for this data block! */
  size_t data[BLOCK_SIZE];

  size_t size;
#ifndef NDEBUG
  struct Block* next_by_size;
#endif

  size_t* end;
} Block;

typedef struct TermInfo
{
  Block*       at_block;
  size_t* top_at_blocks;
  detail::_aterm*       at_freelist;

  TermInfo():at_block(NULL),top_at_blocks(NULL),at_freelist(NULL)
  {}

} TermInfo;

extern std::vector<TermInfo> terminfo;

static const size_t INITIAL_TERM_TABLE_CLASS = 17;

inline
size_t START(const size_t w)
{
  return w;
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
HashNumber FINISH(const HashNumber hnr)
{
  return hnr;
}

inline
void CHECK_TERM(const aterm &t)
{
  assert(t != aterm());
  assert(t->reference_count>0);
  assert(t->m_function_symbol.name().size()!=0);
}

extern HashNumber table_mask;

inline size_t detail::term_size(const detail::_aterm *t)
{
  if (t->m_function_symbol==AS_INT)
  {
    return TERM_SIZE_INT;
  }
  return TERM_SIZE_APPL(t->m_function_symbol.arity());
}

inline HashNumber detail::hash_number(const detail::_aterm *t, const size_t size)
{
  HashNumber hnr;

  hnr = START(t->m_function_symbol.number());
  for (size_t i=TERM_SIZE_APPL(0); i<size; i++)
  {
    hnr = COMBINE(hnr, *(reinterpret_cast<const size_t *>(t) + i));
  }

  return FINISH(hnr);
}



static void remove_from_hashtable(detail::_aterm *t)
{
  // fprintf(stderr,"Remove term from hashtable %p\n",t);
  detail::_aterm *prev=NULL, *cur;

  /* Remove the node from the hashtable */
  const HashNumber hnr = hash_number(t, term_size(t)) & table_mask;
  cur = detail::hashtable()[hnr];

  do
  {
    if (!cur)
    {
      throw mcrl2::runtime_error("free_term: cannot find term in hashtable."); // If only occurs if the internal administration is in error.
    }
    if (cur == t)
    {
      if (prev)
      {
        prev->next = cur->next;
      }
      else
      {
        detail::hashtable()[hnr] = cur->next;
      }
      /* Put the node in the appropriate free list */
      total_nodes--;
      return;
    }
  }
  while (((prev=cur), (cur=cur->next)));
  assert(0);
}

#ifndef NDEBUG
  bool check_that_all_objects_are_free();
#endif

inline void detail::free_term(detail::_aterm *t)
{
  const size_t function_symbol_index=t->m_function_symbol.number();
  if (function_symbol_index==AS_EMPTY_LIST.number())
  {
    return;
  }
  assert(t->reference_count==0);
  const size_t size=term_size(t);
  remove_from_hashtable(t);  // Remove from hash_table

  for(size_t i=0; i<t->m_function_symbol.arity(); ++i)
  {
    reinterpret_cast<detail::_aterm_appl<aterm> *>(t)->arg[i]=aterm();
  }
  size_t ref_count=function_symbol::at_lookup_table[function_symbol_index]->reference_count;
  t->m_function_symbol=function_symbol(); 

  TermInfo &ti = terminfo[size];
  t->next  = ti.at_freelist;
  ti.at_freelist = t; 

  if (function_symbol_index==AS_INT.number() && ref_count==1) // When destroying ATempty, it appears that all other terms have been removed.
  {
    assert(check_that_all_objects_are_free());
    return;
  }
}

template <class Term>
template <class ForwardIterator, class ATermConverter>
term_appl<Term>::term_appl(const function_symbol &sym, const ForwardIterator begin, const ForwardIterator end, ATermConverter convert_to_aterm)
{
  const size_t arity = sym.arity();

  HashNumber hnr = START(sym.number());
  std::vector <aterm> arguments(arity);
  
  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    arguments[j]=convert_to_aterm(*i);
    const aterm &arg = arguments[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, arg);
  }
  assert(j==arity);
  hnr = FINISH(hnr);

  detail::_aterm* cur = detail::hashtable()[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==sym)
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
    cur = cur->next;
  }

  if (!cur)
  {
    cur = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));
    /* Delay masking until after allocate_term */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    
    for (size_t i=0; i<arity; i++)
    {
      new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term(arguments[i]);
    }
    cur->next = detail::hashtable()[hnr];
    detail::hashtable()[hnr] = cur;
  }
  
  m_term=cur;
  increase_reference_count<false>(m_term);
}


template <class Term>
template <class ForwardIterator>
term_appl<Term>::term_appl(const function_symbol &sym, const ForwardIterator begin, const ForwardIterator end)
                     /*    typename boost::enable_if<
                           typename boost::is_convertible< typename boost::iterator_traversal< ForwardIterator >::type,
                           boost::forward_traversal_tag >::type >::type* / * = 0* /) */
{
  const size_t arity = sym.arity();
  HashNumber hnr = START(sym.number());
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(arguments, detail::_aterm*, arity);
  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    arguments[j]=&* *i; 
    const aterm &arg = arguments[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, arg);
  }
  assert(j==arity);
  hnr = FINISH(hnr);

  detail::_aterm* cur = detail::hashtable()[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==sym)
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
    cur = cur->next;
  }

  if (!cur)
  {
    cur = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));
    /* Delay masking until after allocate_term */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    
    for (size_t i=0; i<arity; i++)
    {
      new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term(arguments[i]);
    }
    cur->next = detail::hashtable()[hnr];
    detail::hashtable()[hnr] = cur;
  }
  
  m_term=cur;
  increase_reference_count<false>(m_term);
}

/**
 * Create an ATermAppl with one argument.
 */

template <class Term>
term_appl<Term>::term_appl(const function_symbol &sym, const Term &arg0)
{
  detail::_aterm* cur, *prev, **hashspot;

  assert(sym.arity()==1);
  CHECK_TERM(arg0);

  HashNumber hnr = START(sym.number());
  hnr = COMBINE(hnr, arg0);
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = &(detail::hashtable()[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if ((sym==cur->m_function_symbol) && 
         reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next = cur->next;
        cur->next = *hashspot;
        *hashspot = cur;
      }
      m_term=cur;
      increase_reference_count<false>(m_term);
      return;
    }
    prev = cur;
    cur = cur->next;
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(1));
  /* Delay masking until after allocate_term */
  hnr &= table_mask;
  cur->m_function_symbol = sym;
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  cur->next = detail::hashtable()[hnr];
  detail::hashtable()[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>(m_term);
}


/**
 * Create an ATermAppl with one argument.
 */

template <class Term>
term_appl<Term>::term_appl(const function_symbol &sym, const Term &arg0, const Term &arg1)
{
  detail::_aterm* cur, *prev, **hashspot;

  assert(sym.arity()==2);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  HashNumber hnr = START(sym.number());
  hnr = COMBINE(hnr, arg0);
  hnr = COMBINE(hnr, arg1);
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = &(detail::hashtable()[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->m_function_symbol==sym && 
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 && 
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next = cur->next;
        cur->next = *hashspot;
        *hashspot = cur;
      }
      m_term=cur;
      increase_reference_count<false>(m_term);
      return;
    }
    prev = cur;
    cur = cur->next;
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(2));
  /* Delay masking until after allocate_term */
  hnr &= table_mask;
  cur->m_function_symbol = sym;
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);

  cur->next = detail::hashtable()[hnr];
  detail::hashtable()[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>(m_term);
}

/**
 * Create an ATermAppl with one argument.
 */

template <class Term>
term_appl<Term>::term_appl(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2)
{

  assert(sym.arity()==3);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  HashNumber hnr = START(sym.number());
  hnr = COMBINE(hnr, arg0);
  hnr = COMBINE(hnr, arg1);
  hnr = COMBINE(hnr, arg2);
  hnr = FINISH(hnr);

  detail::_aterm *cur = detail::hashtable()[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(3));
    /* Delay masking until after allocate_term */
    hnr &= table_mask;
    cur->m_function_symbol=sym;
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);

    cur->next = detail::hashtable()[hnr];
    detail::hashtable()[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl4(function_symbol &sym, aterm &arg0, &arg1, &arg2, &a3) */

/**
 * Create an ATermAppl with four arguments.
 */

template <class Term>
term_appl<Term>::term_appl(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2, const Term &arg3)
{

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  assert(sym.arity()==4);

  HashNumber hnr = START(sym.number());
  hnr = COMBINE(hnr, arg0);
  hnr = COMBINE(hnr, arg1);
  hnr = COMBINE(hnr, arg2);
  hnr = COMBINE(hnr, arg3);
  hnr = FINISH(hnr);

  detail::_aterm* cur = detail::hashtable()[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(4));
    /* Delay masking until after allocate_term */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);

    cur->next = detail::hashtable()[hnr];
    detail::hashtable()[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
}

/**
 * Create an ATermAppl with five arguments.
 */

template <class Term>
term_appl<Term>::term_appl(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4)
{

  assert(sym.arity()==5);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);


  HashNumber hnr = START(sym.number());
  hnr = COMBINE(hnr, arg0);
  hnr = COMBINE(hnr, arg1);
  hnr = COMBINE(hnr, arg2);
  hnr = COMBINE(hnr, arg3);
  hnr = COMBINE(hnr, arg4);
  hnr = FINISH(hnr);

  detail::_aterm *cur = detail::hashtable()[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] != arg4))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(5));
    /* Delay masking until after allocate_term */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4])) Term(arg4);

    cur->next = detail::hashtable()[hnr];
    detail::hashtable()[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
}

/**
 * Create an ATermAppl with six arguments.
 */

template <class Term>
term_appl<Term>::term_appl(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4, const Term &arg5)
{
  assert(sym.arity()==6);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);
  CHECK_TERM(arg5);

  HashNumber hnr = START(sym.number());
  hnr = COMBINE(hnr, arg0);
  hnr = COMBINE(hnr, arg1);
  hnr = COMBINE(hnr, arg2);
  hnr = COMBINE(hnr, arg3);
  hnr = COMBINE(hnr, arg4);
  hnr = COMBINE(hnr, arg5);
  hnr = FINISH(hnr);

  detail::_aterm* cur = detail::hashtable()[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] != arg4 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5] != arg5))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(6));
    /* Delay masking until after allocate_term */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4])) Term(arg4);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5])) Term(arg5);

    cur->next =detail::hashtable()[hnr];
    detail::hashtable()[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
}

/*}}}  */


template <class Term>
term_appl<Term> term_appl<Term>::set_argument(const Term &arg, const size_t n) 
{
  bool found;

  size_t arity = function().arity();
  assert(n < arity);

  HashNumber hnr = START((*this)->m_function_symbol.number());
  for (size_t i=0; i<arity; i++)
  {
    if (i!=n)
    {
      hnr = COMBINE(hnr, ((*this)->arg[i]));
    }
    else
    {
      hnr = COMBINE(hnr, arg);
    }
  }

  hnr = FINISH(hnr);

  detail::_aterm *cur = detail::hashtable()[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==(*this)->m_function_symbol)
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
    cur = cur->next;
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(arity));
    /* Delay masking until after allocate_term */
    hnr &= table_mask;
    cur->m_function_symbol = (*this)->m_function_symbol;
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
    cur->next = detail::hashtable()[hnr];
    detail::hashtable()[hnr] = cur;
  }

  return reinterpret_cast<detail::_aterm_appl<Term>*>(cur);
}


template <class Term>
term_list<Term> push_front(const term_list<Term> &tail, const Term &el)
{
  return static_cast<term_list<Term> > (term_appl<aterm> (AS_LIST,el,tail));
}


} // namespace atermpp

#endif
