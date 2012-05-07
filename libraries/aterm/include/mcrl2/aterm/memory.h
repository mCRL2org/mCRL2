#ifndef MEMORY_H
#define MEMORY_H

#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/aterm/architecture.h"
#include "mcrl2/aterm/aterm2.h"

namespace aterm
{

// static const size_t MAX_INLINE_ARITY = ((size_t)1 << ARITY_BITS)-(size_t)2;


/* To change the block size, modify BLOCK_SHIFT only! */
static const size_t BLOCK_SHIFT = 13;

static const size_t BLOCK_SIZE = 1<<BLOCK_SHIFT;

static const size_t BLOCK_TABLE_SIZE = 4099; /* nextprime(4096) */


template <typename T>
inline HashNumber HN(const T i)
{
  return (HashNumber)(i);
}

inline HashNumber HN(const _ATerm *i)
{
  return (HashNumber)(&*i);
}

inline HashNumber HN(const _ATermList *i)
{
  return (HashNumber)(i);
}

inline HashNumber HN(const _ATermInt *i)
{
  return (HashNumber)(i);
}

inline HashNumber HN(const _ATermAppl *i)
{
  return (HashNumber)(i);
}


#ifdef AT_64BIT
inline
size_t FOLD(const MachineWord w)
{
  return HN(w)^(HN(w) >> 32);
}

static const size_t PTR_ALIGN_SHIFT = 4;

#endif // AT_64BIT

#ifdef AT_32BIT
inline
size_t FOLD(const MachineWord w)
{
  return HN(w);
}

static const size_t PTR_ALIGN_SHIFT = 2;

#endif // AT_32BIT

inline size_t ADDR_TO_BLOCK_IDX(const void* a)
{
  return (((HashNumber)(a))>>(BLOCK_SHIFT+PTR_ALIGN_SHIFT)) % BLOCK_TABLE_SIZE;
}

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

typedef struct BlockBucket
{
  struct Block* first_before;
  struct Block* first_after;
} BlockBucket;

typedef struct TermInfo
{
  Block*       at_block;
  header_type* top_at_blocks;
  _ATerm*       at_freelist;
} TermInfo;

extern TermInfo* terminfo;

extern Block* at_freeblocklist;
extern size_t at_freeblocklist_size;

inline size_t SIZE_TO_BYTES(const size_t size)
{
  return size*sizeof(header_type);
}

extern BlockBucket block_table[BLOCK_TABLE_SIZE];

extern size_t maxTermSize;

extern header_type* min_heap_address;
extern header_type* max_heap_address;

inline
bool AT_isPotentialTerm(const _ATerm *term)
{
  return min_heap_address <= (header_type*)(&*term) && (header_type*)(&*term) <= max_heap_address;
}


void AT_initMemory();
void AT_initMemmgnt();
void AT_cleanupMemory();
HashNumber AT_hashnumber(const _ATerm* t);
_ATerm* AT_allocate(const size_t size);
void  AT_freeTerm(const size_t size, const _ATerm *t);
bool AT_isValidTerm(const _ATerm *term);
// _ATerm *AT_isInsideValidTerm(const _ATerm *term);
size_t AT_inAnyFreeList(const _ATerm *t);

/* struct _ATprotected_block
{
  ATerm* term;                     / * Pointer to the allocated block * /
  size_t size;                     / * Size of the allocated block, in bytes * /
  size_t protsize;                 / * Protected size (the actual size that is in use) * /
  struct _ATprotected_block* next, *prev; / * Chain * /
};
typedef struct _ATprotected_block* ATprotected_block;
*/

size_t AT_getMaxTermSize();

/*{{{  defines */

static const size_t INITIAL_TERM_TABLE_CLASS = 17;

/* inline
void CHECK_ARGUMENT(const ATermAppl / *t* /, const size_t / *n* /)
{
} */

inline
void CHECK_ARITY(const size_t ari1, const size_t ari2)
{
  assert(ari1 == ari2);
}

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
void CHECK_TERM(const _ATerm *t)
{
  assert(&*t != NULL && (AT_isValidTerm(t)));
  assert(t->reference_count>0);
}

/*}}}  */

extern HashNumber table_mask;

template <class Term>
template <class ForwardIterator, class ATermConverter>
term_appl<Term>::term_appl(const AFun &sym, ForwardIterator begin, ForwardIterator end, ATermConverter convert_to_aterm)
{
  const size_t arity = sym.arity();

  HashNumber hnr = START(sym.number());
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(arguments, ATerm, arity);
  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    new (&arguments[j]) ATerm(convert_to_aterm(*i)); // Placement new.
    _ATerm* arg = &*arguments[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, HN(arg));
  }
  assert(j==arity);
  hnr = FINISH(hnr);

  _ATerm* cur = ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==sym)
    {
      bool found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<_ATermAppl*>(cur)->arg[i] != &*arguments[i])
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
    cur = (_ATermAppl*) AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    
    for (size_t i=0; i<arity; i++)
    {
      _ATerm* arg = &*arguments[i];
      // arg->reference_count++; The reference counts do not need to be increased.
      //                         These are inherited from the array `arguments', which is
      //                         not properly deleted from the stack.
      reinterpret_cast<_ATermAppl*>(cur)->arg[i] = arg;
    }
    cur->next = &*ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }
  
  m_term=cur;
  increase_reference_count<false>(m_term);
}


template <class Term>
template <class ForwardIterator>
term_appl<Term>::term_appl(const AFun &sym, ForwardIterator begin, ForwardIterator end)
{
  const size_t arity = sym.arity();

  HashNumber hnr = START(sym.number());
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(arguments, _ATerm*, arity);
  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    arguments[j]=&* *i; 
    _ATerm* arg = arguments[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, HN(arg));
  }
  assert(j==arity);
  hnr = FINISH(hnr);

  _ATerm* cur = ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==sym)
    {
      bool found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<_ATermAppl*>(cur)->arg[i] != arguments[i])
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
    cur = (_ATermAppl*) AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    
    for (size_t i=0; i<arity; i++)
    {
      _ATerm* arg = &*arguments[i];
      arg->reference_count++; 
      reinterpret_cast<_ATermAppl*>(cur)->arg[i] = arg;
    }
    cur->next = &*ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }
  
  m_term=cur;
  increase_reference_count<false>(m_term);
}

/*{{{  ATermAppl ATmakeAppl0(AFun &sym) */

template <class Term>
// ATermAppl ATmakeAppl0(const AFun &sym)
term_appl<Term>::term_appl(const AFun &sym)
{
  _ATerm *cur, *prev, **hashspot;
  HashNumber hnr;


  CHECK_ARITY(ATgetArity(sym), 0);

  hnr = FINISH(START(sym.number()));

  prev = NULL;
  hashspot = &(ATerm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->m_function_symbol==sym)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next = cur->next;
        cur->next = (_ATerm*) &**hashspot;
        *hashspot = cur;
      }

      m_term=cur;
      increase_reference_count<false>(m_term);
      return;
    }
    prev = cur;
    cur = cur->next;
  }

  cur = (_ATermAppl*) &*AT_allocate(TERM_SIZE_APPL(0));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->m_function_symbol = sym;
  // AFun::increase_reference_count<true>(sym.number());
  cur->next = &*ATerm::hashtable[hnr];
  ATerm::hashtable[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl1(AFun &sym, ATerm &arg0) */

/**
 * Create an ATermAppl with one argument.
 */

// ATermAppl ATmakeAppl1(const AFun &sym, const ATerm &arg0)
template <class Term>
term_appl<Term>::term_appl(const AFun &sym, const Term &arg0)
{
  _ATerm* cur, *prev, **hashspot;
  // header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 1);
  CHECK_TERM(&*arg0);

  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = &(ATerm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if ((sym==cur->m_function_symbol) && 
         reinterpret_cast<_ATermAppl*>(cur)->arg[0] == &*arg0)
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
  // AFun::increase_reference_count<true>(sym.number());
  arg0->reference_count++;
  reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
  cur->next = ATerm::hashtable[hnr];
  ATerm::hashtable[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl2(AFun &sym, &arg0, &arg1) */

/**
 * Create an ATermAppl with one argument.
 */

// ATermAppl ATmakeAppl2(const AFun &sym, const ATerm &arg0, const ATerm &arg1)
template <class Term>
term_appl<Term>::term_appl(const AFun &sym, const Term &arg0, const Term &arg1)
{
  _ATerm* cur, *prev, **hashspot;
  // header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 2);

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = &(ATerm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->m_function_symbol==sym && 
        reinterpret_cast<_ATermAppl*>(cur)->arg[0] == &*arg0 && 
        reinterpret_cast<_ATermAppl*>(cur)->arg[1] == &*arg1)
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
  // AFun::increase_reference_count<true>(sym.number());
  arg0->reference_count++;
  reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
  arg1->reference_count++;
  reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;

  cur->next = ATerm::hashtable[hnr];
  ATerm::hashtable[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl3(AFun &sym, ATerm &arg0, &arg1, &arg2) */

/**
 * Create an ATermAppl with one argument.
 */

// ATermAppl ATmakeAppl3(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2)
template <class Term>
term_appl<Term>::term_appl(const AFun &sym, const Term &arg0, const Term &arg1, const Term &arg2)
{
  _ATerm *cur;
  // header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 3);

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] != &*arg0 ||
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] != &*arg1 ||
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] != &*arg2))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(3));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol=sym;
    // AFun::increase_reference_count<true>(sym.number());
    arg0->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] = &*arg2;

    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl4(AFun &sym, ATerm &arg0, &arg1, &arg2, &a3) */

/**
 * Create an ATermAppl with four arguments.
 */

// ATermAppl ATmakeAppl4(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2, const ATerm &arg3)
template <class Term>
term_appl<Term>::term_appl(const AFun &sym, const Term &arg0, const Term &arg1, const Term &arg2, const Term &arg3)
{
  _ATerm* cur;
  // header_type header;
  HashNumber hnr;

  // header = APPL_HEADER(sym.number());

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  CHECK_TERM(&*arg3);
  CHECK_ARITY(ATgetArity(sym), 4);

  hnr = START(sym.number());
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = COMBINE(hnr, HN(&*arg3));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[0] != &*arg0 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[1] != &*arg1 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[2] != &*arg2 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[3] != &*arg3))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(4));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    // AFun::increase_reference_count<true>(sym.number());
    arg0->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[3] = &*arg3;

    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl5(AFun &sym, ATerm &arg0, &arg1, &arg2, &a3, &a4) */

/**
 * Create an ATermAppl with five arguments.
 */

/* ATermAppl ATmakeAppl5(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                                       const ATerm &arg3, const ATerm &arg4) */
template <class Term>
term_appl<Term>::term_appl(const AFun &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4)
{
  _ATerm *cur;
  // header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 5);
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

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[0] != &*arg0 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[1] != &*arg1 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[2] != &*arg2 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[3] != &*arg3 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[4] != &*arg4))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(5));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    // AFun::increase_reference_count<true>(sym.number());
    arg0->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[3] = &*arg3;
    arg4->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[4] = &*arg4;

    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl6(AFun &sym, ATerm &arg0, &arg1, &arg2, &a3, &a4, &a5) */

/**
 * Create an ATermAppl with six arguments.
 */

/* ATermAppl ATmakeAppl6(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                                      const ATerm &arg3, const ATerm &arg4, const ATerm &arg5) */
template <class Term>
term_appl<Term>::term_appl(const AFun &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4, const Term &arg5)
{
  _ATerm* cur;
  // header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 6);
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

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=sym ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[0] != &*arg0 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[1] != &*arg1 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[2] != &*arg2 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[3] != &*arg3 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[4] != &*arg4 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[5] != &*arg5))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(6));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    // AFun::increase_reference_count<true>(sym.number());
    arg0->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[3] = &*arg3;
    arg4->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[4] = &*arg4;
    arg5->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[5] = &*arg5;

    cur->next =ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */


template <class Term>
term_appl<Term> term_appl<Term>::set_argument(const Term &arg, const size_t n) const
{
  const size_t sym_number = ATgetAFun(*this);
  
  bool found;

  size_t arity = ATgetArity(sym_number);
  assert(n < arity);

  HashNumber hnr = START((*this)->m_function_symbol.number());
  for (size_t i=0; i<arity; i++)
  {
    if (i!=n)
    {
      hnr = COMBINE(hnr, (HashNumber)((*this)->arg[i]));
    }
    else
    {
      hnr = COMBINE(hnr, (HashNumber)&*arg);
    }
  }

  hnr = FINISH(hnr);

  _ATerm *cur = ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==(*this)->m_function_symbol)
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (i!=n)
        {
          if (reinterpret_cast<_ATermAppl*>(cur)->arg[i]!=(&*(*this))->arg[i])
          {
            found = false;
            break;
          }
        }
        else
        {
          if (reinterpret_cast<_ATermAppl*>(cur)->arg[i]!=&*arg)
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
    // AFun::increase_reference_count<true>(sym_number);
    for (size_t i=0; i<arity; i++)
    {
      if (i!=n)
      {
        reinterpret_cast<_ATermAppl*>(cur)->arg[i] = (*this)->arg[i];
      }
      else
      {
        reinterpret_cast<_ATermAppl*>(cur)->arg[i] = &*arg;
      }
      reinterpret_cast<_ATermAppl*>(cur)->arg[i]->reference_count++;
    }
    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  // m_term=cur;
  // increase_reference_count<false>(m_term);
  return reinterpret_cast<_ATermAppl*>(cur);
}


template <class Term>
term_list<Term>::term_list(const term_list<Term> &tail, const Term &el)
{
  HashNumber hnr;
  _ATerm* cur;

  assert(ATgetType(tail) == AT_LIST);

  hnr = START(AS_LIST.number());
  hnr = COMBINE(hnr, HN(&*el));
  hnr = COMBINE(hnr, HN(&*tail));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=AS_LIST
                      || reinterpret_cast<_ATermList*>(cur)->head != &*el
                      || reinterpret_cast<_ATermList*>(cur)->tail != &*tail))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_LIST);
    /* Hashtable might be resized, so delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = AS_LIST;
    el->reference_count++;
    reinterpret_cast<_ATermList*>(cur)->head = &*el;
    tail->reference_count++;
    reinterpret_cast<_ATermList*>(cur)->tail = &*tail;
    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  // return reinterpret_cast<_ATermList*>(cur);
  m_term=cur;
  increase_reference_count<false>(m_term);
}


} // namespace aterm

#endif
