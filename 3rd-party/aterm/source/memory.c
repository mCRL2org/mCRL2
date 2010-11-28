/*{{{  includes */

#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "_aterm.h"
#include "aterm2.h"
#include "memory.h"
#include "util.h"
#include "debug.h"
#include "gc.h"

#ifndef NDEBUG
#include <stdint.h>
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/*}}}  */
/*{{{  defines */

#define INITIAL_TERM_TABLE_CLASS      17

/* when less than GC_THRESHOLD percent of the terms has been freed by
   the previous garbage collect, a new block will be allocated.
   Otherwise a new garbage collect is started. */
#define GC_THRESHOLD 45

#define CHECK_HEADER(h) assert((GET_AGE(h)==0) && !IS_MARKED(h) )
#define CHECK_ARGUMENT(t,n) { assert( (GET_AGE(t->header) <= GET_AGE(ATgetArgument(t,n)->header)) );\
                              assert( (GET_AGE(t->header) <= GET_AGE(at_lookup_table[ATgetSymbol((ATermAppl)t)]->header))); }

#define MAX_DESTRUCTORS     16
#define MAX_BLOCKS_PER_SIZE 1024

#define TERM_HASH_OPT       "-at-termtable"
#define HASH_INFO_OPT       "-at-hashinfo"
#define TERM_CHECK_OPT      "-at-check"
#define TERM_SIZE_OPT       "-at-termsize"

#define CHECK_ARITY(ari1,ari2) DBG_ARITY(assert((ari1) == (ari2)))


#define INFO_HASHING    1
#define MAX_INFO_SIZES 256

#define HN(i)          ((HashNumber)(i))
#define IDX(w)         HN(((w>>2) ^ (w>>10)) & 0xFF)
#define SHIFT(w)       HN((w>>3) & 0xF)

#define START(w)       FOLD(HIDE_AGE_MARK(w))
#define COMBINE(hnr,w) ((hnr)<<1 ^ (hnr)>>1 ^ HN(FOLD(w)))
#define FINISH(hnr)    (hnr)

#define CHECK_TERM(t) \
  assert((t) != NULL \
         && (!at_check || (AT_isValidTerm(t) && "term is invalid")))

/*}}}  */
/*{{{  globals */

char memory_id[] = "$Id: memory.c 23465 2007-08-27 09:35:50Z eriks $";

unsigned int maxTermSize = INITIAL_MAX_TERM_SIZE;

TermInfo* terminfo = NULL;

Block *at_freeblocklist = NULL;
unsigned int at_freeblocklist_size = 0;
BlockBucket block_table[BLOCK_TABLE_SIZE] = { { NULL, NULL } };

unsigned long total_nodes = 0;

static unsigned int table_class = INITIAL_TERM_TABLE_CLASS;
static HashNumber table_size    = AT_TABLE_SIZE(INITIAL_TERM_TABLE_CLASS);
static HashNumber table_mask    = AT_TABLE_MASK(INITIAL_TERM_TABLE_CLASS);

/*
 * For GC tuning
 */
int nb_minor_since_last_major = 0;
int old_bytes_in_young_blocks_after_last_major = 0; /* only live old cells in young blocks */
int old_bytes_in_old_blocks_after_last_major = 0; /* only live old cells in old blocks */
int old_bytes_in_young_blocks_since_last_major = 0; /* only live cells */

static int maxload = 80;
static ATerm *hashtable;

static unsigned int destructor_count = 0;
static ATbool (*destructors[MAX_DESTRUCTORS])(ATermBlob) = { NULL };

ATermList ATempty;

static int infoflags = 0;

#ifdef WITH_STATS
static int hash_info_before_gc[MAX_INFO_SIZES][3];
static int hash_info_after_gc[MAX_INFO_SIZES][3];
#endif

static ATbool at_check = ATfalse;

extern void AT_initMemmgnt();

/*}}}  */

/*{{{  void ATsetChecking(ATbool on) */

void ATsetChecking(ATbool on)
{
  at_check = on;

#ifdef NDEBUG
  if (on) {
    ATabort("warning: call to ATsetChecking has no effect in ATerm library without asserts!\n");
  }
#endif
}

/*}}}  */
/*{{{  ATbool ATgetChecking(void) */

ATbool ATgetChecking(void)
{
  return at_check;
}

/*}}}  */

/*{{{  static int term_size(ATerm t) */

/**
 * Calculate the size (in words) of a term.
 */

static int term_size(ATerm t)
{
  unsigned int size = 0;

  switch(ATgetType(t)) {
    case AT_INT:
      size = TERM_SIZE_INT;
      break;
    case AT_PLACEHOLDER:
      size = TERM_SIZE_PLACEHOLDER;
      break;
    case AT_REAL:
      size = TERM_SIZE_REAL;
      break;
    case AT_LIST:
      size = TERM_SIZE_LIST;
      break;
    /* case AT_BLOB:
      size = TERM_SIZE_BLOB;
      break; */
    case AT_APPL:
      size = TERM_SIZE_APPL(ATgetArity(ATgetSymbol(t)));
      break;
  }

  if(HAS_ANNO(t->header)) {
    size++;
  }

  return size;
}

/*}}}  */

/*{{{  static HashNumber hash_number(ATerm t, int size) */

static HashNumber hash_number(ATerm t, unsigned int size)
{
  unsigned int i;
  HashNumber hnr;

  hnr = START(HIDE_AGE_MARK(t->word[0]));
  
  for (i=ARG_OFFSET; i<size; i++) {
    hnr = COMBINE(hnr, t->word[i]);
  }

  return FINISH(hnr);
}

/* static HashNumber hash_number_anno(ATerm t, unsigned int size, ATerm anno)
{
  unsigned int i;
  HashNumber hnr;

  hnr = START(HIDE_AGE_MARK(t->word[0]));
  
  for (i=ARG_OFFSET; i<size; i++) {
    hnr = COMBINE(hnr, t->word[i]);
  }
  hnr = COMBINE(hnr, anno);

  return FINISH(hnr);
} */

/*}}}  */
/*{{{  HashNumber AT_hashnumber(ATerm t) */

HashNumber AT_hashnumber(ATerm t)
{
  return hash_number(t, term_size(t));
}

/*}}}  */

#ifdef WITH_STATS
/*{{{  static void hash_info(int stats[3][]) */

static void hash_info(int stats[MAX_INFO_SIZES][3]) 
{
  unsigned int i, len;
  static int count[MAX_INFO_SIZES];

  /* Initialize statistics */
  for(i=0; i<MAX_INFO_SIZES; i++)
    count[i] = 0;

  /* Gather statistics on the current fill of the hashtable */
  for(i=0; i<table_size; i++) {
    ATerm cur = hashtable[i];
    len = 0;
    while(cur) {
      len++;
      cur = cur->aterm.next; 
    }
    if(len >= MAX_INFO_SIZES)
      len = MAX_INFO_SIZES-1;
    count[len]++;
  }

  /* Update global statistic information */
  for(i=0; i<MAX_INFO_SIZES; i++) {
    STATS(stats[i], count[i]);
  }
}

/*}}}  */
#endif

/*{{{  static void resize_hashtable() */

/**
 * Resize the hashtable
 */

void resize_hashtable()
{
  ATerm *newhalf, *p;
  HashNumber oldsize;
  ATerm* newtable;

    /*fprintf(stderr, "warning: do not resize hashtable\n"); */
    /*return;*/

  oldsize = table_size;  
  table_class++;
  table_size = ((HashNumber)1)<<table_class;
  table_mask = table_size-1;
  if (!silent)
    fprintf(stderr, "resizing hashtable, class = %d\n", table_class);

  /*{{{  Create new term table */
  newtable = (ATerm *) AT_realloc(hashtable, table_size * sizeof(ATerm));
  
  if (newtable)
    hashtable = newtable;
  else {
    fprintf(stderr, "warning: could not resize hashtable to class %d.\n",
            table_class);
    table_class--;
    table_size = ((HashNumber)1)<<table_class;
    table_mask = table_size-1;
    return;
  }
  /*}}}  */

  /*{{{  Clear 2nd half of new table, uses increment == 2*oldsize */
  memset(hashtable+oldsize, 0, oldsize*sizeof(ATerm));
  /*}}}  */

  /*{{{  Rehash all old elements */

  newhalf = hashtable + oldsize;
  for(p=hashtable; p < newhalf; p++) {
    ATerm marked = *p;
    ATerm prev = NULL;
    /*{{{  Loop over marked part */
    while(marked && IS_MARKED(marked->header)) {
      CLR_MARK(marked->header);
      prev = marked;
      marked = marked->aterm.next;
    }
    /*}}}  */

    /*{{{  Loop over unmarked part */

    if (marked) {
      ATerm unmarked;
      ATerm *hashspot;

      if (marked == *p) {
        /* No marked terms */
        unmarked = marked;
        *p = NULL;
      } else {
        /* disconnect unmarked terms from rest */
        unmarked = marked;
        prev->aterm.next = NULL;
      }

      while(unmarked) {
        ATerm next = unmarked->aterm.next;
        HashNumber hnr;

        hnr = hash_number(unmarked, term_size(unmarked));
        hnr &= table_mask;
        hashspot = hashtable+hnr;
        unmarked->aterm.next = *hashspot;
        *hashspot = unmarked;

        if (hashspot > p && hashspot < newhalf)
          SET_MARK(unmarked->header);

        unmarked = next;
      }
    }

    /*}}}  */
  }

  /*}}}  */

}

/*}}}  */

/*{{{  void AT_initMemory(unsigned int argc, char *argv[]) */
/**
 * Initialize memory allocation datastructures
 */

void AT_initMemory(unsigned int argc, char *argv[])
{
  unsigned int i;
  HashNumber hnr;

  /*{{{  Analyze arguments */

  for (i = 1; i < argc; i++) {
    if (streq(argv[i], TERM_HASH_OPT)) {
      table_class = atoi(argv[++i]);
      table_size  = AT_TABLE_SIZE(table_class);
      table_mask  = AT_TABLE_MASK(table_class);
    } else if(streq(argv[i], HASH_INFO_OPT)) {
      infoflags |= INFO_HASHING;
    } else if(strcmp(argv[i], TERM_CHECK_OPT) == 0) {
      at_check = ATtrue;
    } else if (streq(argv[i], TERM_SIZE_OPT)) {
      maxTermSize = atoi(argv[++i]);
      if (maxTermSize < MIN_TERM_SIZE)
        maxTermSize = MIN_TERM_SIZE;
    } else if(strcmp(argv[i], "-at-help") == 0) {
      fprintf(stderr, "    %-20s: initial termtable size " 
              "(2^size, default=%d)\n",        TERM_HASH_OPT " <size>", table_class);
#ifdef WITH_STATS
      fprintf(stderr, "    %-20s: write information to 'hashing.stats'\n",
              HASH_INFO_OPT);
#else
      fprintf(stderr, "    %-20s: write information to 'hashing.stats' "
              "(disabled!)\n", HASH_INFO_OPT);
#endif
#ifdef NDEBUG
      fprintf(stderr, "    %-20s: check subterm validity at term creation "
              "(disabled!)\n", TERM_CHECK_OPT);
#else
      fprintf(stderr, "    %-20s: check subterm validity at term creation\n", 
              TERM_CHECK_OPT);
#endif
      fprintf(stderr, "    %-20s: initial maximum term size " 
              "(minimum=%lud, default=%lud)\n", TERM_SIZE_OPT " <size>", (unsigned long) MIN_TERM_SIZE, (unsigned long) maxTermSize);
    } 
  }

  /*}}}  */

  /*{{{  Initialize blocks */
  
  terminfo = (TermInfo*) AT_calloc(maxTermSize, sizeof(TermInfo));
  
  /*}}}  */
  /*{{{  Create term term table */

  hashtable = (ATerm *)AT_calloc(table_size, sizeof(ATerm ));
  if(!hashtable) {
    ATerror("AT_initMemory: cannot allocate term table of size %d\n", 
            table_size);
  }

  for(i=0; i<BLOCK_TABLE_SIZE; i++) {
    block_table[i].first_before = NULL;
    block_table[i].first_after  = NULL;
  }

  /*}}}  */
  /*{{{  Create the empty list */

  ATempty = (ATermList)AT_allocate(TERM_SIZE_LIST);
  ATempty->header = EMPTY_HEADER(0);
  CHECK_HEADER(ATempty->header);
  ATempty->aterm.next = NULL;
  ATempty->aterm.head = NULL;
  ATempty->aterm.tail = NULL;

  hnr = hash_number((ATerm)ATempty, TERM_SIZE_LIST);
  hashtable[hnr & table_mask] = (ATerm)ATempty;

  ATprotectList(&ATempty);

  /*}}}  */
#ifdef WITH_STATS
  /*{{{  Initialize info structures */
  for(i=0; i<MAX_INFO_SIZES; i++) {
    hash_info_before_gc[i][IDX_TOTAL] = 0;
    hash_info_before_gc[i][IDX_MIN] = MYMAXINT;
    hash_info_before_gc[i][IDX_MAX] = 0;
    hash_info_after_gc[i][IDX_TOTAL] = 0;
    hash_info_after_gc[i][IDX_MIN] = MYMAXINT;
    hash_info_after_gc[i][IDX_MAX] = 0;
  }
  /*}}}  */
#endif

  AT_initMemmgnt();
}

/*}}}  */
/*{{{  void AT_cleanupMemory() */

/**
 * Print hashtable info
 */

void AT_cleanupMemory()
{
#ifdef WITH_STATS
  unsigned int i;
  int info[MAX_INFO_SIZES][3];

  if(infoflags & INFO_HASHING) {
    unsigned int max = MAX_INFO_SIZES-1;
    FILE *f = fopen("hashing.stats", "wb");

    if(!f)
      ATerror("cannot open hashing statisics file: \"hashing.stats\"\n");

    while(hash_info_before_gc[max][IDX_MAX] == 0)
      max--;

    if(at_gc_count > 0) {
      fprintf(f, "hash statistics before and after garbage collection:\n");
      for(i=0; i<=max; i++) {
        fprintf(f, "%8d %8d %8d   %8d %8d %8d\n", 
                hash_info_before_gc[i][IDX_MIN],
                hash_info_before_gc[i][IDX_TOTAL]/at_gc_count,
                hash_info_before_gc[i][IDX_MAX],
                hash_info_after_gc[i][IDX_MIN],
                hash_info_after_gc[i][IDX_TOTAL]/at_gc_count,
                hash_info_after_gc[i][IDX_MAX]);
      }
    }

    for(i=0; i<MAX_INFO_SIZES; i++) {
      info[i][IDX_MIN] = MYMAXINT;
      info[i][IDX_TOTAL] = 0;
      info[i][IDX_MAX] = 0;
    }
    hash_info(info);
    fprintf(f, "hash statistics at end of program:\n");
    max = MAX_INFO_SIZES-1;
    while(info[max][IDX_MAX] == 0)
      max--;
    for(i=0; i<=max; i++) {
      fprintf(f, "%8d\n", info[i][IDX_TOTAL]);
    }

    for(i=0; i<table_size; i++) {
      int size = 0;
      ATerm cur = hashtable[i];
      for(size=0; cur; size++)
        cur = cur->aterm.next;
      if(size > 5) {
        fprintf(f, "bucket %d has length %d\n", i, size);
        cur = hashtable[i];
        while(cur) {
          ATfprintf(f, "%t\n", cur);
          cur = cur->aterm.next;
        }
      }
    }
  }
#endif

  AT_free(hashtable);
  AT_free(terminfo);
  AT_free_protected_blocks();
}

/*}}}  */

/**
 * Allocate a new block of a particular size class
 */

header_type *min_heap_address = (header_type*)(~0);
header_type *max_heap_address = 0;

/*{{{  static void allocate_block(unsigned int size)  */

static void allocate_block(unsigned int size) 
{
  int idx;
  Block *newblock;
  int init = 0;
  TermInfo* ti;
  
  if(at_freeblocklist != NULL) {
#ifdef GC_VERBOSE
    fprintf(stderr,"allocate_block %p from at_freeblocklist\n",at_freeblocklist);
#endif
    newblock = at_freeblocklist;
    at_freeblocklist = at_freeblocklist->next_by_size;
    at_freeblocklist_size--;
  } else {
    newblock = (Block *)AT_calloc(1, sizeof(Block));
#ifdef GC_VERBOSE
    fprintf(stderr,"allocate_block %p with calloc\n",newblock);
#endif
    if (newblock == NULL) {
      ATerror("allocate_block: out of memory!\n");
    }
    init = 1;

      /*printf("min_heap_address = %p\n",min_heap_address );*/
      /*printf("max_heap_address = %p\n",max_heap_address );*/
    
    min_heap_address = MIN(min_heap_address,(newblock->data));
    max_heap_address = MAX(max_heap_address,(newblock->data+BLOCK_SIZE));
    assert(min_heap_address < max_heap_address);
    
  }

  assert(size >= MIN_TERM_SIZE && size < maxTermSize);
  
  ti = &terminfo[size];
  ti->at_nrblocks++;
  
  /*
    printf("allocate_block[%d] = %d*%d\ttotal_nodes = %d\n",
       size, at_nrblocks[size],sizeof(Block),total_nodes);
    printf("at_blocks[%d] = %d\n",size,at_blocks[size]);
  */

  newblock->end = (newblock->data) + (BLOCK_SIZE - (BLOCK_SIZE % size));

  CLEAR_FROZEN(newblock);
  newblock->size = size;
  newblock->next_by_size = ti->at_blocks[AT_BLOCK];
  ti->at_blocks[AT_BLOCK] = newblock;
  ti->top_at_blocks = newblock->data;
  assert(ti->at_blocks[AT_BLOCK] != NULL);
  assert(((long)ti->top_at_blocks % MAX(sizeof(double), sizeof(void *))) == 0);
  
    /* [pem: Feb 14 02] TODO: fast allocation */
  assert(ti->at_freelist == NULL);

  if(init) {
      /* TODO: optimize */
      /* Place the new block in the block_table */
      /*idx = (((MachineWord)newblock) >> (BLOCK_SHIFT+2)) % BLOCK_TABLE_SIZE;*/
    idx = ADDR_TO_BLOCK_IDX(newblock);
    newblock->next_after = block_table[idx].first_after;
    block_table[idx].first_after = newblock;
    idx = (idx+1) % BLOCK_TABLE_SIZE;
    newblock->next_before = block_table[idx].first_before;
    block_table[idx].first_before = newblock;
  }
}

/*}}}  */

/**
 * Allocate a node of a particular size
 */

/*{{{  statistics macros */

#define AT_STATISTICS 

#define ALLOCATE_BLOCK_TEXT\
        allocate_block(size);\
        if((total_nodes/maxload)*100 > table_size) {\
          resize_hashtable();\
        }\
        assert(ti->at_blocks[AT_BLOCK] != NULL);\
        at = (ATerm)ti->top_at_blocks;\
        ti->top_at_blocks += size;

#ifdef WITH_STATS
#define GC_MINOR_TEXT\
          AT_STATISTICS;\
          nb_minor_since_last_major++;\
          if(infoflags & INFO_HASHING) {\
            hash_info(hash_info_before_gc);\
          }\
              /*fprintf(stderr,"-> AT_collect_minor\n");*/\
          AT_collect_minor();\
          if(infoflags & INFO_HASHING) {\
            hash_info(hash_info_after_gc);\
          }\
          AT_STATISTICS;

#define GC_MAJOR_TEXT\
        AT_STATISTICS;\
        nb_minor_since_last_major = 0;\
        if(infoflags & INFO_HASHING) {\
          hash_info(hash_info_before_gc);\
        }\
            /*fprintf(stderr,"-> AT_collect\n");*/\
        AT_collect();\
        if(infoflags & INFO_HASHING) {\
          hash_info(hash_info_after_gc);\
        }\
        AT_STATISTICS;
#else
#define GC_MINOR_TEXT\
          AT_STATISTICS;\
          nb_minor_since_last_major++;\
              /*fprintf(stderr,"-> AT_collect_minor\n");*/\
          AT_collect_minor();\
          AT_STATISTICS;

#define GC_MAJOR_TEXT\
        AT_STATISTICS;\
        nb_minor_since_last_major = 0;\
            /*fprintf(stderr,"-> AT_collect\n");*/\
        AT_collect();\
        AT_STATISTICS;
#endif

/*}}}  */

static unsigned long nb_at_allocate=0;

/*{{{  unsigned_long AT_getAllocatedCount() */

unsigned long AT_getAllocatedCount()
{
  return nb_at_allocate;
}

/*}}}  */

/*{{{  void AT_statistics()  */

void AT_statistics() 
{
  unsigned int size;
  Block *block;
  header_type *cur;
  int old_in_old_heap=0;
  int old_in_young_heap=0;
  int young_in_heap=0;
  int free_in_heap=0;
    /*
     * STATISTICS
     */
  for(size=MIN_TERM_SIZE; size<maxTermSize; size++) {
    
    block = terminfo[size].at_blocks[AT_OLD_BLOCK];
    while(block) {
      for(cur=block->data ; cur<block->end ; cur+=size) {
        ATerm t = (ATerm)cur;
        if(IS_OLD(t->header)) {
          old_in_old_heap+=size;
        } else if(ATgetType(t) == AT_FREE) {
          free_in_heap+=size;
        } else {
          young_in_heap+=size;
        }
      }
      block = block->next_by_size;
    }

    block = terminfo[size].at_blocks[AT_BLOCK];
    while(block) {
      for(cur=block->data ; cur<block->end ; cur+=size) {
        ATerm t = (ATerm)cur;
        if(IS_OLD(t->header)) {
          old_in_young_heap+=size;
        } else if(ATgetType(t) == AT_FREE) {
          free_in_heap+=size;
        } else {
          young_in_heap+=size;
        }
      }
      block = block->next_by_size;
    }
  }
#ifdef GC_VERBOSE
  fprintf(stderr,"nb_at_allocate = %ld\n",nb_at_allocate);
  fprintf(stderr,"young in heap = %d kB\n",young_in_heap/1024);
  fprintf(stderr,"old   in heap = %d kB\t(%d in old + %d in young)\n",
          (old_in_old_heap+old_in_young_heap)/1024,
          old_in_old_heap/1024, old_in_young_heap/1024);
  fprintf(stderr,"free  in heap = %d kB\n",free_in_heap/1024);

  fprintf(stderr,"mpold %ld\t%d\n",
          nb_at_allocate,
          (old_in_young_heap+old_in_old_heap)/1024);

  fprintf(stderr,"mpyoung %ld\t%d\n",
          nb_at_allocate,
          (young_in_heap+old_in_old_heap+old_in_young_heap)/1024);

    /*
  fprintf(stderr,"cumul %d\t%d\t%d\t%d\n",
          nb_at_allocate,
          (old_in_young_heap+old_in_old_heap)/1024,
          (young_in_heap+old_in_old_heap+old_in_young_heap)/1024,
          (young_in_heap+old_in_old_heap+old_in_young_heap+free_in_heap)/1024);
    */    
#endif
  
}

/*}}}  */

void AT_growMaxTermSize(unsigned int neededsize)
{
  TermInfo* newterminfo;
  unsigned int newsize;

  if (low_memory) {
    newsize = neededsize;
  }
  else {
    /* Grow twice as much as currently needed, to accomodate for future growth */
    newsize = maxTermSize+(neededsize-maxTermSize)*2;
  }

#ifndef NDEBUG
  fprintf(stderr, "Growing administrative structures to accomodate terms of size %d\n", newsize);
#endif

  newterminfo = (TermInfo*)AT_realloc((void*)terminfo, newsize*sizeof(TermInfo));
  if ((!newterminfo)&&(newsize>neededsize)) {
    /* Realloc failed again; try with needed size */
    newsize = neededsize;
    newterminfo = (TermInfo*)AT_realloc((void*)terminfo, newsize*sizeof(TermInfo));
  }
  if (!newsize) {
    ATerror("AT_growMaxTermSize: cannot allocate %d extra TermInfo elements.\n", newsize-maxTermSize);
  }
  
  /* Clear new area */
  memset(&newterminfo[maxTermSize], 0, (newsize-maxTermSize)*sizeof(TermInfo));

  terminfo = newterminfo;
  maxTermSize = newsize;
}

/*{{{  ATerm AT_allocate(unsigned int size)  */

ATerm AT_allocate(unsigned int size) 
{
  ATerm at;
  TermInfo* ti;
  
  nb_at_allocate++;
  
  if (size+1 > maxTermSize) {
    AT_growMaxTermSize(size+1);
  }
  
  ti = &terminfo[size];

  while(1) {
    if(ti->at_blocks[AT_BLOCK] && ti->top_at_blocks < ti->at_blocks[AT_BLOCK]->end) {
        /* the first block is not full: allocate a cell */
      at = (ATerm)ti->top_at_blocks;
      ti->top_at_blocks += size;
      break;

    } else if(ti->at_freelist) {
        /* the freelist is not empty: allocate a cell */
      at = ti->at_freelist;
      ti->at_freelist = ti->at_freelist->aterm.next;
      assert(ti->at_blocks[AT_BLOCK] != NULL);
      assert(ti->top_at_blocks == ti->at_blocks[AT_BLOCK]->end);
      break;

    } else {
        /* there is no more memory: run the GC or allocate a block */
#ifdef GC_VERBOSE
      fprintf(stderr,"AT_allocate(%d)\n",size);
#endif
  
      if(ti->at_nrblocks <= gc_min_number_of_blocks) {
#ifdef GC_VERBOSE
        fprintf(stderr,"INITIAL PHASE -> ALLOCATE_BLOCK\n");
#endif
        ALLOCATE_BLOCK_TEXT;
      } else {
        int reclaimed_memory_during_last_gc =
            /*(ti->nb_reclaimed_blocks_during_last_gc*sizeof(Block)) +*/
          (ti->nb_reclaimed_cells_during_last_gc*SIZE_TO_BYTES(size));
          /* +1 to avoid division by zero */
        int reclaimed_memory_ratio_during_last_gc =
          (100*reclaimed_memory_during_last_gc) / (1+ti->nb_live_blocks_before_last_gc*sizeof(Block));
#ifdef GC_VERBOSE
        fprintf(stderr,"reclaimed_memory_during_last_gc = %d\n",reclaimed_memory_during_last_gc);
        fprintf(stderr,"reclaimed_memory_ratio_during_last_gc = %d\n",reclaimed_memory_ratio_during_last_gc);

        
#endif
        if(reclaimed_memory_ratio_during_last_gc > good_gc_ratio) {
          if(nb_minor_since_last_major < min_nb_minor_since_last_major) {
#ifdef GC_VERBOSE
            fprintf(stderr,"GOOD GC -> GC_MINOR\n");
#endif
            GC_MINOR_TEXT;
          } else {
#ifdef GC_VERBOSE
            fprintf(stderr,"GOOD GC and MANY SUCCESSIVE MINOR -> GC_MAJOR\n");
#endif
            GC_MAJOR_TEXT;
          }
          
        } else {
          int nb_allocated_blocks_since_last_gc = ti->at_nrblocks-ti->nb_live_blocks_before_last_gc;
            /* +1 to avoid division by zero */
          int allocation_rate =
            (100*nb_allocated_blocks_since_last_gc)/(1+ti->nb_live_blocks_before_last_gc);
#ifdef GC_VERBOSE
          fprintf(stderr,"allocation_rate = %d\n",allocation_rate);
#endif
          
          if(allocation_rate < small_allocation_rate_ratio) {
#ifdef GC_VERBOSE
            fprintf(stderr,"NOT GOOD GC and SMALL ALLOCATION RATE -> ALLOCATE_BLOCK\n");
#endif
            ALLOCATE_BLOCK_TEXT;
          } else {
              /* +1 to avoid division by zero */
            int old_increase_rate =
              (100*(old_bytes_in_young_blocks_since_last_major-old_bytes_in_young_blocks_after_last_major)) /
              (1+old_bytes_in_young_blocks_after_last_major+old_bytes_in_old_blocks_after_last_major);

#ifdef GC_VERBOSE
            fprintf(stderr,"old_bytes_in_young_blocks_since_last_major = %d\n",old_bytes_in_young_blocks_since_last_major);
            fprintf(stderr,"old_bytes_in_young_blocks_after_last_major = %d\n",old_bytes_in_young_blocks_after_last_major);
            fprintf(stderr,"old_bytes_in_old_blocks_after_last_major = %d\n",old_bytes_in_old_blocks_after_last_major);
            fprintf(stderr,"old_increase_rate = %d\n",old_increase_rate);
#endif
            if(old_increase_rate < old_increase_rate_ratio) {
#ifdef GC_VERBOSE
              fprintf(stderr,"NOT GOOD GC and SMALL OLD INCREASE RATE -> GC_MINOR\n");
#endif
              GC_MINOR_TEXT;
            } else {
#ifdef GC_VERBOSE
              fprintf(stderr,"NOT GOOD GC and HIGH OLD INCREASE RATE -> GC_MAJOR\n");
#endif
              GC_MAJOR_TEXT;
            }
          }
        }
      }
    }
  }

  total_nodes++;
    /*fprintf(stderr,"AT_allocate term[%ld] = %p\n",size,at);*/
    
  return at;
}

/*}}}  */

/*{{{  void AT_freeTerm(unsigned int size, ATerm t) */

/**
 * Free a term of a particular size.
 */

void AT_freeTerm(unsigned int size, ATerm t)
{
  HashNumber hnr = hash_number(t, size);
  ATerm prev = NULL, cur;
  /* unsigned int i; */

  terminfo[size].nb_reclaimed_cells_during_last_gc++;
  
    /*fprintf(stderr,"AT_freeTerm term[%d] = %x\theader = %x\n",size,(unsigned int)t,t->header);*/
  
  /* The data of a blob needs to be freed!!! */
  /* if (ATgetType(t) == AT_BLOB) 
  {
    ATbool destructed = ATfalse;
    for (i=0; i<destructor_count; i++) {
      if ((destructors[i])((ATermBlob)t)) {
        destructed = ATtrue;
        break;
      }
    }
    if (!destructed) {
      AT_free(ATgetBlobData((ATermBlob)t));
    }
  } */

    /* Remove the node from the hashtable */
  hnr &= table_mask; 
  cur = hashtable[hnr];

  do {
    if(!cur) {
        /*printf("freeterm = %d\n",t);*/
      /*fprintf(stderr,"### cannot find term %x in hashtable at pos %d header = %x\n", (unsigned int)(intptr_t)t, (int)hnr, (unsigned int)t->header);*/

      ATabort("AT_freeTerm: cannot find term %n at %p in hashtable at pos %d"
              ", header = 0x%x\n", t, t, hnr, t->header);
    }
    if (cur == t) {
      if(prev)
        prev->aterm.next = cur->aterm.next;
      else
        hashtable[hnr] = cur->aterm.next;
      /* Put the node in the appropriate free list */
      /*printf("AT_freeTerm: put cell[%d] %x into freelist\n",size,t);*/
      total_nodes--;
      return;
    }
  } while(((prev=cur), (cur=cur->aterm.next)));
}

/*}}}  */

/*{{{  ATermAppl ATmakeAppl(Symbol sym, ...) */

/**
 * Create a new ATermAppl. The argument count can be found in the symbol.
 */

ATermAppl ATmakeAppl(Symbol sym, ...)
{
  ATermAppl cur;
  unsigned int i, arity = ATgetArity(sym);
  ATbool found;
  header_type header;
  HashNumber hnr;
  va_list args;
  ATerm arg;
  ATerm* buffer;
  
  header = APPL_HEADER(0, arity > MAX_INLINE_ARITY ?
                       MAX_INLINE_ARITY+1 : arity, sym);

  buffer = AT_alloc_protected(arity);
  
  hnr = START(header);
  va_start(args, sym);
  for (i=0; i<arity; i++) {
  	arg = va_arg(args, ATerm);
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, HN(arg));
    buffer[i] = arg;
  }
  va_end(args);
  hnr = FINISH(hnr);
  
  cur = (ATermAppl)hashtable[hnr & table_mask];
  while (cur) {
    if (EQUAL_HEADER(cur->header,header)) {
      found = ATtrue;
      for (i=0; i<arity; i++) {
        if (!ATisEqual(ATgetArgument(cur, i), buffer[i])) {
          found = ATfalse;
          break;
        }
      }
      if (found)
        break;
    }
    cur = (ATermAppl)cur->aterm.next;
  }

  if (!cur) {
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    for (i=0; i<arity; i++)
      ATgetArgument(cur, i) = buffer[i];
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }
  
  AT_free_protected(buffer);

  return cur;
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl0(Symbol sym) */

ATermAppl ATmakeAppl0(Symbol sym)
{
  ATermAppl cur, prev, *hashspot;
  header_type header = APPL_HEADER(0, 0, sym);
  HashNumber hnr;
  
  PARK_SYMBOL(sym);
  
  CHECK_ARITY(ATgetArity(sym), 0);
  
  hnr = FINISH(START(header));

  prev = NULL;
  hashspot = (ATermAppl*) &(hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur) {
    if(EQUAL_HEADER(cur->header,header)) {
      /* Promote current entry to front of hashtable */
      if (prev != NULL) {
        prev->aterm.next = cur->aterm.next;
        cur->aterm.next = (ATerm) *hashspot;
        *hashspot = cur;
      }

        /*fprintf(stderr,"ATmakeAppl0 found = %x\n",(unsigned int)cur);*/
      return cur;
    }
    prev = cur;
    cur = (ATermAppl)cur->aterm.next;
  }

  cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(0));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  CHECK_HEADER(cur->header);
  cur->aterm.next = hashtable[hnr];
  hashtable[hnr] = (ATerm) cur;

    /*fprintf(stderr,"ATmakeAppl0 alloc = %x\n",(unsigned int)cur);*/
  return cur;
}



/**
 * Create an ATermAppl with zero arguments.
 */

/*}}}  */
/*{{{  ATermAppl ATmakeAppl1(Symbol sym, ATerm arg0) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl1(Symbol sym, ATerm arg0)
{
  ATermAppl cur, prev, *hashspot;
  header_type header = APPL_HEADER(0, 1, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_ARITY(ATgetArity(sym), 1);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)arg0));
  hnr = FINISH(hnr);
  
  prev = NULL;
  hashspot = (ATermAppl*) &(hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur) {
    if(EQUAL_HEADER(header,cur->header) 
          && ATgetArgument(cur, 0) == arg0) {
        /* Promote current entry to front of hashtable */
      if (prev != NULL) {
        prev->aterm.next = cur->aterm.next;
        cur->aterm.next = (ATerm) *hashspot;
        *hashspot = cur;
      }
        /*fprintf(stderr,"ATmakeAppl1 found = %p\tsub = %p\n",cur,ATgetArgument(cur, 0));*/
      CHECK_ARGUMENT(cur, 0);
      return cur;
    }
    prev = cur;
    cur = (ATermAppl)cur->aterm.next;
  }

  cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(1));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  CHECK_HEADER(cur->header);
  ATgetArgument(cur, 0) = arg0;
  CHECK_ARGUMENT(cur, 0);
  cur->aterm.next = hashtable[hnr];
  hashtable[hnr] = (ATerm) cur;

    /*fprintf(stderr,"ATmakeAppl1 alloc = %x\tsub = %x\n",(unsigned int)cur,(unsigned int)ATgetArgument(cur, 0));*/
  return cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl2(Symbol sym, arg0, arg1) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl2(Symbol sym, ATerm arg0, ATerm arg1)
{
  ATermAppl cur, prev, *hashspot;
  header_type header = APPL_HEADER(0, 2, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_ARITY(ATgetArity(sym), 2);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)arg0));
  hnr = COMBINE(hnr, HN((char*)arg1));
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = (ATermAppl*) &(hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur) {
    if(EQUAL_HEADER(cur->header,header) 
         && ATgetArgument(cur, 0) == arg0 
         && ATgetArgument(cur, 1) == arg1) {
      /* Promote current entry to front of hashtable */
      if (prev != NULL) {
        prev->aterm.next = cur->aterm.next;
        cur->aterm.next = (ATerm) *hashspot;
        *hashspot = cur;
      }
      CHECK_ARGUMENT(cur, 0);
      CHECK_ARGUMENT(cur, 1);
      return cur;
    }
    prev = cur;
    cur = (ATermAppl)cur->aterm.next;
  }

  cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(2));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  CHECK_HEADER(cur->header);
  ATgetArgument(cur, 0) = arg0;
  ATgetArgument(cur, 1) = arg1;
  CHECK_ARGUMENT(cur, 0);
  CHECK_ARGUMENT(cur, 1);

  cur->aterm.next = hashtable[hnr];
  hashtable[hnr] = (ATerm) cur;

  return cur;
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl3(Symbol sym, ATerm arg0, arg1, arg2) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl3(Symbol sym, ATerm arg0, ATerm arg1, ATerm arg2)
{
  ATermAppl cur;
  header_type header = APPL_HEADER(0, 3, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_ARITY(ATgetArity(sym), 3);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)arg0));
  hnr = COMBINE(hnr, HN((char*)arg1));
  hnr = COMBINE(hnr, HN((char*)arg2));
  hnr = FINISH(hnr);

  cur = (ATermAppl) hashtable[hnr & table_mask];
  while(cur && (!EQUAL_HEADER(cur->header,header) ||
                ATgetArgument(cur, 0) != arg0 ||
                ATgetArgument(cur, 1) != arg1 ||
                ATgetArgument(cur, 2) != arg2)) {
    cur = (ATermAppl) cur->aterm.next;
  }

  if(!cur) {
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(3));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ATgetArgument(cur, 0) = arg0;
    ATgetArgument(cur, 1) = arg1;
    ATgetArgument(cur, 2) = arg2;
    CHECK_ARGUMENT(cur, 0);
    CHECK_ARGUMENT(cur, 1);
    CHECK_ARGUMENT(cur, 2);

    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }
    
  return cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl4(Symbol sym, ATerm arg0, arg1, arg2, a3) */

/**
 * Create an ATermAppl with four arguments.
 */

ATermAppl ATmakeAppl4(Symbol sym, ATerm arg0, ATerm arg1, ATerm arg2, ATerm arg3)
{
  ATermAppl cur;
  header_type header;
  HashNumber hnr;

  PARK_SYMBOL(sym);

  header = APPL_HEADER(0, 4, sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_ARITY(ATgetArity(sym), 4);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)arg0));
  hnr = COMBINE(hnr, HN((char*)arg1));
  hnr = COMBINE(hnr, HN((char*)arg2));
  hnr = COMBINE(hnr, HN((char*)arg3));
  hnr = FINISH(hnr);

  cur = (ATermAppl)hashtable[hnr & table_mask];
  while(cur && (!EQUAL_HEADER(cur->header,header) ||
                ATgetArgument(cur, 0) != arg0 ||
                ATgetArgument(cur, 1) != arg1 ||
                ATgetArgument(cur, 2) != arg2 ||
                ATgetArgument(cur, 3) != arg3))
    cur = (ATermAppl)cur->aterm.next;

  if(!cur) {
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(4));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ATgetArgument(cur, 0) = arg0;
    ATgetArgument(cur, 1) = arg1;
    ATgetArgument(cur, 2) = arg2;
    ATgetArgument(cur, 3) = arg3;
    CHECK_ARGUMENT(cur, 0);
    CHECK_ARGUMENT(cur, 1);
    CHECK_ARGUMENT(cur, 2);
    CHECK_ARGUMENT(cur, 3);

    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl5(Symbol sym, ATerm arg0, arg1, arg2, a3, a4) */

/**
 * Create an ATermAppl with five arguments.
 */

ATermAppl ATmakeAppl5(Symbol sym, ATerm arg0, ATerm arg1, ATerm arg2, 
                      ATerm arg3, ATerm arg4)
{
  ATermAppl cur;
  header_type header = APPL_HEADER(0, 5, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);

  CHECK_ARITY(ATgetArity(sym), 5);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)arg0));
  hnr = COMBINE(hnr, HN((char*)arg1));
  hnr = COMBINE(hnr, HN((char*)arg2));
  hnr = COMBINE(hnr, HN((char*)arg3));
  hnr = COMBINE(hnr, HN((char*)arg4));
  hnr = FINISH(hnr);

  cur = (ATermAppl) hashtable[hnr & table_mask];
  while(cur && (!EQUAL_HEADER(cur->header,header) ||
                ATgetArgument(cur, 0) != arg0 ||
                ATgetArgument(cur, 1) != arg1 ||
                ATgetArgument(cur, 2) != arg2 ||
                ATgetArgument(cur, 3) != arg3 ||
                ATgetArgument(cur, 4) != arg4))
    cur = (ATermAppl) cur->aterm.next;

  if(!cur) {
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(5));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ATgetArgument(cur, 0) = arg0;
    ATgetArgument(cur, 1) = arg1;
    ATgetArgument(cur, 2) = arg2;
    ATgetArgument(cur, 3) = arg3;
    ATgetArgument(cur, 4) = arg4;
    CHECK_ARGUMENT(cur, 0);
    CHECK_ARGUMENT(cur, 1);
    CHECK_ARGUMENT(cur, 2);
    CHECK_ARGUMENT(cur, 3);
    CHECK_ARGUMENT(cur, 4);

    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl6(Symbol sym, ATerm arg0, arg1, arg2, a3, a4, a5) */

/**
 * Create an ATermAppl with six arguments.
 */

ATermAppl ATmakeAppl6(Symbol sym, ATerm arg0, ATerm arg1, ATerm arg2, 
                      ATerm arg3, ATerm arg4, ATerm arg5)
{
  ATermAppl cur;
  header_type header = APPL_HEADER(0, 6, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);
  CHECK_TERM(arg5);
  CHECK_ARITY(ATgetArity(sym), 6);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)arg0));
  hnr = COMBINE(hnr, HN((char*)arg1));
  hnr = COMBINE(hnr, HN((char*)arg2));
  hnr = COMBINE(hnr, HN((char*)arg3));
  hnr = COMBINE(hnr, HN((char*)arg4));
  hnr = COMBINE(hnr, HN((char*)arg5));
  hnr = FINISH(hnr);

  cur = (ATermAppl) hashtable[hnr & table_mask];
  while(cur && (!EQUAL_HEADER(cur->header,header) ||
                ATgetArgument(cur, 0) != arg0 ||
                ATgetArgument(cur, 1) != arg1 ||
                ATgetArgument(cur, 2) != arg2 ||
                ATgetArgument(cur, 3) != arg3 ||
                ATgetArgument(cur, 4) != arg4 ||
                ATgetArgument(cur, 5) != arg5))
    cur = (ATermAppl) cur->aterm.next;

  if(!cur) {
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(6));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ATgetArgument(cur, 0) = arg0;
    ATgetArgument(cur, 1) = arg1;
    ATgetArgument(cur, 2) = arg2;
    ATgetArgument(cur, 3) = arg3;
    ATgetArgument(cur, 4) = arg4;
    ATgetArgument(cur, 5) = arg5;
    CHECK_ARGUMENT(cur, 0);
    CHECK_ARGUMENT(cur, 1);
    CHECK_ARGUMENT(cur, 2);
    CHECK_ARGUMENT(cur, 3);
    CHECK_ARGUMENT(cur, 4);
    CHECK_ARGUMENT(cur, 5);

    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeApplList(Symbol sym, ATermList args) */

/**
 * Build a function application from a symbol and a list of arguments.
 */

ATermAppl ATmakeApplList(Symbol sym, ATermList args)
{
  ATermAppl cur;
  ATermList argptr;
  unsigned int i, arity = ATgetArity(sym);
  ATbool found;
  header_type header = APPL_HEADER(0, arity > MAX_INLINE_ARITY ?
                                   MAX_INLINE_ARITY+1 : arity, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM((ATerm)args);
  assert(arity == ATgetLength(args));

  argptr = args;
  hnr = START(header);
  for (i=0; i<arity; i++) {
    hnr = COMBINE(hnr, HN((char*)ATgetFirst(argptr)));
    argptr = ATgetNext(argptr);
  }
  hnr = FINISH(hnr);

  cur = (ATermAppl) hashtable[hnr & table_mask];
  while(cur)
  {
    if(EQUAL_HEADER(cur->header,header))
    {
      found = ATtrue;
      argptr = args;
      for(i=0; i<arity; i++) {
        if(!ATisEqual(ATgetArgument(cur, i), ATgetFirst(argptr))) {
          found = ATfalse;
          break;
        }
        argptr = ATgetNext(argptr);
      }
      if(found)
        break;
    }
    cur = (ATermAppl) cur->aterm.next;
  }

  if(!cur)
  {
    ATprotectList(&args);
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(arity));
    ATunprotect((ATerm *)(void *)&args);

    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    argptr = args;
    for (i=0; i<arity; i++) {
      ATgetArgument(cur, i) = ATgetFirst(argptr);
      CHECK_ARGUMENT(cur, i);
      argptr = ATgetNext(argptr);
    }
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
}

/*}}}  */
/*{{{  ATermAppl ATmakeApplArray(Symbol sym, ATerm args[]) */

/**
 * Build a function application from a symbol and an array of arguments.
 */

ATermAppl ATmakeApplArray(Symbol sym, ATerm args[])
{
  ATermAppl cur;
  unsigned int i, arity = ATgetArity(sym);
  ATbool found;
  HashNumber hnr;
  header_type header = APPL_HEADER(0, arity > MAX_INLINE_ARITY ?
                                   MAX_INLINE_ARITY+1 : arity, sym);

  PARK_SYMBOL(sym);
  
  hnr = START(header);
  for (i=0; i<arity; i++)
    hnr = COMBINE(hnr, (HashNumber)(char*)args[i]);
  hnr = FINISH(hnr);

  cur = (ATermAppl) hashtable[hnr & table_mask];
  while(cur) {
    if(EQUAL_HEADER(cur->header,header)) {
      found = ATtrue;
      for(i=0; i<arity; i++) {
        if(!ATisEqual(ATgetArgument(cur, i), args[i])) {
          found = ATfalse;
          break;
        }
      }
      if(found)
        break;
    }
    cur = (ATermAppl) cur->aterm.next;
  }

  if(!cur) {
    ATprotectArray(args, arity);
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(arity));
    ATunprotectArray(args);

    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    for (i=0; i<arity; i++) {
      ATgetArgument(cur, i) = args[i];
      CHECK_ARGUMENT(cur, i);
    }
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
}

/*}}}  */

/*{{{  ATermInt ATmakeInt(int val) */

/**
 * Create an ATermInt
 */

ATermInt ATmakeInt(int val)
{
  ATermInt cur;
  header_type header = INT_HEADER(0);
  HashNumber hnr;

  hnr = START(header);
  hnr = COMBINE(hnr, HN(val));
  hnr = FINISH(hnr);

  cur = (ATermInt) hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header) || (cur->aterm.value != val))) {
    cur = (ATermInt) cur->aterm.next;
  }

  if (!cur) {
    cur = (ATermInt)AT_allocate(TERM_SIZE_INT);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    cur->aterm.value = (long)val;
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  assert((hnr & table_mask) == (hash_number((ATerm)cur, TERM_SIZE_INT) & table_mask));

  return cur;  
}

/*}}}  */
/*{{{  ATermReal ATmakeReal(double val) */

/**
 * Create an ATermReal
 */

#define DOUBLEWORDSIZE (sizeof(double)/sizeof(MachineWord))
union doublewords {
  double      val; 
  MachineWord word[DOUBLEWORDSIZE];
};

ATermReal ATmakeReal(double val)
{
  ATermReal cur;
  header_type header = REAL_HEADER(0);
  HashNumber hnr;
  unsigned int i;
  union doublewords dw;
  
  hnr = START(header);
  dw.val = val;
  for (i=0; i<DOUBLEWORDSIZE; i++)
    hnr = COMBINE(hnr, HN(dw.word[i]));
  hnr = FINISH(hnr);
  
  cur = (ATermReal) hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
                 || ATgetReal(cur) != val)) {
    cur = (ATermReal) cur->aterm.next;
  }

  if (!cur) {
    cur = (ATermReal) AT_allocate(TERM_SIZE_REAL);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    cur->aterm.value = val;
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
}

/*}}}  */

/*{{{  ATermList ATmakeList1(ATerm el) */

/**
 * Build a list with one element.
 */

ATermList ATmakeList1(ATerm el)
{
  ATermList cur;
  header_type header = LIST_HEADER(0, 1);
  HashNumber hnr;

  CHECK_TERM(el);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)el));
  hnr = COMBINE(hnr, HN((char*)ATempty));
  hnr = FINISH(hnr);

  cur = (ATermList)hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
                 || ATgetFirst(cur) != el
                 || ATgetNext(cur) != ATempty)) {
    cur = (ATermList) cur->aterm.next;
  }

  if (!cur) {
    cur = (ATermList) AT_allocate(TERM_SIZE_LIST);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ATgetFirst(cur) = el;
    ATgetNext(cur) = ATempty;
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
}

/*}}}  */
/*{{{  ATermList ATinsert(ATermList tail, ATerm el) */

/**
 * Insert an element at the front of a list.
 */

ATermList ATinsert(ATermList tail, ATerm el)
{
  unsigned int curLength = GET_LENGTH(tail->header);
  unsigned int newLength;
  header_type header;
  HashNumber hnr;
  ATermList cur;
  
  /* If length exceeds the maximum length that can be stored in the header,
     store MAX_LENGTH-1 in the header. ATgetLength will then count the length of the
     list instead of using on the header
  */
  if (curLength >= MAX_LENGTH-1)
    newLength = MAX_LENGTH-1;
  else
    newLength = curLength+1;
  
  header = LIST_HEADER(0, newLength);
  
  CHECK_TERM((ATerm)tail);
  CHECK_TERM(el);

  assert(ATgetType(tail) == AT_LIST);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)el));
  hnr = COMBINE(hnr, HN((char*)tail));
  hnr = FINISH(hnr);

  cur = (ATermList) hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
                 || ATgetFirst(cur) != el
                 || ATgetNext(cur) != tail)) {
    cur = (ATermList) cur->aterm.next;
  }

  if (!cur) {
    cur = (ATermList) AT_allocate(TERM_SIZE_LIST);
    /* Hashtable might be resized, so delay masking until after AT_allocate */
    hnr &= table_mask; 
    cur->header = header;
    CHECK_HEADER(cur->header);
    ATgetFirst(cur) = el;
    ATgetNext(cur) = tail;
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
}

/*}}}  */

/*{{{  ATermPlaceholder ATmakePlaceholder(ATerm type) */

/**
 * Create a new placeholder.
 */

ATermPlaceholder ATmakePlaceholder(ATerm type)
{
  ATermPlaceholder cur;
  header_type header = PLACEHOLDER_HEADER(0);
  HashNumber hnr;

  CHECK_TERM(type);

  hnr = START(header);
  hnr = COMBINE(hnr, HN((char*)type));
  hnr = FINISH(hnr);

  cur = (ATermPlaceholder) hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
                 || ATgetPlaceholder(cur) != type)) {
    cur = (ATermPlaceholder) cur->aterm.next;
  }

  if (!cur) {
    cur = (ATermPlaceholder) AT_allocate(TERM_SIZE_PLACEHOLDER);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    cur->aterm.ph_type = type;
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;

}

/*}}}  */

/*{{{  ATermBlob ATmakeBlob(void *data, int size) */

/**
 * Create a new BLOB (Binary Large OBject)
 */

/* ATermBlob ATmakeBlob(unsigned int size, void *data)
{
  ATermBlob cur;
  header_type header = BLOB_HEADER(0);
  HashNumber hnr;

/ *
  if (low_memory) {
    AT_collect();
  }
* /

  hnr = START(header);
  hnr = COMBINE(hnr, size);
  hnr = COMBINE(hnr, (HashNumber)(char*)data);
  hnr = FINISH(hnr);

  cur = (ATermBlob) hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
                 || cur->aterm.size != size
                 || cur->aterm.data != data)) {
    cur = (ATermBlob) cur->aterm.next;
  }

  if (!cur) {
    cur = (ATermBlob) AT_allocate(TERM_SIZE_BLOB);
    / * Delay masking until after AT_allocate * /
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    cur->aterm.data = data;
    cur->aterm.size = size;
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
} */

/*}}}  */

/*{{{  ATerm AT_setAnnotations(ATerm t, ATerm annos) */

/**
 * Change the annotations of a term.
 */

/* ATerm AT_setAnnotations(ATerm t, ATerm annos)
{
  HashNumber hnr;
  unsigned int i, size = term_size(t);
  header_type header;
  ATbool found;
  ATerm cur;

  CHECK_TERM(t);
  CHECK_TERM(annos);
  assert(annos != NULL);

  if (HAS_ANNO(t->header)) {
    header = t->header;
    size--;
    hnr = hash_number_anno(t, size, annos);
  } else {
    SET_ANNO(t->header);
    header = t->header;
    hnr = hash_number_anno(t, size, annos);
    CLR_ANNO(t->header);
  }

  cur = hashtable[hnr & table_mask];
  found = ATfalse;

  / * Look through the hashtable for an identical term * /
  while (cur && !found) {
    if ((EQUAL_HEADER(cur->header,header))&&(ATisEqual(cur->subaterm[size], annos))) {
      found = ATtrue;

      / * check if other components are equal * /
      for (i=ARG_OFFSET; i<size; i++) {
        if (!ATisEqual(cur->subaterm[i], t->subaterm[i])) {
          found = ATfalse;
          break;
        }
      }
    }
    if (!found)
      cur = cur->aterm.next;
  }

  if (!found) {
    / * We need to create a new term * /
    cur = AT_allocate(size+1);
    / * Delay masking until after AT_allocate * /
    hnr &= table_mask;
    cur->header = header;
    SET_AGE(cur->header,YOUNG_AGE);
    CHECK_HEADER(cur->header);
    
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = cur;

    for (i=ARG_OFFSET; i<size; i++) {
      cur->subaterm[i] = t->subaterm[i];
    }
    
    cur->subaterm[size] = annos;
  }

  return cur;
} */

/*}}}  */
/*{{{  ATerm AT_removeAnnotations(ATerm t) */

/**
 * Remove all annotations of a term.
 */

/* ATerm AT_removeAnnotations(ATerm t)
{
  HashNumber hnr;
  unsigned int i, size;
  header_type header;
  ATbool found;
  ATerm cur;

  CHECK_TERM(t);
  if (!HAS_ANNO(t->header)) {
    return t;
  }

  CLR_ANNO(t->header);
  header = t->header;
  size = term_size(t);
  hnr = hash_number(t, size);
  SET_ANNO(t->header);

  cur = hashtable[hnr & table_mask];
  found = ATfalse;

  / * Look through the hashtable for an identical term * /
  while (cur && !found) {
    if (EQUAL_HEADER(cur->header,header)) {
      found = ATtrue;

      / * check if other components are equal * /
      for (i=ARG_OFFSET; i<size; i++) {
        if (!ATisEqual(cur->subaterm[i], t->subaterm[i])) {
          found = ATfalse;
          break;
        }
      }
    }
    if (!found) {
      cur = cur->aterm.next;
    }
  }

  if (!found) {
    / * We need to create a new term * /
    cur = AT_allocate(size);
    / * Delay masking until after AT_allocate * /
    hnr &= table_mask;
    cur->header = header;
    SET_AGE(cur->header,YOUNG_AGE);
    CHECK_HEADER(cur->header);
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = cur;

    for (i=ARG_OFFSET; i<size; i++) {
      cur->subaterm[i] = t->subaterm[i];
    }
  }

  return cur;
}

/ *}}}  */

/*{{{  ATermAppl ATsetArgument(ATermAppl appl, ATerm arg, unsigned int n) */

/**
 * Change one argument of an application.
 */

ATermAppl ATsetArgument(ATermAppl appl, ATerm arg, unsigned int n)
{
  unsigned int i, arity;
  Symbol sym = ATgetSymbol(appl);
  /* ATerm annos = AT_getAnnotations((ATerm)appl); */
  ATerm annos = NULL;
  ATermAppl cur;
  ATbool found;
  HashNumber hnr;

  CHECK_TERM(arg);

  arity = ATgetArity(sym);
  assert(n < arity);

  hnr = START(appl->header);
  for (i=0; i<arity; i++) {
    if (i!=n) 
      hnr = COMBINE(hnr, (HashNumber)(char*)ATgetArgument(appl, i));
    else
      hnr = COMBINE(hnr, (HashNumber)(char*)arg);
  }
  
  /* if (annos)
    hnr = COMBINE(hnr, (HashNumber)(char*)annos); */
    
  hnr = FINISH(hnr);

  cur = (ATermAppl) hashtable[hnr & table_mask];
  while(cur) {
    if(EQUAL_HEADER(cur->header,appl->header)
       /*  && (AT_getAnnotations((ATerm)cur) == annos)) { */
       && (NULL == annos)) {
      found = ATtrue;
      for(i=0; i<arity; i++) {
        if (i!=n) {
          if(!ATisEqual(ATgetArgument(cur, i), ATgetArgument(appl, i))) {
            found = ATfalse;
            break;
          }
        }
        else {
          if(!ATisEqual(ATgetArgument(cur, i), arg)) {
            found = ATfalse;
            break;
          }
        }
      }
      if(found)
        break;
    }
    cur = (ATermAppl) cur->aterm.next;
  }

  if(!cur) {
    cur = (ATermAppl) AT_allocate(TERM_SIZE_APPL(arity) + (annos?1:0));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = HIDE_AGE_MARK(appl->header);
    CHECK_HEADER(cur->header);
    for (i=0; i<arity; i++) {
      if (i!=n)
        ATgetArgument(cur, i) = ATgetArgument(appl, i);
      else
        ATgetArgument(cur, i) = arg;
    }
    if (annos)
      ATgetArgument(cur, arity) = annos;
    cur->aterm.next = hashtable[hnr];
    hashtable[hnr] = (ATerm) cur;
  }

  return cur;
}

/*}}}  */
/*{{{  ATermList ATmakeList(int n, ...) */

/**
 * Create a list with n arguments.
 */

ATermList ATmakeList(unsigned int n, ...)
{
  unsigned int i;
  va_list args;
  ATermList l;
  ATerm* elems = AT_alloc_protected(n);

  va_start(args, n);
  for (i=0; i<n; i++) {
    elems[i] = va_arg(args, ATerm);
  }
  va_end(args);

  l = ATempty;
  for (i=n; i>0; i--) {
    l = ATinsert(l, elems[i-1]);
  }
  
  AT_free_protected(elems);

  return l;
}

/*}}}  */

/*{{{  void ATregisterBlobDestructor(ATbool (*destructor)(ATermBlob)) */

/**
 * Add a blob destructor.
 */

void ATregisterBlobDestructor(ATbool (*destructor)(ATermBlob))
{
  assert(destructor_count < MAX_DESTRUCTORS);
  destructors[destructor_count++] = destructor;
}

/*}}}  */
/*{{{  void ATunregisterBlobDestructor(ATbool (*destructor)(ATermBlob)) */

/**
 * Add a blob destructor.
 */

void ATunregisterBlobDestructor(ATbool (*destructor)(ATermBlob))
{
  unsigned int i;

  for(i=0; i<destructor_count; i++) {
    if(destructors[i] == destructor) {
      destructors[i] = destructors[--destructor_count];
      destructors[destructor_count] = 0;
      break;
    }
  }
}

/*}}}  */

/*{{{  ATermList AT_getAnnotations(ATerm t) */

/**
 * Retrieve the annotations of a term.
 */

/* ATerm AT_getAnnotations(ATerm t)
{
  if(HAS_ANNO(t->header)) {
    int size = term_size(t);
    return ((ATerm *)t)[size-1];
  }
  return NULL;
} */

/*}}}  */

/*{{{  ATerm ATsetAnnotation(ATerm t, ATerm label, ATerm anno) */

/* ATerm ATsetAnnotation(ATerm t, ATerm label, ATerm anno)
{
  ATerm newannos, oldannos = AT_getAnnotations(t);

  if(!oldannos)
    oldannos = ATdictCreate();

  newannos = ATdictPut(oldannos, label, anno);

  if(ATisEqual(oldannos, newannos))
    return t;

  return AT_setAnnotations(t, newannos);
} */

/*}}}  */
/*{{{  ATerm ATgetAnnotation(ATerm t, ATerm label) */

/**
 * Retrieve an annotation with a specific label.
 */

/* ATerm ATgetAnnotation(ATerm t, ATerm label)
{
  ATerm annos = AT_getAnnotations(t);
  if(!annos)
    return NULL;

  return ATdictGet(annos, label);
} */

/*}}}  */
/*{{{  ATerm ATremoveAnnotation(ATerm t, ATerm label) */

/**
 * Remove an annotation
 */

/* ATerm ATremoveAnnotation(ATerm t, ATerm label)
{
  ATerm newannos, oldannos = AT_getAnnotations(t);

  if(!oldannos)
    return t;

  newannos = ATdictRemove(oldannos, label);

  if(ATisEqual(newannos, oldannos))
    return t;

  if(ATisEmpty((ATermList)newannos))
    return AT_removeAnnotations(t);

  return AT_setAnnotations(t, newannos);
} */

/*}}}  */
/*{{{  ATbool ATisValidTerm(ATerm term) */

/**
 * Determine if a given term is valid.
 */

ATbool AT_isValidTerm(ATerm term)
{
  Block *cur;
  header_type header;
  ATbool inblock = ATfalse;
  int idx = ADDR_TO_BLOCK_IDX(term);
  int type;
  int offset = 0;

  assert(block_table[idx].first_after == block_table[(idx+1)%BLOCK_TABLE_SIZE].first_before);
  
  for(cur=block_table[idx].first_after; cur; cur=cur->next_after) 
  {
    if(cur->size) {
      assert(cur->next_before == cur->next_after);
      offset  = ((char *)term) - ((char *)&cur->data);
      if (offset >= 0        && offset < (int)(BLOCK_SIZE * sizeof(header_type))) {
        inblock = ATtrue;
        break;
      }
    }
  }

  if(!inblock)
  {
    for(cur=block_table[idx].first_before; cur; cur=cur->next_before) 
    {
      if(cur->size) {
        assert(cur->next_before == cur->next_after);
        offset  = ((char *)term) - ((char *)&cur->data);
        if (offset >= 0 && offset < (int)(BLOCK_SIZE * sizeof(header_type))) {
          inblock = ATtrue;
          break;
        }
      }
    }
  }

  if(!inblock) {
    return ATfalse;
  }

  /* Check if we point to the start of a term. Pointers inside terms
     are not allowed.
     */
  if(offset % (cur->size*sizeof(header))) {
    return ATfalse;
  }

  header = term->header;
  type = GET_TYPE(header);

  /* The only possibility left for an invalid term is AT_FREE */
  return (((type == AT_FREE) || (type == AT_SYMBOL)) ? ATfalse : ATtrue);
}

/*}}}  */

/**
 * Determine if a given term is valid.
 */


/*{{{  ATerm AT_isInsideValidTerm(ATerm term) */

ATerm AT_isInsideValidTerm(ATerm term)
{
  Block *cur;
  TermInfo* ti;
  ATbool inblock = ATfalse;
  int idx = ADDR_TO_BLOCK_IDX(term);
  int type;
  
  assert(block_table[idx].first_after == block_table[(idx+1)%BLOCK_TABLE_SIZE].first_before);

    /* Warning: symboles*/

  for(cur=block_table[idx].first_after; cur; cur=cur->next_after) {
    header_type *end;
    if(cur->size) {
      assert(cur->next_before == cur->next_after);
      ti = &terminfo[cur->size];

      if(cur != ti->at_blocks[AT_BLOCK]) {
        end = cur->end;
      } else {
        assert(ti->at_blocks[AT_BLOCK] != NULL);
        end = ti->top_at_blocks;
      }

      if(cur->data <= (header_type*)term && (header_type*)term < end) { 
        inblock = ATtrue;
        break;
      }
    }
  }
  
  if(!inblock) {
    for(cur=block_table[idx].first_before; cur; cur=cur->next_before) {
      header_type *end;
      if(cur->size) {
        ti = &terminfo[cur->size];
        assert(cur->next_before == cur->next_after);
        
        if(cur != ti->at_blocks[AT_BLOCK]) {
          end = cur->end;
        } else {
          assert(ti->at_blocks[AT_BLOCK] != NULL);
          end = ti->top_at_blocks;
        }
        
        if(cur->data <= (header_type*)term && (header_type*)term < end) { 
          inblock = ATtrue;
          break;
        }
      }
    }
  }
  
  if(!inblock) {
      /*fprintf(stderr, "not in block: %p\n", term);*/
    return NULL;
  }


  term = (ATerm)(((char*)term)-(((char*)term-((char*)cur->data))%(cur->size*sizeof(header_type)))); 
  
  type = GET_TYPE(term->header);

    /* The only possibility left for an invalid term is AT_FREE */
  return (((type == AT_FREE) || (type == AT_SYMBOL)) ? NULL : term);
}

/*}}}  */

/*{{{  void AT_validateFreeList(unsigned int size) */

void AT_validateFreeList(unsigned int size)
{
  ATerm cur1, cur2;

  for(cur1=terminfo[size].at_freelist; cur1; cur1=cur1->aterm.next) {
    for(cur2=cur1->aterm.next; cur2; cur2=cur2->aterm.next)
      assert(cur1 != cur2);
    assert(ATgetType(cur1) == AT_FREE);
  }

}

/*}}}  */
/*{{{  int AT_inAnyFreeList(ATerm t) */

/**
 * Check if a term is in any free list.
 */

int AT_inAnyFreeList(ATerm t)
{
  unsigned int i;

  for(i=MIN_TERM_SIZE; i<maxTermSize; i++) {
    ATerm cur = terminfo[i].at_freelist;

    while(cur) {
      if(cur == t)
        return i;
      cur = cur->aterm.next;
    }
  }
  return 0;
}

/*}}}  */

/*{{{  void AT_printAllTerms(FILE *file) */

void AT_printAllTerms(FILE *file)
{
  unsigned int i;

  for(i=0; i<table_size; i++) {
    ATerm cur = hashtable[i];
    while(cur) {
      ATfprintf(file, "%t\n", cur);
        /*fprintf(file, "sym = %s\n",ATgetName(ATgetAFun(cur)));*/
      
      cur = cur->aterm.next;
    }
  }
}

/*}}}  */
/*{{{  void AT_printAllAFunCounts(FILE *file) */

static int compare_afuns(const void *l, const void *r)
{
  AFun left, right;
  int left_count, right_count;

  left = *((AFun *)l);
  right = *((AFun *)r);

  if(left == -1)
    return 1;
  if(right == -1)
    return -1;

  left_count = at_lookup_table[left]->count;
  right_count = at_lookup_table[right]->count;

  if(left_count < right_count)
    return 1;

  if(left_count > right_count)
    return -1;

  return 0;
}

void AT_printAllAFunCounts(FILE *file)
{
  unsigned int i, nr_syms;
  AFun *afuns;

  nr_syms = AT_symbolTableSize();

  for(i=0; i<nr_syms; i++) {
    if(!SYM_IS_FREE(at_lookup_table[i]))
      at_lookup_table[i]->count = 0;
  }

  for(i=0; i<table_size; i++) {
    ATerm cur = hashtable[i];
    while(cur) {
      if(ATgetType(cur) == AT_APPL) {
        ATermAppl appl = (ATermAppl)cur;
        AFun fun = ATgetAFun(appl);
        at_lookup_table[fun]->count++;
      }
      cur = cur->aterm.next;
    }
  }

  afuns = (AFun *)AT_calloc(nr_syms, sizeof(AFun));
  assert(afuns);

  for(i=0; i<nr_syms; i++) {
    if(SYM_IS_FREE(at_lookup_table[i]))
      afuns[i] = -1;
    else
      afuns[i] = i;
  }

  qsort(afuns, nr_syms, sizeof(AFun), compare_afuns);

  for(i=0; i<nr_syms; i++) {
    if(afuns[i] != -1)
      ATfprintf(file, "%y: %d\n", afuns[i], at_lookup_table[afuns[i]]->count);
  }
}

/*}}}  */
/*{{{  int AT_calcAllocatedBytes() */


/**
 * Calculate all allocated bytes containing ATerms.
 */

unsigned long AT_calcAllocatedSize()
{
  unsigned int i;
  unsigned long total = 0;

  for(i=0; i<maxTermSize; i++)
    total += terminfo[i].at_nrblocks*sizeof(Block);

  total += table_size*sizeof(ATerm);

  return total;
}

/*}}}  */

