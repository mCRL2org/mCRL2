#ifndef MEMORY_H
#define MEMORY_H

#include "mcrl2/aterm/architecture.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/gc.h"

/* Uncomment the second line, to do a garbage collection before a term
 * is constructed. This can be used to find subtle garbage collection problems.
 * Note that this is very time consuming... The first line below expresses that
 * no agressive checking is done, which is the default. */
// #define DO_AGGRESSIVE_GARBAGE_COLLECT

#ifdef DO_AGGRESSIVE_GARBAGE_COLLECT
#define AGGRESSIVE_GARBAGE_COLLECT_CHECK AT_collect()
#else
#define AGGRESSIVE_GARBAGE_COLLECT_CHECK
#endif


namespace aterm
{

static const size_t MAX_INLINE_ARITY = ((size_t)1 << ARITY_BITS)-(size_t)2;


/* To change the block size, modify BLOCK_SHIFT only! */
static const size_t BLOCK_SHIFT = 13;

static const size_t BLOCK_SIZE = 1<<BLOCK_SHIFT;

static const size_t BLOCK_TABLE_SIZE = 4099; /* nextprime(4096) */


template <typename T>
inline HashNumber HN(const T i)
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
  ATerm        at_freelist;
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
bool AT_isPotentialTerm(const ATerm term)
{
  return min_heap_address <= (header_type*)(term) && (header_type*)(term) <= max_heap_address;
}


void AT_initMemory();
void AT_initMemmgnt();
void AT_cleanupMemory();
HashNumber AT_hashnumber(const ATerm t);
ATerm AT_allocate(const size_t size);
void  AT_freeTerm(const size_t size, const ATerm t);
bool AT_isValidTerm(const ATerm term);
ATerm AT_isInsideValidTerm(ATerm term);
size_t AT_inAnyFreeList(const ATerm t);

struct _ATprotected_block
{
  ATerm* term;                     /* Pointer to the allocated block */
  size_t size;                     /* Size of the allocated block, in bytes */
  size_t protsize;                 /* Protected size (the actual size that is in use) */
  struct _ATprotected_block* next, *prev; /* Chain */
};
typedef struct _ATprotected_block* ATprotected_block;

/* Protected Memory management functions */
void* AT_malloc(const size_t size);
void* AT_calloc(const size_t nmemb, const size_t size);
void* AT_realloc(void* ptr, const size_t size);
void AT_free(void* ptr);
ATerm* AT_alloc_protected(const size_t nelem);
void AT_free_protected(ATerm* term);
void AT_free_protected_blocks();

size_t AT_getMaxTermSize();

/*{{{  defines */

static const size_t INITIAL_TERM_TABLE_CLASS = 17;

#ifndef NDEBUG
inline
void CHECK_HEADER(const header_type h)
{
  assert(!IS_MARKED(h));
}

inline
void CHECK_ARGUMENT(const ATermAppl /*t*/, const size_t /*n*/)
{
}

inline
void CHECK_ARITY(const size_t ari1, const size_t ari2)
{
  assert((ari1) == (ari2));
}

inline
void CHECK_TERM(const ATerm t)
{
  assert((t) != NULL && (AT_isValidTerm(t)));
}
#else
inline void CHECK_HEADER(const header_type) {}
inline void CHECK_ARGUMENT(const ATermAppl, const size_t) {}
inline void CHECK_ARITY(const size_t, const size_t) {}
inline void CHECK_TERM(const ATerm) {}
#endif

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

/*}}}  */

extern ATerm* hashtable;
extern HashNumber table_mask;

/*{{{  ATermAppl ATmakeAppl(AFun sym, ...) */

struct default_aterm_converter
{
  template <typename T>
  ATerm operator()(const T& x) const
  {
    return reinterpret_cast<ATerm>(x);
  }
};

/**
 * Create a new ATermAppl. The argument count can be found in the symbol.
 */

template <class ForwardIterator, class ATermConverter>
ATermAppl ATmakeAppl(const AFun sym, const ForwardIterator begin, const ForwardIterator end, ATermConverter convert_to_aterm = default_aterm_converter())
{
  ATermAppl cur;
  size_t arity = ATgetArity(sym);
  bool found;
  header_type header;
  HashNumber hnr;
  ATerm arg;

  AGGRESSIVE_GARBAGE_COLLECT_CHECK;
  header = APPL_HEADER(arity > MAX_INLINE_ARITY ? MAX_INLINE_ARITY+1 : arity, sym);

  hnr = START(header);
  for (ForwardIterator i=begin; i!=end; i++)
  {
    arg = convert_to_aterm(*i);
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, HN(arg));
  }
  hnr = FINISH(hnr);

  cur = (ATermAppl)hashtable[hnr & table_mask];
  while (cur)
  {
    if (EQUAL_HEADER(cur->header,header))
    {
      found = true;
      size_t j=0;
      for (ForwardIterator i=begin; i!=end; i++,j++)
      {
        assert(j<arity);
        if (!ATisEqual(ATgetArgument(cur, j), convert_to_aterm(*i)))
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
    cur = (ATermAppl)cur->aterm.next;
  }

  if (!cur)
  {
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    size_t j=0;
    for (ForwardIterator i=begin; i!=end; i++,j++)
    {
      assert(j<arity);
      ATgetArgument(cur, j) = convert_to_aterm(*i);
    }
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
}

} // namespace aterm

#endif
