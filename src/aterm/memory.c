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

#define CHECK_ARITY(ari1,ari2) DBG_ARITY(assert((ari1) == (ari2)))


#define INFO_HASHING    1
#define MAX_INFO_SIZES 256

#ifdef HASHPEM
#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */

static HashNumber hash_number3(ub4 k0, ub4 k1, ub4 k2) {
   register ub4 a,b,c;
   c = 3;         /* the previous hash value */
   a = b = 0x9e3779b9;
   a += HIDE_AGE_MARK((k0)) + ((k1)<<8) + ((k2)<<16);
   mix(a,b,c);
   /*-------------------------------------------- report the result */
   return c;
}

static HashNumber hash_number4(ub4 k0, ub4 k1, ub4 k2, ub4 k3) {
   register ub4 a,b,c;
   c = 4;         /* the previous hash value */
   a = b = 0x9e3779b9;
   a += HIDE_AGE_MARK((k0)) + ((k1)<<8) + ((k2)<<16) + ((k3)<<24);
   mix(a,b,c);
   /*-------------------------------------------- report the result */
   return c;
}

/*
#define HASHNUMBER3(t) hash_number3(((MachineWord*)t)[0],\
                                    ((MachineWord*)t)[1],\
                                    ((MachineWord*)t)[2])
#define HASHNUMBER4(t) hash_number4(((MachineWord*)t)[0],\
                                    ((MachineWord*)t)[1],\
                                    ((MachineWord*)t)[2],\
                                    ((MachineWord*)t)[3])
#define HASHINT(val)   hash_number3((MachineWord)(AT_INT<<SHIFT_TYPE),\
                                    (MachineWord)0,\
                                    (MachineWord)val)
*/

static MachineWord tmp[3];
#define HASHNUMBER3(t) hash_number(t,3)
#define HASHNUMBER4(t) hash_number(t,4)
#define HASHINT(val)   (tmp[0]=(MachineWord)(AT_INT<<SHIFT_TYPE),\
                        tmp[1]=(MachineWord)0,\
                        tmp[2]=(MachineWord)val,\
                        hash_number(tmp,3)) 

#else

#define HN(i)          ((HashNumber)(i))
#define IDX(w)         HN(((w>>2) ^ (w>>10)) & 0xFF)
#define SHIFT(w)       HN((w>>3) & 0xF)

#define START(w)       FOLD(w)
#define COMBINE(hnr,w) ((hnr)<<1 ^ (hnr)>>1 ^ (HashNumber)(FOLD(w)))
#define FINISH(hnr)	   (hnr)

/*
static MachineWord tmp[3];
#define HASHNUMBER3(t) hash_number(t,3)
#define HASHNUMBER4(t) hash_number(t,4)
#define HASHINT(val)   (tmp[0]=(MachineWord)(AT_INT<<SHIFT_TYPE),\
                        tmp[1]=(MachineWord)0,\
                        tmp[2]=(MachineWord)val,\
                        hash_number(tmp,3)) 
*/

#define HASHNUMBER3(t)\
FINISH(COMBINE(START(((MachineWord*)t)[0]), ((MachineWord*)t)[2]))

#define HASHNUMBER4(t)\
FINISH(COMBINE(COMBINE(START(((MachineWord*)t)[0]), \
		       ((MachineWord*)t)[2]),((MachineWord*)t)[3]))

#define HASHINT(val) \
FINISH(COMBINE(START( (AT_INT<<SHIFT_TYPE) ), val))


#endif /* HASHPEM */

#define CHECK_TERM(t) assert(!at_check || \
			     (AT_isValidTerm(t) && "term is invalid"))

/*}}}  */
/*{{{  globals */

char memory_id[] = "$Id: memory.c,v 1.107 2004/06/09 08:52:33 uid506 Exp $";

Block *at_blocks[MAX_TERM_SIZE]  = { NULL };
Block *at_old_blocks[MAX_TERM_SIZE]  = { NULL };
header_type *top_at_blocks[MAX_TERM_SIZE]  = { NULL };
Block *at_freeblocklist = NULL;
int at_freeblocklist_size = 0;
ATerm at_freelist[MAX_TERM_SIZE] = { NULL };
BlockBucket block_table[BLOCK_TABLE_SIZE] = { { NULL, NULL } };

int at_nrblocks[MAX_TERM_SIZE]   = { 0 };
MachineWord total_nodes = 0;

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
int nb_live_blocks_before_last_gc[MAX_TERM_SIZE] = { 0 }; /* holes included */
int nb_reclaimed_blocks_during_last_gc[MAX_TERM_SIZE] = { 0 }; /* dead blocks */
int nb_reclaimed_cells_during_last_gc[MAX_TERM_SIZE] = { 0 };

static int maxload = 80;
static ATerm *hashtable;

static int destructor_count = 0;
static ATbool (*destructors[MAX_DESTRUCTORS])(ATermBlob) = { NULL };

/* Do not optimize protoTerm to use static array.
 * Static arrays are not guaranteed to be sizeof(double)-aligned.
 */
static MachineWord *protoTerm = NULL;
static ATerm *arg_buffer = NULL;

static ATerm protected_buffer[MAX_ARITY] = { NULL };

ATermList ATempty;

static int infoflags = 0;

static int hash_info_before_gc[MAX_INFO_SIZES][3];
static int hash_info_after_gc[MAX_INFO_SIZES][3];

static ATbool at_check = ATfalse;

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
  int size = 0;

  if(HAS_ANNO(t->header)) {
    size++;
  }

  switch(ATgetType(t)) {
    case AT_INT:
      size += TERM_SIZE_INT;
      break;
    case AT_PLACEHOLDER:
      size += TERM_SIZE_PLACEHOLDER;
      break;
    case AT_REAL:
      size += TERM_SIZE_REAL;
      break;
    case AT_LIST:
      size += TERM_SIZE_LIST;
      break;
    case AT_BLOB:
      size += TERM_SIZE_BLOB;
      break;
    case AT_APPL:
      size += 2+ATgetArity(ATgetSymbol(t));
      break;
  }
  return size;
}

/*}}}  */
/*{{{  static HashNumber hash_number(ATerm t, int size) */

#ifdef HASHPEM
#define mw MachineWord
static HashNumber hash_number(ATerm t, int size) {
   register mw a,b,c,len;
   mw *k = (mw*) t;

   /* Set up the internal state */
   len = size;
   a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
   c = 0;         /* the previous hash value */
   
   /*---------------------------------------- handle most of the key */
   while (len > 12) {
     a += ((((mw*)k)[0]) + (((mw*)k)[1]<<8) + (((mw*)k)[2]<<16) +(((mw*)k)[3]<<24));
     b += ((((mw*)k)[4]) +(((mw*)k)[5]<<8) +(((mw*)k)[6]<<16) +(((mw*)k)[7]<<24));
     c += ((((mw*)k)[8]) +(((mw*)k)[9]<<8) +(((mw*)k)[10]<<16)+(((mw*)k)[11]<<24));
     mix(a,b,c);
     k += 12; len -= 12;
   }
   if(len == 12) {
     a += (HIDE_AGE_MARK(((mw*)k)[0]) + /*(((mw*)k)[1]<<8) +*/
           (((mw*)k)[2]<<16) +(((mw*)k)[3]<<24));
     b += ((((mw*)k)[4]) +(((mw*)k)[5]<<8) +(((mw*)k)[6]<<16) +(((mw*)k)[7]<<24));
     c += ((((mw*)k)[4]) +(((mw*)k)[9]<<8) +(((mw*)k)[10]<<16)+(((mw*)k)[11]<<24));
     mix(a,b,c);
     k += 12; len -= 12;
   }
   /*------------------------------------- handle the last 11 bytes */
   c += size;
   switch(len)              /* all the case statements fall through */
   {
   case 11: c+=(((mw*)k)[10]<<24);
   case 10: c+=(((mw*)k)[9]<<16);
   case 9 : c+=(((mw*)k)[8]<<8);
      /* the first byte of c is reserved for the length */
   case 8 : b+=(((mw*)k)[7]<<24);
   case 7 : b+=(((mw*)k)[6]<<16);
   case 6 : b+=(((mw*)k)[5]<<8);
   case 5 : b+=(((mw*)k)[4]);
   case 4 : a+=(((mw*)k)[3]<<24);
   case 3 : a+=(((mw*)k)[2]<<16);
       case 2 : /* a+=(((mw*)k)[1]<<8); */
   case 1 : a+=HIDE_AGE_MARK(((mw*)k)[0]);
     /* case 0: nothing left to add */
   }
   mix(a,b,c);
   /*-------------------------------------------- report the result */
     /*fprintf(stderr,"hash_number(%p,%d) = %u\tsize = %d\n",t,size,c,table_size);*/
   return c;
}

static HashNumber hash_number_anno(ATerm t, int size, ATerm anno) {
  HashNumber hnr;
  hnr = hash_number(t,size);
  fprintf(stderr,"hash_number_anno(%p) = %d\tsize = %d\n",t,hnr,table_size);
  return hnr;
}

#else
static HashNumber hash_number(ATerm t, int size)
{
  MachineWord *words = (MachineWord *) t;
  int i;
  HashNumber hnr;

  hnr = START(HIDE_AGE_MARK(words[0]));
  
  for (i=2; i<size; i++) {
    hnr = COMBINE(hnr, words[i]);
  }

  return FINISH(hnr);
}

static HashNumber hash_number_anno(ATerm t, int size, ATerm anno)
{
  MachineWord *words = (MachineWord *) t;
  int i;
  HashNumber hnr;

  hnr = START(HIDE_AGE_MARK(words[0]));
  
  for (i=2; i<size; i++) {
    hnr = COMBINE(hnr, words[i]);
  }
  hnr = COMBINE(hnr, (MachineWord)anno);

  return FINISH(hnr);
}

#endif /* HASHPEM */


/*}}}  */
/*{{{  HashNumber AT_hashnumber(ATerm t) */

HashNumber AT_hashnumber(ATerm t)
{
  return hash_number(t, term_size(t));
}

/*}}}  */
/*{{{  static void hash_info(int stats[3][]) */

static void hash_info(int stats[MAX_INFO_SIZES][3]) 
{
  int i, len;
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
      cur = cur->next; 
    }
    if(len >= MAX_INFO_SIZES)
      len = MAX_INFO_SIZES-1;
    count[len]++;
  }
}

/*}}}  */
/*{{{  static void resize_hashtable() */

/**
 * Resize the hashtable
 */

void resize_hashtable()
{
  ATerm *oldtable;
  ATerm *newhalf, *p;
  HashNumber oldsize;

    /*fprintf(stderr, "warning: do not resize hashtable\n"); */
    /*return;*/
  
  oldtable = hashtable;
  oldsize = table_size;

  table_class++;
  table_size = 1<<table_class;
  table_mask = table_size-1;
  if (!silent)
    fprintf(stderr, "resizing hashtable, class = %d\n", table_class);

  /*{{{  Create new term table */
  hashtable = (ATerm *) realloc(hashtable, table_size * sizeof(ATerm));
  if (!hashtable) {
    fprintf(stderr, "warning: could not resize hashtable to class %d.\n",
	    table_class);
    table_class--;
    hashtable = oldtable;
    table_size = oldsize;
    table_mask = oldsize-1;
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
    /*{{{  Loop over marked part */
    while(marked && IS_MARKED(marked->header)) {
      CLR_MARK(marked->header);
      marked = marked->next;
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
	unmarked = marked->next;
	marked->next = NULL;
      }

      while(unmarked) {
	ATerm next = unmarked->next;
	HashNumber hnr;

	hnr = hash_number(unmarked, term_size(unmarked));
	hnr &= table_mask;
	hashspot = hashtable+hnr;
	unmarked->next = *hashspot;
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

/*{{{  void AT_initMemory(int argc, char *argv[]) */

/**
 * Initialize memory allocation datastructures
 */

void AT_initMemory(int argc, char *argv[])
{
  int i;
  HashNumber hnr;

  protoTerm  = (MachineWord *) calloc(MAX_TERM_SIZE, sizeof(MachineWord));
  arg_buffer = (ATerm *) (protoTerm + 2);

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
    } else if(strcmp(argv[i], "-at-help") == 0) {
      fprintf(stderr, "    %-20s: initial termtable size " 
	      "(2^size, default=%d)\n",	TERM_HASH_OPT " <size>", table_class);
      fprintf(stderr, "    %-20s: write information to 'hashing.stats'\n",
	      HASH_INFO_OPT);
#ifdef NDEBUG
      fprintf(stderr, "    %-20s: check subterm validity at term creation "
	      "(disabled!)\n", TERM_CHECK_OPT);
#else
      fprintf(stderr, "    %-20s: check subterm validity at term creation\n", 
	      TERM_CHECK_OPT);
#endif
    } 
  }

  /*}}}  */

  /*{{{  Initialize blocks */

  for(i=0; i<MAX_TERM_SIZE; i++) {
    at_nrblocks[i] = 0;
    at_freelist[i] = NULL;
  }

  /*}}}  */
  /*{{{  Create term term table */

  hashtable = (ATerm *)calloc(table_size, sizeof(ATerm ));
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
  ATempty->next = NULL;
  ATempty->head = NULL;
  ATempty->tail = NULL;

  hnr = hash_number((ATerm)ATempty, TERM_SIZE_LIST);
  hashtable[hnr & table_mask] = (ATerm)ATempty;

  ATprotectList(&ATempty);

  /*}}}  */
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
  ATprotectArray(protected_buffer, MAX_ARITY);
}

/*}}}  */
/*{{{  void AT_cleanupMemory() */

/**
 * Print hashtable info
 */

void AT_cleanupMemory()
{
  int i, info[MAX_INFO_SIZES][3];

  if(infoflags & INFO_HASHING) {
    int max = MAX_INFO_SIZES-1;
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
	cur = cur->next;
      if(size > 5) {
	fprintf(f, "bucket %d has length %d\n", i, size);
	cur = hashtable[i];
	while(cur) {
	  ATfprintf(f, "%t\n", cur);
	  cur = cur->next;
	}
      }
    }
  }
}

/*}}}  */

/**
 * Allocate a new block of a particular size class
 */

header_type *min_heap_address = (header_type*)(~0);
header_type *max_heap_address = 0;

/*{{{  static void allocate_block(int size)  */

static void allocate_block(int size) 
{
  int idx;
  Block *newblock;
  int init = 0;
  
  if(at_freeblocklist != NULL) {
#ifdef GC_VERBOSE
    fprintf(stderr,"allocate_block %p from at_freeblocklist\n",at_freeblocklist);
#endif
    newblock = at_freeblocklist;
    at_freeblocklist = at_freeblocklist->next_by_size;
    at_freeblocklist_size--;
  } else {
    newblock = (Block *)calloc(1, sizeof(Block));
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

  at_nrblocks[size]++;
  
  assert(size >= MIN_TERM_SIZE && size < MAX_TERM_SIZE);

  /*
    printf("allocate_block[%d] = %d*%d\ttotal_nodes = %d\n",
       size, at_nrblocks[size],sizeof(Block),total_nodes);
    printf("at_blocks[%d] = %d\n",size,at_blocks[size]);
  */

  newblock->end = (newblock->data) + (BLOCK_SIZE - (BLOCK_SIZE % size));

  CLEAR_FROZEN(newblock);
  newblock->size = size;
  newblock->next_by_size = at_blocks[size];
  at_blocks[size] = newblock;
  top_at_blocks[size] = newblock->data;
  assert(at_blocks[size] != NULL);
  assert(((int)top_at_blocks[size] % MAX(sizeof(double), sizeof(void *))) == 0);
  
    /* [pem: Feb 14 02] TODO: fast allocation */
  assert(at_freelist[size] == NULL);

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
        assert(at_blocks[size] != NULL);\
        at = (ATerm)top_at_blocks[size];\
        top_at_blocks[size] += size;

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


/*}}}  */

static int nb_at_allocate=0;
/*{{{  void AT_statistics()  */

void AT_statistics() 
{
  int size;
  Block *block;
  header_type *cur;
  int old_in_old_heap=0;
  int old_in_young_heap=0;
  int young_in_heap=0;
  int free_in_heap=0;
    /*
     * STATISTICS
     */
  for(size=MIN_TERM_SIZE; size<MAX_TERM_SIZE; size++) {
    block = at_old_blocks[size];
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

    block = at_blocks[size];
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
  fprintf(stderr,"nb_at_allocate = %d\n",nb_at_allocate);
  fprintf(stderr,"young in heap = %d kB\n",young_in_heap/1024);
  fprintf(stderr,"old   in heap = %d kB\t(%d in old + %d in young)\n",
          (old_in_old_heap+old_in_young_heap)/1024,
          old_in_old_heap/1024, old_in_young_heap/1024);
  fprintf(stderr,"free  in heap = %d kB\n",free_in_heap/1024);

  fprintf(stderr,"mpold %d\t%d\n",
          nb_at_allocate,
          (old_in_young_heap+old_in_old_heap)/1024);

  fprintf(stderr,"mpyoung %d\t%d\n",
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

/*{{{  ATerm AT_allocate(int size)  */

ATerm AT_allocate(int size) 
{
  ATerm at;
  nb_at_allocate++;
  
  while(1) {
    if(at_blocks[size] && top_at_blocks[size] < at_blocks[size]->end) {
        /* the first block is not full: allocate a cell */
      at = (ATerm)top_at_blocks[size];
      top_at_blocks[size] += size;
      break;

    } else if(at_freelist[size]) {
        /* the freelist is not empty: allocate a cell */
      at = at_freelist[size];
      at_freelist[size] = at_freelist[size]->next;
      assert(at_blocks[size] != NULL);
      assert(top_at_blocks[size] == at_blocks[size]->end);
      break;

    } else {
        /* there is no more memory: run the GC or allocate a block */
#ifdef GC_VERBOSE
  fprintf(stderr,"AT_allocate(%d)\n",size);
#endif
  
      if(at_nrblocks[size] <= gc_min_number_of_blocks) {
#ifdef GC_VERBOSE
        fprintf(stderr,"INITIAL PHASE -> ALLOCATE_BLOCK\n");
#endif
        ALLOCATE_BLOCK_TEXT;
      } else {
        int reclaimed_memory_during_last_gc =
            /*(nb_reclaimed_blocks_during_last_gc[size]*sizeof(Block)) +*/
          (nb_reclaimed_cells_during_last_gc[size]*SIZE_TO_BYTES(size));
          /* +1 to avoid division by zero */
        int reclaimed_memory_ratio_during_last_gc =
          (100*reclaimed_memory_during_last_gc) / (1+nb_live_blocks_before_last_gc[size]*sizeof(Block));
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
          int nb_allocated_blocks_since_last_gc = at_nrblocks[size]-nb_live_blocks_before_last_gc[size];
            /* +1 to avoid division by zero */
          int allocation_rate =
            (100*nb_allocated_blocks_since_last_gc)/(1+nb_live_blocks_before_last_gc[size]);
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
    /*fprintf(stderr,"AT_allocate term[%d] = %x\n",size,(unsigned int)at);*/
  return at;
}

/*}}}  */

/*{{{  void AT_freeTerm(int size, ATerm t) */

/**
 * Free a term of a particular size.
 */

void AT_freeTerm(int size, ATerm t)
{
  int i;
  HashNumber hnr = hash_number(t, size);
  ATerm prev = NULL, cur;

  nb_reclaimed_cells_during_last_gc[size]++;
  
    /*fprintf(stderr,"AT_freeTerm term[%d] = %x\theader = %x\n",size,(unsigned int)t,t->header);*/
  
    /* The data of a blob needs to be freed!!! */
  if (ATgetType(t) == AT_BLOB) {
    ATbool destructed = ATfalse;
      /*ATfprintf(stderr, "freeing blob %p (%p): %t\n", t, ATgetBlobData((ATermBlob)t), t);*/
    for (i=0; i<destructor_count; i++) {
        /*fprintf(stderr,"apply destructors[%d] on (%d)\n",i,t);*/
      if ((destructors[i])((ATermBlob)t)) {
	destructed = ATtrue;
	break;
      }
    }
      /*printf("destructed = %d\n",destructed);*/
    if (!destructed) {
        /*printf("free BlobData(%d)\n",ATgetBlobData((ATermBlob)t));*/
      free(ATgetBlobData((ATermBlob)t));
    }
  }

    /* Remove the node from the hashtable */
  hnr &= table_mask; 
  cur = hashtable[hnr];

  do {
    if(!cur) {
        /*printf("freeterm = %d\n",t);*/
      fprintf(stderr,"### cannot find term %x in hashtable at pos %d header = %x\n", (unsigned int)t, hnr, t->header);

      ATabort("### cannot find term %n at %p in hashtable at pos %d"
	      ", header = %d\n", t, t, hnr, t->header);
    }
    if (cur == t) {
      if(prev)
	prev->next = cur->next;
      else
	hashtable[hnr] = cur->next;
      /* Put the node in the appropriate free list */
      /*printf("AT_freeTerm: put cell[%d] %x into freelist\n",size,t);*/
      total_nodes--;
      return;
    }
  } while(((prev=cur), (cur=cur->next)));
}

/*}}}  */

/*{{{  ATermAppl ATmakeAppl(Symbol sym, ...) */

/**
 * Create a new ATermAppl. The argument count can be found in the symbol.
 */

ATermAppl ATmakeAppl(Symbol sym, ...)
{
  ATermAppl protoAppl;
  int i, arity = ATgetArity(sym);
  ATbool found;
  ATerm cur;
  ATermAppl appl;
  header_type header;
  HashNumber hnr;
  va_list args;

  protoAppl = (ATermAppl) protoTerm;

  va_start(args, sym);
  for (i=0; i<arity; i++) {
    arg_buffer[i] = va_arg(args, ATerm);
    protected_buffer[i] = arg_buffer[i];
    CHECK_TERM(arg_buffer[i]);
  }
  va_end(args);

  header = APPL_HEADER(0, arity > MAX_INLINE_ARITY ?
		       MAX_INLINE_ARITY+1 : arity, sym);

  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);

  hnr = hash_number((ATerm)protoAppl, arity + 2);
  cur = hashtable[hnr & table_mask];

  while (cur) {
    if (EQUAL_HEADER(cur->header,header)) {
      appl = (ATermAppl)cur;
      found = ATtrue;
      for (i=0; i<arity; i++) {
	if (!ATisEqual(ATgetArgument(appl, i), arg_buffer[i])) {
	  found = ATfalse;
	  break;
	}
      }
      if (found)
	break;
    }
    cur = cur->next;
  }

  if (!cur) {
    cur = AT_allocate(arity + ARG_OFFSET);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    for (i=0; i<arity; i++) {
      ATgetArgument(cur, i) = arg_buffer[i];
      CHECK_ARGUMENT(cur, i);
    }
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  for (i=0; i<arity; i++) {
    protected_buffer[i] = NULL;
  }

  return (ATermAppl)cur;
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl0(Symbol sym) */

/**
 * Create an ATermAppl with zero arguments.
 */

ATermAppl ATmakeAppl0(Symbol sym)
{
  ATermAppl protoAppl;
  ATerm cur, prev;
  ATerm *hashspot;
  header_type header = APPL_HEADER(0, 0, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_ARITY(ATgetArity(sym), 0);

  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);
  hnr = hash_number((ATerm) protoAppl, 2);

  prev = NULL;
  hashspot = &(hashtable[hnr & table_mask]);

  for(cur = *hashspot; cur; cur = cur->next) {
    if(EQUAL_HEADER(cur->header,header)) {
      /* Promote current entry to front of hashtable */
      if (prev != NULL) {
	prev->next = cur->next;
	cur->next = *hashspot;
	*hashspot = cur;
      }

        /*fprintf(stderr,"ATmakeAppl0 found = %x\n",(unsigned int)cur);*/
      return (ATermAppl)cur;
    }
    prev = cur;
  }

  cur = AT_allocate(ARG_OFFSET);
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  CHECK_HEADER(cur->header);
  cur->next = hashtable[hnr];
  hashtable[hnr] = cur;

    /*fprintf(stderr,"ATmakeAppl0 alloc = %x\n",(unsigned int)cur);*/
  return (ATermAppl) cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl1(Symbol sym, ATerm arg0) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl1(Symbol sym, ATerm arg0)
{
  ATermAppl protoAppl;
  ATerm cur, prev;
  ATerm *hashspot;
  header_type header = APPL_HEADER(0, 1, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_ARITY(ATgetArity(sym), 1);
  
  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);
  arg_buffer[0] = arg0;
  hnr = HASHNUMBER3((ATerm) protoAppl);
  
  prev = NULL;
  hashspot = &(hashtable[hnr & table_mask]);

  for(cur = *hashspot; cur; cur = cur->next) {
    if(EQUAL_HEADER(header,cur->header) && ATgetArgument(cur, 0) == arg0) {
        /* Promote current entry to front of hashtable */
      if (prev != NULL) {
	prev->next = cur->next;
	cur->next = *hashspot;
	*hashspot = cur;
      }
      
        /*fprintf(stderr,"ATmakeAppl1 found = %p\tsub = %p\n",cur,ATgetArgument(cur, 0));*/

      CHECK_ARGUMENT(cur, 0);
      return (ATermAppl)cur;
    }
    prev = cur;
  }

  cur = AT_allocate(ARG_OFFSET+1);
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  CHECK_HEADER(cur->header);
  ATgetArgument(cur, 0) = arg0;
  CHECK_ARGUMENT(cur, 0);
  cur->next = hashtable[hnr];
  hashtable[hnr] = cur;

    /*fprintf(stderr,"ATmakeAppl1 alloc = %x\tsub = %x\n",(unsigned int)cur,(unsigned int)ATgetArgument(cur, 0));*/
  return (ATermAppl) cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl2(Symbol sym, arg0, arg1) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl2(Symbol sym, ATerm arg0, ATerm arg1)
{
  ATermAppl protoAppl;
  ATerm cur, prev;
  ATerm *hashspot;
  header_type header = APPL_HEADER(0, 2, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_ARITY(ATgetArity(sym), 2);

  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);
  arg_buffer[0] = arg0;
  arg_buffer[1] = arg1;
  hnr = HASHNUMBER4((ATerm) protoAppl);

  prev = NULL;
  hashspot = &(hashtable[hnr & table_mask]);

  for(cur = *hashspot; cur; cur = cur->next) {
    if(EQUAL_HEADER(cur->header,header) && ATgetArgument(cur, 0) == arg0 &&
       ATgetArgument(cur, 1) == arg1) {
      /* Promote current entry to front of hashtable */
      if (prev != NULL) {
	prev->next = cur->next;
	cur->next = *hashspot;
	*hashspot = cur;
      }
      CHECK_ARGUMENT(cur, 0);
      CHECK_ARGUMENT(cur, 1);
      return (ATermAppl)cur;
    }
    prev = cur;
  }

  cur = AT_allocate(ARG_OFFSET+2);
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  CHECK_HEADER(cur->header);
  ATgetArgument(cur, 0) = arg0;
  ATgetArgument(cur, 1) = arg1;
  CHECK_ARGUMENT(cur, 0);
  CHECK_ARGUMENT(cur, 1);
  cur->next = hashtable[hnr];
  hashtable[hnr] = cur;

  return (ATermAppl)cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl3(Symbol sym, ATerm arg0, arg1, arg2) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl3(Symbol sym, ATerm arg0, ATerm arg1, ATerm arg2)
{
  ATermAppl protoAppl;
  ATerm cur;
  header_type header = APPL_HEADER(0, 3, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_ARITY(ATgetArity(sym), 3);

  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);
  arg_buffer[0] = arg0;
  arg_buffer[1] = arg1;
  arg_buffer[2] = arg2;
  hnr = hash_number((ATerm) protoAppl, 5);

  cur = hashtable[hnr & table_mask];
  while(cur && (!EQUAL_HEADER(cur->header,header) ||
		ATgetArgument(cur, 0) != arg0 ||
		ATgetArgument(cur, 1) != arg1 ||
		ATgetArgument(cur, 2) != arg2)) {
    cur = cur->next;
  }

  if(!cur) {
    cur = AT_allocate(ARG_OFFSET+3);
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

    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermAppl)cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl4(Symbol sym, ATerm arg0, arg1, arg2, a3) */

/**
 * Create an ATermAppl with four arguments.
 */

ATermAppl ATmakeAppl4(Symbol sym, ATerm arg0, ATerm arg1, ATerm arg2, ATerm arg3)
{
  ATermAppl protoAppl;
  ATerm cur;
  header_type header;
  HashNumber hnr;

  PARK_SYMBOL(sym);

  header = APPL_HEADER(0, 4, sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_ARITY(ATgetArity(sym), 4);

  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);
  arg_buffer[0] = arg0;
  arg_buffer[1] = arg1;
  arg_buffer[2] = arg2;
  arg_buffer[3] = arg3;
  hnr = hash_number((ATerm) protoAppl, 6);

  cur = hashtable[hnr & table_mask];
  while(cur && (!EQUAL_HEADER(cur->header,header) ||
		ATgetArgument(cur, 0) != arg0 ||
		ATgetArgument(cur, 1) != arg1 ||
		ATgetArgument(cur, 2) != arg2 ||
		ATgetArgument(cur, 3) != arg3))
    cur = cur->next;

  if(!cur) {
    cur = AT_allocate(ARG_OFFSET+4);
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

    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermAppl)cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl5(Symbol sym, ATerm arg0, arg1, arg2, a3, a4) */

/**
 * Create an ATermAppl with five arguments.
 */

ATermAppl ATmakeAppl5(Symbol sym, ATerm arg0, ATerm arg1, ATerm arg2, 
		      ATerm arg3, ATerm arg4)
{
  ATermAppl protoAppl;
  ATerm cur;
  header_type header = APPL_HEADER(0, 5, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);

  CHECK_ARITY(ATgetArity(sym), 5);

  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);
  arg_buffer[0] = arg0;
  arg_buffer[1] = arg1;
  arg_buffer[2] = arg2;
  arg_buffer[3] = arg3;
  arg_buffer[4] = arg4;
  hnr = hash_number((ATerm) protoAppl, 7);

  cur = hashtable[hnr & table_mask];
  while(cur && (!EQUAL_HEADER(cur->header,header) ||
		ATgetArgument(cur, 0) != arg0 ||
		ATgetArgument(cur, 1) != arg1 ||
		ATgetArgument(cur, 2) != arg2 ||
		ATgetArgument(cur, 3) != arg3 ||
		ATgetArgument(cur, 4) != arg4))
    cur = cur->next;

  if(!cur) {
    cur = AT_allocate(ARG_OFFSET+5);
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

    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermAppl)cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl6(Symbol sym, ATerm arg0, arg1, arg2, a3, a4, a5) */

/**
 * Create an ATermAppl with six arguments.
 */

ATermAppl ATmakeAppl6(Symbol sym, ATerm arg0, ATerm arg1, ATerm arg2, 
		      ATerm arg3, ATerm arg4, ATerm arg5)
{
  ATermAppl protoAppl;
  ATerm cur;
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

  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);
  arg_buffer[0] = arg0;
  arg_buffer[1] = arg1;
  arg_buffer[2] = arg2;
  arg_buffer[3] = arg3;
  arg_buffer[4] = arg4;
  arg_buffer[5] = arg5;
  hnr = hash_number((ATerm) protoAppl, 8);

  cur = hashtable[hnr & table_mask];
  while(cur && (!EQUAL_HEADER(cur->header,header) ||
		ATgetArgument(cur, 0) != arg0 ||
		ATgetArgument(cur, 1) != arg1 ||
		ATgetArgument(cur, 2) != arg2 ||
		ATgetArgument(cur, 3) != arg3 ||
		ATgetArgument(cur, 4) != arg4 ||
		ATgetArgument(cur, 5) != arg5))
    cur = cur->next;

  if(!cur) {
    cur = AT_allocate(ARG_OFFSET+6);
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

    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermAppl)cur;  
}

/*}}}  */
/*{{{  ATermAppl ATmakeApplList(Symbol sym, ATermList args) */

/**
 * Build a function application from a symbol and a list of arguments.
 */

ATermAppl ATmakeApplList(Symbol sym, ATermList args)
{
  ATermAppl protoAppl;
  int i, arity = ATgetArity(sym);
  ATbool found;
  ATerm cur;
  ATermAppl appl;
  header_type header = APPL_HEADER(0, arity > MAX_INLINE_ARITY ?
				   MAX_INLINE_ARITY+1 : arity, sym);
  HashNumber hnr;

  PARK_SYMBOL(sym);
  protected_buffer[0] = (ATerm)args;

  CHECK_TERM((ATerm)args);
  assert(arity == ATgetLength(args));

  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);

  for (i=0; i<arity; i++) {
    arg_buffer[i] = ATgetFirst(args);
    args = ATgetNext(args);
  }

  hnr = hash_number((ATerm) protoAppl, arity + 2);

  cur = hashtable[hnr & table_mask];
  while(cur)
  {
    if(EQUAL_HEADER(cur->header,header))
    {
      appl = (ATermAppl)cur;
      found = ATtrue;
      for(i=0; i<arity; i++)
      {
	if(!ATisEqual(ATgetArgument(appl, i), arg_buffer[i]))
	{
	  found = ATfalse;
	  break;
	}
      }
      if(found)
	break;
    }
    cur = cur->next;
  }

  if(!cur)
  {
    cur = AT_allocate(ARG_OFFSET + arity);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    for (i=0; i<arity; i++) {
      ATgetArgument(cur, i) = arg_buffer[i];
      CHECK_ARGUMENT(cur, i);
    }
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  protected_buffer[0] = NULL;

  return (ATermAppl)cur;
}

/*}}}  */
/*{{{  ATermAppl ATmakeApplArray(Symbol sym, ATerm args[]) */

/**
 * Build a function application from a symbol and an array of arguments.
 */

ATermAppl ATmakeApplArray(Symbol sym, ATerm args[])
{
  ATermAppl protoAppl;
  int i, arity = ATgetArity(sym);
  ATbool found;
  ATerm cur;
  ATermAppl appl;
  HashNumber hnr;
  header_type header = APPL_HEADER(0, arity > MAX_INLINE_ARITY ?
				   MAX_INLINE_ARITY+1 : arity, sym);

  PARK_SYMBOL(sym);

  protoAppl = (ATermAppl) protoTerm;
  protoAppl->header = header;
  CHECK_HEADER(protoAppl->header);

  if (args != arg_buffer) {
    for (i=0; i<arity; i++) {
      CHECK_TERM(args[i]);
      arg_buffer[i] = args[i];
      protected_buffer[i] = args[i];
    }
  }

  hnr = hash_number((ATerm) protoAppl, arity + 2);

  cur = hashtable[hnr & table_mask];
  while(cur) {
    if(EQUAL_HEADER(cur->header,header)) {
      appl = (ATermAppl)cur;
      found = ATtrue;
      for(i=0; i<arity; i++) {
	if(!ATisEqual(ATgetArgument(appl, i), arg_buffer[i])) {
	  found = ATfalse;
	  break;
	}
      }
      if(found)
	break;
    }
    cur = cur->next;
  }

  if(!cur) {
    cur = AT_allocate(ARG_OFFSET + arity);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    for (i=0; i<arity; i++) {
      ATgetArgument(cur, i) = arg_buffer[i];
      CHECK_ARGUMENT(cur, i);
    }
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  if (args != arg_buffer) {
    for (i=0; i<arity; i++) {
      protected_buffer[i] = NULL;
    }
  }

  return (ATermAppl)cur;
}

/*}}}  */

/*{{{  ATermInt ATmakeInt(int val) */

/**
 * Create an ATermInt
 */

ATermInt ATmakeInt(int val)
{
  ATerm cur;
  header_type header = INT_HEADER(0);
  HashNumber hnr;
#ifdef AT_64BIT
  MachineWord *words;
#endif

  hnr = HASHINT(val);

  cur = hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header) || ((ATermInt)cur)->value != val)) {
    cur = cur->next;
  }

  if (!cur) {
    cur = AT_allocate(TERM_SIZE_INT);
#ifdef AT_64BIT
    /* Clear unused half of int word */
    words = (MachineWord *)cur;   
    words[2] = 0;
#endif
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ((ATermInt)cur)->value = val;
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermInt)cur;  
}

/*}}}  */
/*{{{  ATermReal ATmakeReal(double val) */

/**
 * Create an ATermReal
 */

ATermReal ATmakeReal(double val)
{
  ATermReal protoReal;
  ATerm cur;
  header_type header = REAL_HEADER(0);
  HashNumber hnr;

  protoReal = (ATermReal) protoTerm;
  protoReal->header = header;
  CHECK_HEADER(protoReal->header);
  protoReal->value = val;

  hnr = hash_number((ATerm)protoReal, TERM_SIZE_REAL);

  cur = hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header) || ((ATermReal)cur)->value != val)) {
    cur = cur->next;
  }

  if (!cur) {
    cur = AT_allocate(TERM_SIZE_REAL);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ((ATermReal)cur)->value = val;
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermReal)cur;  
}

/*}}}  */

/*{{{  ATermList ATmakeList1(ATerm el) */

/**
 * Build a list with one element.
 */

ATermList ATmakeList1(ATerm el)
{
  ATermList protoList;
  ATerm cur;
  header_type header = LIST_HEADER(0, 1);
  HashNumber hnr;

  CHECK_TERM(el);

  protoList = (ATermList) protoTerm;
  protoList->header = header;
  CHECK_HEADER(protoList->header);
  protoList->head = el;
  protoList->tail = ATempty;

  hnr = HASHNUMBER4((ATerm)protoList);

  cur = hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
		 || ATgetFirst((ATermList)cur) != el
		 || ATgetNext((ATermList)cur) != ATempty)) {
    cur = cur->next;
  }

  if (!cur) {
    cur = AT_allocate(TERM_SIZE_LIST);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ATgetFirst((ATermList)cur) = el;
    ATgetNext((ATermList)cur) = ATempty;
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermList) cur;
}

/*}}}  */
/*{{{  ATermList ATinsert(ATermList tail, ATerm el) */

/**
 * Insert an element at the front of a list.
 */

ATermList ATinsert(ATermList tail, ATerm el)
{
  ATermList protoList;
  ATerm cur;
  header_type header = LIST_HEADER(0, (GET_LENGTH(tail->header)+1));
  HashNumber hnr;

  CHECK_TERM((ATerm)tail);
  CHECK_TERM(el);

  assert(ATgetType(tail) == AT_LIST);

  protoList = (ATermList) protoTerm;
  protoList->header = header;
  CHECK_HEADER(protoList->header);
  protoList->head = el;
  protoList->tail = tail;

  hnr = HASHNUMBER4((ATerm)protoList);

  cur = hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
		 || ATgetFirst((ATermList)cur) != el
		 || ATgetNext((ATermList)cur) != tail)) {
    cur = cur->next;
  }

  if (!cur) {
    cur = AT_allocate(TERM_SIZE_LIST);
    /* Hashtable might be resized, so delay masking until after AT_allocate */
    hnr &= table_mask; 
    cur->header = header;
    CHECK_HEADER(cur->header);
    ATgetFirst((ATermList)cur) = el;
    ATgetNext((ATermList)cur) = tail;
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermList) cur;
}

/*}}}  */

/*{{{  ATermPlaceholder ATmakePlaceholder(ATerm type) */

/**
 * Create a new placeholder.
 */

ATermPlaceholder ATmakePlaceholder(ATerm type)
{
  ATermPlaceholder protoPlaceholder;
  ATerm cur;
  header_type header = PLACEHOLDER_HEADER(0);
  HashNumber hnr;

  CHECK_TERM(type);

  protoPlaceholder = (ATermPlaceholder) protoTerm;
  protoPlaceholder->header = header;
  CHECK_HEADER(protoPlaceholder->header);
  protoPlaceholder->ph_type = type;

  hnr = hash_number((ATerm) protoPlaceholder, TERM_SIZE_PLACEHOLDER);

  cur = hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
		 || ATgetPlaceholder((ATermPlaceholder)cur) != type)) {
    cur = cur->next;
  }

  if (!cur) {
    cur = AT_allocate(TERM_SIZE_PLACEHOLDER);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ((ATermPlaceholder)cur)->ph_type = type;
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  return (ATermPlaceholder) cur;

}

/*}}}  */

/*{{{  ATermBlob ATmakeBlob(void *data, int size) */

/**
 * Create a new BLOB (Binary Large OBject)
 */

ATermBlob ATmakeBlob(int size, void *data)
{
  ATermBlob protoBlob;
  ATerm cur;
  header_type header = BLOB_HEADER(0);
  HashNumber hnr;

/*
  if (low_memory) {
    AT_collect();
  }
*/

  protoBlob = (ATermBlob) protoTerm;
  protoBlob->header = header;
  CHECK_HEADER(protoBlob->header);
  protoBlob->size = size;
  protoBlob->data = data;

  hnr = hash_number((ATerm) protoBlob, TERM_SIZE_BLOB);

  cur = hashtable[hnr & table_mask];
  while (cur && (!EQUAL_HEADER(cur->header,header)
		 || ((ATermBlob)cur)->size != size
		 || ((ATermBlob)cur)->data != data)) {
    cur = cur->next;
  }

  if (!cur) {
    cur = AT_allocate(TERM_SIZE_BLOB);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    CHECK_HEADER(cur->header);
    ((ATermBlob)cur)->data = data;
    ((ATermBlob)cur)->size = size;
    cur->next = hashtable[hnr];
    hashtable[hnr] = cur;
  }

  /*ATwarning("ATmakeBlob(%d): hnr = %d, t = %t\n", size, (int)hnr, cur);*/

  return (ATermBlob)cur;
}

/*}}}  */

/*{{{  ATerm AT_setAnnotations(ATerm t, ATermList annos) */

/**
 * Change the annotations of a term.
 */

ATerm AT_setAnnotations(ATerm t, ATerm annos)
{
  HashNumber hnr;
  int i, size = term_size(t);
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

  /* Look through the hashtable for an identical term */
  while (cur && !found) {
    if (!EQUAL_HEADER(cur->header,header) || !ATisEqual(((ATerm *)cur)[size],annos)) {
      /* header or annos are different, must be another term */
      cur = cur->next;
    } else {
      ATbool rest_equal = ATtrue;

      /* check if other components are equal */
      for (i=2; i<size; i++) {
	if (!ATisEqual(((ATerm *)cur)[i], ((ATerm *)t)[i])) {
	  rest_equal = ATfalse;
	  break;
	}
      }

      if (rest_equal) {
	found = ATtrue;
      } else {
	cur = cur->next;
      }
    }
  }

  if (!found) {
    /* We need to create a new term */
    cur = AT_allocate(size+1);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    SET_AGE(cur->header,YOUNG_AGE);
    CHECK_HEADER(cur->header);
    
    cur->next   = hashtable[hnr];
    hashtable[hnr] = cur;

    for (i=2; i<size; i++) {
      ((ATerm *)cur)[i] = ((ATerm *)t)[i];
    }
    ((ATerm *)cur)[i] = annos;
  } else {

  }

  return cur;
}

/*}}}  */
/*{{{  ATerm AT_removeAnnotations(ATerm t) */

/**
 * Remove all annotations of a term.
 */

ATerm AT_removeAnnotations(ATerm t)
{
  HashNumber hnr;
  int i, size;
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

  /* Look through the hashtable for an identical term */
  while(cur && !found) {
    if(!EQUAL_HEADER(cur->header,header)) {
      /* header is different, must be another term */
      cur = cur->next;
    } else {
      ATbool rest_equal = ATtrue;

      /* check if other components are equal */
      for(i=2; i<size; i++) {
	if(((ATerm *)cur)[i] != ((ATerm *)t)[i]) {
	  rest_equal = ATfalse;
	  break;
	}
      }

      if (rest_equal) {
	found = ATtrue;
      } else {
	cur = cur->next;
      }
    }
  }

  if (!found) {
    /* We need to create a new term */
    cur = AT_allocate(size);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    SET_AGE(cur->header,YOUNG_AGE);
    CHECK_HEADER(cur->header);
    cur->next   = hashtable[hnr];
    hashtable[hnr] = cur;

    for (i=2; i<size; i++) {
      ((ATerm *)cur)[i] = ((ATerm *)t)[i];
    }
  }

  return cur;
}

/*}}}  */

/*{{{  ATermAppl ATsetArgument(ATermAppl appl, ATerm arg, int n) */

/**
 * Change one argument of an application.
 */

ATermAppl ATsetArgument(ATermAppl appl, ATerm arg, int n)
{
  int i, arity;
  Symbol sym = ATgetSymbol(appl);
  ATermAppl result;
  ATerm annos;

  arity = ATgetArity(sym);
  assert(n >= 0 && n < arity);

  for (i=0; i<arity; i++) {
    arg_buffer[i] = ATgetArgument(appl, i);
  }
  arg_buffer[n] = arg;

  result = ATmakeApplArray(sym, arg_buffer);
  annos = AT_getAnnotations((ATerm)appl);
  if (annos != NULL) {
    result = (ATermAppl)AT_setAnnotations((ATerm)result, annos);
  }

  return result;
}

/*}}}  */
/*{{{  ATermList ATmakeList(int n, ...) */

/**
 * Create a list with n arguments.
 */

ATermList ATmakeList(int n, ...)
{
  int i;
  va_list args;
  ATermList l;
  static ATerm *elems = 0;
  static int maxelems = 0;

  /* See if we have enough space to store the elements */
  if (n > maxelems) {
    if (!elems) {
      elems = (ATerm *)malloc(n*sizeof(ATerm));
    } else {
      elems = (ATerm *)realloc(elems, n*sizeof(ATerm));
    }
    if (!elems) {
      ATerror("ATmakeListn: cannot allocate space for %d terms.\n", n);
    }
    maxelems = n;
  }

  va_start(args, n);

  for (i=0; i<n; i++) {
    elems[i] = va_arg(args, ATerm);
  }

  l = ATempty;
  for (i=n-1; i>=0; i--) {
    l = ATinsert(l, elems[i]);
  }

  va_end(args);
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
  int i;

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

ATerm AT_getAnnotations(ATerm t)
{
  if(HAS_ANNO(t->header)) {
    int size = term_size(t);
    return ((ATerm *)t)[size-1];
  }
  return NULL;
}

/*}}}  */

/*{{{  ATerm ATsetAnnotation(ATerm t, ATerm label, ATerm anno) */

ATerm ATsetAnnotation(ATerm t, ATerm label, ATerm anno)
{
  ATerm newannos, oldannos = AT_getAnnotations(t);

  if(!oldannos)
    oldannos = ATdictCreate();

  newannos = ATdictPut(oldannos, label, anno);

  if(ATisEqual(oldannos, newannos))
    return t;
  return AT_setAnnotations(t, newannos);
}

/*}}}  */
/*{{{  ATerm ATgetAnnotation(ATerm t, ATerm label) */

/**
 * Retrieve an annotation with a specific label.
 */

ATerm ATgetAnnotation(ATerm t, ATerm label)
{
  ATerm annos = AT_getAnnotations(t);
  if(!annos)
    return NULL;

  return ATdictGet(annos, label);
}

/*}}}  */
/*{{{  ATerm ATremoveAnnotation(ATerm t, ATerm label) */

/**
 * Remove an annotation
 */

ATerm ATremoveAnnotation(ATerm t, ATerm label)
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
}

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
      if (offset >= 0	&& offset < (BLOCK_SIZE * sizeof(header_type))) {
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
        if (offset >= 0 && offset < (BLOCK_SIZE * sizeof(header_type))) {
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
  ATbool inblock = ATfalse;
  int idx = ADDR_TO_BLOCK_IDX(term);
  int type;
  
  assert(block_table[idx].first_after == block_table[(idx+1)%BLOCK_TABLE_SIZE].first_before);

    /* Warning: symboles*/

  for(cur=block_table[idx].first_after; cur; cur=cur->next_after) {
    header_type *end;
    if(cur->size) {
      assert(cur->next_before == cur->next_after);

      if(cur != at_blocks[cur->size]) {
        end = cur->end;
      } else {
        assert(at_blocks[cur->size] != NULL);
        end = top_at_blocks[cur->size];
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
        assert(cur->next_before == cur->next_after);
        
        if(cur != at_blocks[cur->size]) {
          end = cur->end;
        } else {
          assert(at_blocks[cur->size] != NULL);
          end = top_at_blocks[cur->size];
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

/*{{{  void AT_validateFreeList(int size) */

void AT_validateFreeList(int size)
{
  ATerm cur1, cur2;

  for(cur1=at_freelist[size]; cur1; cur1=cur1->next) {
    for(cur2=cur1->next; cur2; cur2=cur2->next)
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
  int i;

  for(i=MIN_TERM_SIZE; i<MAX_TERM_SIZE; i++) {
    ATerm cur = at_freelist[i];

    while(cur) {
      if(cur == t)
	return i;
      cur = cur->next;
    }
  }
  return 0;
}

/*}}}  */

/*{{{  void AT_printAllTerms(FILE *file) */

void AT_printAllTerms(FILE *file)
{
  int i;

  for(i=0; i<table_size; i++) {
    ATerm cur = hashtable[i];
    while(cur) {
      ATfprintf(file, "%t\n", cur);
        /*fprintf(file, "sym = %s\n",ATgetName(ATgetAFun(cur)));*/
      
      cur = cur->next;
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
  int i, nr_syms;
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
      cur = cur->next;
    }
  }

  afuns = (AFun *)calloc(nr_syms, sizeof(AFun));
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

int AT_calcAllocatedSize()
{
  int i;
  int total = 0;

  for(i=0; i<MAX_TERM_SIZE; i++)
    total += at_nrblocks[i]*sizeof(Block);

  total += table_size*sizeof(ATerm);

  return total;
}

/*}}}  */
