#ifndef MEMORY_H
#define MEMORY_H

#include "aterm2.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#define MAX_BLOCKS_PER_SIZE 1024

#define MAX_INLINE_ARITY    ((1 << ARITY_BITS)-2)

/* To change the block size, modify BLOCK_SHIFT only! */
#define BLOCK_SHIFT      13

#define BLOCK_SIZE       (1<<BLOCK_SHIFT)
#define BLOCK_TABLE_SIZE 4099     /* nextprime(4096) */

#ifdef AT_64BIT 
#define FOLD(w)         ((HN(w)) ^ (HN(w) >> 32))
#define PTR_ALIGN_SHIFT	4
#else
#define FOLD(w)         (HN(w))
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

  unsigned int size;
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

#define AT_BLOCK      0
#define AT_OLD_BLOCK  1
  
typedef struct TermInfo {
  Block*       at_blocks[2];
  header_type* top_at_blocks;
  int          at_nrblocks;
  ATerm        at_freelist;
  int          nb_live_blocks_before_last_gc;
  int          nb_reclaimed_blocks_during_last_gc;
  int          nb_reclaimed_cells_during_last_gc;
} TermInfo;

extern TermInfo *terminfo;
  
extern Block *at_freeblocklist;
extern unsigned int at_freeblocklist_size;
#define SIZE_TO_BYTES(size) ((size)*sizeof(header_type))

extern BlockBucket block_table[BLOCK_TABLE_SIZE];

extern int nb_minor_since_last_major;
extern int old_bytes_in_young_blocks_after_last_major;
extern int old_bytes_in_old_blocks_after_last_major;
extern int old_bytes_in_young_blocks_since_last_major;

extern unsigned int maxTermSize;

extern header_type *min_heap_address;
extern header_type *max_heap_address;
#define AT_isPotentialTerm(term) (min_heap_address <= (header_type*)(term) && (header_type*)(term) <= max_heap_address)

void AT_initMemory(unsigned int argc, char *argv[]);
void AT_cleanupMemory();
HashNumber AT_hashnumber(ATerm t);
ATerm AT_allocate(unsigned int size);
void  AT_freeTerm(unsigned int size, ATerm t);
ATbool AT_isValidTerm(ATerm term);
ATerm AT_isInsideValidTerm(ATerm term);
void  AT_validateFreeList(unsigned int size);
int AT_inAnyFreeList(ATerm t);
void AT_printAllTerms(FILE *file);
void AT_printAllAFunCounts(FILE *file);
unsigned long AT_getAllocatedCount();

#define AT_getMaxTermSize() (maxTermSize)

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
