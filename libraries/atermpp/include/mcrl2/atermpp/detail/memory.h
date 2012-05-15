#ifndef MEMORY_H
#define MEMORY_H

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

static const size_t ARG_OFFSET = TERM_SIZE_APPL(0);

/* The constants below are not static to prevent some compiler warnings */
const size_t MIN_TERM_SIZE = TERM_SIZE_APPL(0);
const size_t INITIAL_MAX_TERM_SIZE = 256;


// static const size_t MAX_INLINE_ARITY = ((size_t)1 << ARITY_BITS)-(size_t)2;


/* To change the block size, modify BLOCK_SHIFT only! */
static const size_t BLOCK_SHIFT = 13;

static const size_t BLOCK_SIZE = 1<<BLOCK_SHIFT;

// static const size_t BLOCK_TABLE_SIZE = 4099; /* nextprime(4096) */


template <typename T>
inline HashNumber HN(const T i)
{
  return (HashNumber)(i);
}

template <class POINTER>
inline HashNumber HN(const POINTER *i)
{
  return (HashNumber)(i);
}

#ifdef AT_64BIT
inline
size_t FOLD(const MachineWord w)
{
  return HN(w)^(HN(w) >> 32);
}

#endif // AT_64BIT

#ifdef AT_32BIT
inline
size_t FOLD(const MachineWord w)
{
  return HN(w);
}

#endif // AT_32BIT

typedef struct Block
{
  /* We need platform alignment for this data block! */
  header_type data[BLOCK_SIZE];

  size_t size;
  struct Block* next_by_size;
  struct Block* next_before;
  struct Block* next_after;

  header_type* end;
} Block;

typedef struct TermInfo
{
  Block*       at_block;
  header_type* top_at_blocks;
  detail::_aterm*       at_freelist;

  TermInfo():at_block(NULL),top_at_blocks(NULL),at_freelist(NULL)
  {}

} TermInfo;

extern std::vector<TermInfo> terminfo;

extern Block* at_freeblocklist;
extern size_t at_freeblocklist_size;

inline size_t SIZE_TO_BYTES(const size_t size)
{
  return size*sizeof(header_type);
}

HashNumber AT_hashnumber(const detail::_aterm* t);
detail::_aterm* AT_allocate(const size_t size);
void  AT_freeTerm(const size_t size, const detail::_aterm *t);

/*{{{  defines */

static const size_t INITIAL_TERM_TABLE_CLASS = 17;

inline
size_t START(const MachineWord w)
{
  return FOLD(w);
}

inline
size_t COMBINE(const HashNumber hnr, const MachineWord w)
{
  return ((hnr)<<1 ^(hnr)>>1 ^ HN(FOLD(w)));
}

inline
HashNumber FINISH(const HashNumber hnr)
{
  return hnr;
}

inline
void CHECK_TERM(const detail::_aterm *t)
{
  assert(&*t != NULL);
  assert(t->reference_count>0);
}

/*}}}  */

extern HashNumber table_mask;

template <class Term>
template <class ForwardIterator, class ATermConverter>
term_appl<Term>::term_appl(const function_symbol &sym, ForwardIterator begin, ForwardIterator end, ATermConverter convert_to_aterm)
{
  const size_t arity = sym.arity();

  HashNumber hnr = START(sym.number());
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(arguments, aterm, arity);
  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    new (&arguments[j]) aterm(convert_to_aterm(*i)); // Placement new.
    detail::_aterm* arg = &*arguments[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, HN(arg));
  }
  assert(j==arity);
  hnr = FINISH(hnr);

  detail::_aterm* cur = aterm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==sym)
    {
      bool found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i] != &*arguments[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        for (size_t j=0; j<arity; j++)
        {
          arguments[j].decrease_reference_count();
        }
        break;
      }
    }
    cur = cur->next;
  }

  if (!cur)
  {
    cur = (detail::_aterm_appl<Term>*) AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    
    for (size_t i=0; i<arity; i++)
    {
      detail::_aterm* arg = &*arguments[i];
      // arg->reference_count++; The reference counts do not need to be increased.
      //                         These are inherited from the array `arguments', which is
      //                         not properly deleted from the stack.
      reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i] = arg;
    }
    cur->next = &*aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
  }
  
  m_term=cur;
  increase_reference_count<false>(m_term);
}


template <class Term>
template <class ForwardIterator>
term_appl<Term>::term_appl(const function_symbol &sym, ForwardIterator begin, ForwardIterator end)
                     /*      typename boost::enable_if<
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
    detail::_aterm* arg = arguments[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, HN(arg));
  }
  assert(j==arity);
  hnr = FINISH(hnr);

  detail::_aterm* cur = aterm::hashtable[hnr & table_mask];
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
    cur = (detail::_aterm_appl<Term>*) AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    
    for (size_t i=0; i<arity; i++)
    {
      /* detail::_aterm* arg = &*arguments[i];
      arg->reference_count++; 
      reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i] = arg; */
      new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term(arguments[i]);
    }
    cur->next = &*aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
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
  HashNumber hnr;

  assert(sym.arity()==1);
  CHECK_TERM(&*arg0);

  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = &(aterm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if ((sym==cur->m_function_symbol) && 
         reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == &*arg0)
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

  cur = AT_allocate(TERM_SIZE_APPL(1));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->m_function_symbol = sym;
  /* arg0->reference_count++;
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] = &*arg0; */
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  cur->next = aterm::hashtable[hnr];
  aterm::hashtable[hnr] = cur;

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
  HashNumber hnr;

  assert(sym.arity()==2);

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = &(aterm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->m_function_symbol==sym && 
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == &*arg0 && 
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] == &*arg1)
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

  cur = AT_allocate(TERM_SIZE_APPL(2));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->m_function_symbol = sym;
  /* arg0->reference_count++;
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] = &*arg0; */
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  /* arg1->reference_count++;
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] = &*arg1; */
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);

  cur->next = aterm::hashtable[hnr];
  aterm::hashtable[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>(m_term);
}

/**
 * Create an ATermAppl with one argument.
 */

template <class Term>
term_appl<Term>::term_appl(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2)
{
  detail::_aterm *cur;
  HashNumber hnr;

  assert(sym.arity()==3);

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = FINISH(hnr);

  cur = aterm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != &*arg0 ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != &*arg1 ||
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != &*arg2))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(3));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol=sym;
    /* arg0->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] = &*arg2;
    */
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);

    cur->next = aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
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
  detail::_aterm* cur;
  HashNumber hnr;

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  CHECK_TERM(&*arg3);
  assert(sym.arity()==4);

  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = COMBINE(hnr, HN(&*arg3));
  hnr = FINISH(hnr);

  cur = aterm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != &*arg0 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != &*arg1 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != &*arg2 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != &*arg3))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(4));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    /* arg0->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] = &*arg3;*/
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);

    cur->next = aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
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
  detail::_aterm *cur;
  HashNumber hnr;

  assert(sym.arity()==5);
  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  CHECK_TERM(&*arg3);


  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = COMBINE(hnr, HN(&*arg3));
  hnr = COMBINE(hnr, HN(&*arg4));
  hnr = FINISH(hnr);

  cur = aterm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != &*arg0 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != &*arg1 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != &*arg2 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != &*arg3 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] != &*arg4))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(5));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    /* arg0->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] = &*arg3;
    arg4->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] = &*arg4;*/
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4])) Term(arg4);

    cur->next = aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
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
  detail::_aterm* cur;
  HashNumber hnr;

  assert(sym.arity()==6);
  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  CHECK_TERM(&*arg3);
  CHECK_TERM(&*arg4);
  CHECK_TERM(&*arg5);

  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = COMBINE(hnr, HN(&*arg3));
  hnr = COMBINE(hnr, HN(&*arg4));
  hnr = COMBINE(hnr, HN(&*arg5));
  hnr = FINISH(hnr);

  cur = aterm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != &*arg0 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != &*arg1 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != &*arg2 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != &*arg3 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] != &*arg4 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5] != &*arg5))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(6));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    /* arg0->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] = &*arg3;
    arg4->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] = &*arg4;
    arg5->reference_count++;
    reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5] = &*arg5; */
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4])) Term(arg4);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5])) Term(arg5);

    cur->next =aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
}

/*}}}  */


template <class Term>
term_appl<Term> term_appl<Term>::set_argument(const Term &arg, const size_t n) const
{
  bool found;

  size_t arity = function().arity();
  assert(n < arity);

  HashNumber hnr = START((*this)->m_function_symbol.number());
  for (size_t i=0; i<arity; i++)
  {
    if (i!=n)
    {
      hnr = COMBINE(hnr, (HashNumber)&*((*this)->arg[i]));
    }
    else
    {
      hnr = COMBINE(hnr, (HashNumber)&*arg);
    }
  }

  hnr = FINISH(hnr);

  detail::_aterm *cur = aterm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==(*this)->m_function_symbol)
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (i!=n)
        {
          if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i]!=(&*(*this))->arg[i])
          {
            found = false;
            break;
          }
        }
        else
        {
          if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i]!=&*arg)
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
    cur = AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = (*this)->m_function_symbol;
    for (size_t i=0; i<arity; i++)
    {
      if (i!=n)
      {
        // reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i] = (*this)->arg[i];
        new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term((*this)->arg[i]);
      }
      else
      {
        // reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i] = &*arg;
        new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term(arg);
      }
      // reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i]->reference_count++;
    }
    cur->next = aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<detail::_aterm_appl<Term>*>(cur);
}


template <class Term>
term_list<Term> push_front(const term_list<Term> &tail, const Term &el)
{
  HashNumber hnr;

  assert(tail.type() == AT_LIST);

  hnr = START(AS_LIST.number());
  hnr = COMBINE(hnr, HN(&*el));
  hnr = COMBINE(hnr, HN(&*tail));
  hnr = FINISH(hnr);

  detail::_aterm_list<Term>* cur =reinterpret_cast<detail::_aterm_list<Term>*>(aterm::hashtable[hnr & table_mask]);
  while (cur && (cur->m_function_symbol!=AS_LIST
                      || cur->head != el
                      || cur->tail != tail))
  {
    cur = reinterpret_cast<detail::_aterm_list<Term>*>(cur->next);
  }

  if (!cur)
  {
    cur = reinterpret_cast<detail::_aterm_list<Term>*>(AT_allocate(detail::TERM_SIZE_LIST));
    /* Hashtable might be resized, so delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = AS_LIST;
    // el->reference_count++;
    new (&cur->head)  Term(el);
    // tail->reference_count++;
    new (&cur->tail) term_list<Term>(tail);
    cur->next = aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
  }

  return term_list<Term>(cur);
  // increase_reference_count<false>(m_term);
}


} // namespace atermpp

#endif
