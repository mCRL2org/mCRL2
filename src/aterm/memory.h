#ifndef MEMORY_H
#define MEMORY_H

#include "aterm2.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#define MAX_ARITY            256
#define MIN_TERM_SIZE          2
#define MAX_TERM_SIZE       (MAX_ARITY+3)
#define MAX_BLOCKS_PER_SIZE 1024

#define MAX_INLINE_ARITY       6

/* To change the block size, modify BLOCK_SHIFT only! */
#define BLOCK_SHIFT      13

#define BLOCK_SIZE       (1<<BLOCK_SHIFT)
#define BLOCK_TABLE_SIZE 4099     /* nextprime(4096) */

#ifdef AT_64BIT 
#define FOLD(w)        ((HN(w)) ^ (HN(w) >> 32))
#define PTR_ALIGN_SHIFT	4
#else
#define FOLD(w)        (HN(w))
#define PTR_ALIGN_SHIFT	2
#endif

#define ADDR_TO_BLOCK_IDX(a) \
((((HashNumber)(a))>>(BLOCK_SHIFT+PTR_ALIGN_SHIFT)) % BLOCK_TABLE_SIZE)

#define SET_FROZEN(block) ((block)->frozen=1)
#define IS_FROZEN(block) ((block)->frozen==1)
#define CLEAR_FROZEN(block) ((block)->frozen=0)

typedef struct Block
{
  /* We need platform alignment for this data block! */
  header_type data[BLOCK_SIZE];

  int size;
  int frozen; /* this int is used as a boolean */
  struct Block *next_by_size;
  struct Block *next_before;
  struct Block *next_after;

  header_type *end;
} Block;

typedef struct BlockBucket
{
  struct Block *first_before;
  struct Block *first_after;
} BlockBucket;

extern Block *at_blocks[MAX_TERM_SIZE];
extern Block *at_old_blocks[MAX_TERM_SIZE];
extern header_type *top_at_blocks[MAX_TERM_SIZE];
extern Block *at_freeblocklist;
extern int at_freeblocklist_size;
#define SIZE_TO_BYTES(size) ((size)*sizeof(header_type))

extern int at_nrblocks[MAX_TERM_SIZE];
extern ATerm at_freelist[MAX_TERM_SIZE];
extern BlockBucket block_table[BLOCK_TABLE_SIZE];

extern int nb_minor_since_last_major;
extern int old_bytes_in_young_blocks_after_last_major;
extern int old_bytes_in_old_blocks_after_last_major;
extern int old_bytes_in_young_blocks_since_last_major;
extern int nb_live_blocks_before_last_gc[MAX_TERM_SIZE];
extern int nb_reclaimed_blocks_during_last_gc[MAX_TERM_SIZE];
extern int nb_reclaimed_cells_during_last_gc[MAX_TERM_SIZE];

extern header_type *min_heap_address;
extern header_type *max_heap_address;
#define AT_isPotentialTerm(term) (min_heap_address <= (header_type*)(term) && (header_type*)(term) <= max_heap_address)

void AT_initMemory(int argc, char *argv[]);
void AT_cleanupMemory();
HashNumber AT_hashnumber(ATerm t);
ATerm AT_allocate(int size);
void  AT_freeTerm(int size, ATerm t);
ATbool AT_isValidTerm(ATerm term);
ATerm AT_isInsideValidTerm(ATerm term);
void  AT_validateFreeList(int size);
int AT_inAnyFreeList(ATerm t);
void AT_printAllTerms(FILE *file);
void AT_printAllAFunCounts(FILE *file);
unsigned long AT_getAllocatedCount();

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
