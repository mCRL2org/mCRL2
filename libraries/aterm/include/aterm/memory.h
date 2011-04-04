#ifndef MEMORY_H
#define MEMORY_H

#include "aterm2.h"

namespace aterm
{

static const size_t MAX_BLOCKS_PER_SIZE = 1024;

static const size_t MAX_INLINE_ARITY = ((size_t)1 << ARITY_BITS)-(size_t)2;
//#define MAX_INLINE_ARITY    ((1 << ARITY_BITS)-2)

/* To change the block size, modify BLOCK_SHIFT only! */
static const size_t BLOCK_SHIFT = 13;

static const size_t BLOCK_SIZE = 1<<BLOCK_SHIFT;
//#define BLOCK_SIZE       (1<<BLOCK_SHIFT)
static const size_t BLOCK_TABLE_SIZE = 4099; /* nextprime(4096) */
//#define BLOCK_TABLE_SIZE 4099     /* nextprime(4096) */

template <typename T>
inline HashNumber HN(const T i)
{
  return (HashNumber)(i);
}
//#define HN(i)          ((HashNumber)(i))

#ifdef AT_64BIT
inline
size_t FOLD(const MachineWord w)
{
  return HN(w)^(HN(w) >> 32);
}
//#define FOLD(w)         ((HN(w)) ^ (HN(w) >> 32))
static const size_t PTR_ALIGN_SHIFT = 4;
//#define PTR_ALIGN_SHIFT 4
#else
inline
size_t FOLD(const MachineWord w)
{
  return HN(w);
}
//#define FOLD(w)         (HN(w))
static const size_t PTR_ALIGN_SHIFT = 2;
//#define PTR_ALIGN_SHIFT 2
#endif

inline size_t ADDR_TO_BLOCK_IDX(void* a)
{
  return (((HashNumber)(a))>>(BLOCK_SHIFT+PTR_ALIGN_SHIFT)) % BLOCK_TABLE_SIZE;
}
/*
#define ADDR_TO_BLOCK_IDX(a) \
  ((((HashNumber)(a))>>(BLOCK_SHIFT+PTR_ALIGN_SHIFT)) % BLOCK_TABLE_SIZE)
*/

typedef struct Block
{
  /* We need platform alignment for this data block! */
  header_type data[BLOCK_SIZE];

  size_t size;
  int frozen; /* this int is used as a boolean */
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

static const size_t AT_BLOCK = 0;
static const size_t AT_OLD_BLOCK = 1;
//#define AT_BLOCK      0
//#define AT_OLD_BLOCK  1

typedef struct TermInfo
{
  Block*       at_blocks[2];
  header_type* top_at_blocks;
  size_t       at_nrblocks;
  ATerm        at_freelist;
  size_t       nb_live_blocks_before_last_gc;
  size_t       nb_reclaimed_blocks_during_last_gc;
  size_t       nb_reclaimed_cells_during_last_gc;
} TermInfo;

extern TermInfo* terminfo;

extern Block* at_freeblocklist;
extern size_t at_freeblocklist_size;

inline size_t SIZE_TO_BYTES(const size_t size)
{
  return size*sizeof(header_type);
}
//#define SIZE_TO_BYTES(size) ((size)*sizeof(header_type))

inline
void SET_FROZEN(Block* block)
{
  block->frozen=1;
}
//#define SET_FROZEN(block) ((block)->frozen=1)

inline
bool IS_FROZEN(const Block* block)
{
  return block->frozen==1;
}
//#define IS_FROZEN(block) ((block)->frozen==1)

inline
void CLEAR_FROZEN(Block* block)
{
  block->frozen=0;
}
//#define CLEAR_FROZEN(block) ((block)->frozen=0)

extern BlockBucket block_table[BLOCK_TABLE_SIZE];

extern size_t nb_minor_since_last_major;
extern size_t old_bytes_in_young_blocks_after_last_major;
extern size_t old_bytes_in_old_blocks_after_last_major;
extern size_t old_bytes_in_young_blocks_since_last_major;

extern size_t maxTermSize;

extern header_type* min_heap_address;
extern header_type* max_heap_address;

inline
bool AT_isPotentialTerm(const ATerm term)
{
  return min_heap_address <= (header_type*)(term) && (header_type*)(term) <= max_heap_address;
}
//#define AT_isPotentialTerm(term) (min_heap_address <= (header_type*)(term) && (header_type*)(term) <= max_heap_address)

void AT_initMemory(int argc, char** argv);
void AT_cleanupMemory();
HashNumber AT_hashnumber(ATerm t);
ATerm AT_allocate(size_t size);
void  AT_freeTerm(size_t size, ATerm t);
bool AT_isValidTerm(ATerm term);
ATerm AT_isInsideValidTerm(ATerm term);
void  AT_validateFreeList(size_t size);
size_t AT_inAnyFreeList(ATerm t);
void AT_printAllTerms(FILE* file);
void AT_printAllAFunCounts(FILE* file);
size_t AT_getAllocatedCount();

struct _ATprotected_block
{
  ATerm* term;                     /* Pointer to the allocated block */
  size_t size;                     /* Size of the allocated block, in bytes */
  size_t protsize;                 /* Protected size (the actual size that is in use) */
  struct _ATprotected_block* next, *prev; /* Chain */
};
typedef struct _ATprotected_block* ATprotected_block;

/* Protected Memory management functions */
void* AT_malloc(size_t size);
void* AT_calloc(size_t nmemb, size_t size);
void* AT_realloc(void* ptr, size_t size);
void AT_free(void* ptr);
ATerm* AT_alloc_protected(size_t nelem);
void AT_free_protected(ATerm* term);
void AT_free_protected_blocks();

size_t AT_getMaxTermSize();
//#define AT_getMaxTermSize() (maxTermSize)

} // namespace aterm

#endif
