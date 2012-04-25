#ifndef MEMORY_H
#define MEMORY_H


#include "mcrl2/aterm/architecture.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/gc.h"

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
  assert((ari1) == (ari2));
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

/*{{{  ATermAppl ATmakeAppl(AFun sym, ...) */

struct default_aterm_converter
{
  template <typename T>
  ATerm operator()(const T& x) const
  {
    return static_cast<ATerm>(x);
  }
};

/**
 * Create a new ATermAppl. The argument count can be found in the symbol.
 */

template <class ForwardIterator, class ATermConverter>
ATermAppl ATmakeAppl(const AFun &sym, const ForwardIterator begin, const ForwardIterator end, ATermConverter convert_to_aterm = default_aterm_converter())
{
  /* TODO: THE FORWARD ITERATOR IS HERE USED A NUMBER OF TIMES CONSECUTIVELY, WHICH IS NOT ALLOWED FOR A FORWARD ITERATOR */
  _ATerm* cur;
  size_t arity = ATgetArity(sym);
  bool found;
  header_type header;
  HashNumber hnr;
  _ATerm* arg;

  // header = APPL_HEADER(arity > MAX_INLINE_ARITY ? MAX_INLINE_ARITY+1 : arity, sym.number());
  header = APPL_HEADER(sym.number());
 
  hnr = START(header);
  for (ForwardIterator i=begin; i!=end; i++)
  {
    arg = &*convert_to_aterm(*i);
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, HN(arg));
  }
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (EQUAL_HEADER(cur->header,header))
    {
      found = true;
      size_t j=0;
      for (ForwardIterator i=begin; i!=end; i++,j++)
      {
        assert(j<arity);
        if (reinterpret_cast<_ATermAppl*>(cur)->arg[j] != &*convert_to_aterm(*i))
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
    cur->header = header;
    AFun::increase_reference_count<true>(sym.number());
    
    size_t j=0;
    for (ForwardIterator i=begin; i!=end; i++,j++)
    {
      assert(j<arity);
      arg = &*convert_to_aterm(*i);
      arg->reference_count++;
      reinterpret_cast<_ATermAppl*>(cur)->arg[j] = arg;
    }
    // cur->aterm.next = &*hashtable[hnr];
    cur->next = &*ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return (_ATermAppl*)cur;
}

template <class ForwardIterator>
ATermAppl ATmakeAppl(const AFun &sym, const ForwardIterator begin, const ForwardIterator end)
{
   return ATmakeAppl(sym, begin, end, default_aterm_converter());
}

} // namespace aterm

#endif
