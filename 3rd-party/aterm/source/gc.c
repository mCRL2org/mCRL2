/*{{{  includes */


#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <setjmp.h>

#ifndef WIN32
#include <unistd.h>
#ifndef __MINGW32__
#include <sys/times.h>
#endif
#endif

#include "_aterm.h"
#include "afun.h"
#include "memory.h"
#include "util.h"
#include "gc.h"
#include "debug.h"

/*}}}  */

/*{{{  global variables */

char gc_id[] = "$Id: gc.c 23071 2007-07-02 10:06:17Z eriks $";

static ATerm *stackBot = NULL;

#define PRINT_GC_TIME           1
#define PRINT_GC_STATS          2

#define TO_OLD_RATIO   65
#define TO_YOUNG_RATIO 25

static int     flags               = 0;

int at_gc_count			   = 0;
static int     stack_depth[3]      = { 0, MYMAXINT, 0 };
static int     reclaim_perc[3]     = { 0, MYMAXINT, 0 };
extern int     mark_stats[3];
#ifdef WITH_STATS
static clock_t sweep_time[3]       = { 0, MYMAXINT, 0 };
static clock_t mark_time[3]        = { 0, MYMAXINT, 0 };
extern int     nr_marks;
#endif
static FILE *gc_f = NULL;

extern ATprotected_block protected_blocks;

AFun at_parked_symbol = -1;

int gc_min_number_of_blocks;
int max_freeblocklist_size;
int min_nb_minor_since_last_major;
int good_gc_ratio;
int small_allocation_rate_ratio;
int old_increase_rate_ratio;

ATbool at_mark_young;

/*}}}  */

#ifdef WIN32
#define VOIDCDECL void __cdecl
#else
#define VOIDCDECL void
#endif

/*{{{  local functions */

void major_sweep_phase_old();
void major_sweep_phase_young();
void minor_sweep_phase_young();
void check_unmarked_block(unsigned int blocks);

/*}}}  */

/*{{{  void AT_initGC(int argc, char *argv[], ATerm *bottomOfStack) */

void AT_initGC(int argc, char *argv[], ATerm *bottomOfStack)
{
  int i;

  stackBot = bottomOfStack;
  gc_f = stderr;
  
  for(i=1; i<argc; i++) {
    if(streq(argv[i], "-at-print-gc-time"))
      flags |= PRINT_GC_TIME;
    else if(streq(argv[i], "-at-print-gc-info"))
      flags |= (PRINT_GC_TIME | PRINT_GC_STATS);
    else if(strcmp(argv[i], "-at-help") == 0) {
      fprintf(stderr, "    %-20s: print non-intrusive gc information "
	      "after execution\n", "-at-print-gc-time");
      fprintf(stderr, "    %-20s: print elaborate gc information "
	      "after execution\n", "-at-print-gc-info");
    }
  }
}

/*}}}  */

/**
 * This function can be used to change the bottom of the stack.
 * Note that we only have one application that uses this fuction:
 * the Java ATerm wrapper interface, because here the garbage collector
 * can be called from different (but synchronized) threads, so at
 * the start of any operation that could start the garbage collector,
 * the bottomOfStack must be adjusted to point to the stack of
 * the calling thread.
 */
/*{{{  void AT_setBottomOfStack(ATerm *bottomOfStack) */

void AT_setBottomOfStack(ATerm *bottomOfStack)
{
  stackBot = bottomOfStack;
}

/*}}}  */
/*{{{  ATerm *stack_top() */

ATerm *stack_top()
{
  ATerm topOfStack;
  ATerm *top = &topOfStack;

  return top;
}

/*}}}  */

/*{{{  static void mark_memory(ATerm *start, ATerm *stop) */

static void mark_memory(ATerm *start, ATerm *stop,ATbool check_term) // CHANGED BY JFG
{
  ATerm *cur;
  // fprintf(stderr,"---> mark_memory phase [%x,%x]\n",start,stop);
  /* Traverse the stack */
  if (check_term)
  { ATerm real_term;
    for(cur=start; cur<stop; cur++) 
    { if(AT_isPotentialTerm(*cur)) 
      { real_term = AT_isInsideValidTerm(*cur);
        if (real_term != NULL) 
        { if(!IS_MARKED((real_term)->header)) 
          {
            assert(AT_isValidTerm(real_term));
            AT_markTerm(real_term);
              /*printf("mark_memory: cur = %x\ttop sym = %s\n",cur,ATgetName(ATgetAFun(real_term)));
 *   */
              /*nb_cell_in_stack++;*/
          }
        }
      } 
      else if (AT_isValidSymbol((Symbol)*cur)) 
      {
          /*fprintf(stderr,"mark_memory: AT_markSymbol(%d)\n",(Symbol)*cur);*/
        AT_markSymbol((Symbol)*cur);
          /*nb_cell_in_stack++;*/
      }
    }
  }
  else 
  { for(cur=start; cur<stop; cur++)
    { if ((*cur!=NULL) && (!IS_MARKED((*cur)->header)))
      { assert(AT_isValidTerm(*cur));
        AT_markTerm(*cur);
      }
    }
  }
}

/*}}}  */
/*{{{  static void mark_memory_young(ATerm *start, ATerm *stop)  */

static void mark_memory_young(ATerm *start, ATerm *stop,ATbool check_term) // CHANGED BY JFG
{
  ATerm *cur;
  // fprintf(stderr,"---> mark_memory_young phase [%x,%x]\n",start,stop);

  if (check_term)
  { ATerm real_term;
    for(cur=start; cur<stop; cur++) 
    { if(AT_isPotentialTerm(*cur)) 
      { real_term = AT_isInsideValidTerm(*cur);
        if (real_term != NULL) 
        { if(!IS_MARKED(real_term->header)) 
          { 
            assert(AT_isValidTerm(real_term));
            AT_markTerm_young(real_term);
                /*printf("mark_memory: cur = %x\ttop sym = %s\n",cur,ATgetName(ATgetAFun(real_term)));  */
                /*nb_cell_in_stack++;*/
          }
        }
      } 
      else if (AT_isValidSymbol((Symbol)*cur)) 
      {
            /*fprintf(stderr,"mark_memory_young: AT_markSymbol_young(%d)\n",(Symbol)*cur);*/
          AT_markSymbol_young((Symbol)*cur);
            /*nb_cell_in_stack++;*/
      }
    }
  }
  else 
  { 
    for(cur=start; cur<stop; cur++) 
    { if ((*cur!=NULL) && (!IS_MARKED((*cur)->header)))
      { assert(AT_isValidTerm(*cur));
        AT_markTerm_young(*cur);
      }
    }
  }
}

/*}}}  */

void ATmarkTerm(ATerm t)
{
  ATmarkArray(&t,1);
}

void ATmarkArray(ATerm *start, int size)
{
  if ( at_mark_young == ATtrue )
  {
	  mark_memory_young(start,start+size,ATfalse);
  } else {
	  mark_memory(start,start+size,ATfalse);
  }
}


/*{{{  VOIDCDECL mark_phase() */

VOIDCDECL mark_phase()
{
  unsigned int i,j;
  unsigned long stack_size;
  ATerm *stackTop;
  ATerm *start, *stop;
  ProtEntry *prot;
  ATprotected_block pblock;

#if defined(_MSC_VER) && defined(WIN32)

  unsigned int r_eax, r_ebx, r_ecx, r_edx, \
    r_esi, r_edi, r_esp, r_ebp;
  ATerm reg[8], real_term;

  __asm {
      /* Get the registers into local variables to check them
         for aterms later. */
    mov r_eax, eax
      mov r_ebx, ebx
      mov r_ecx, ecx
      mov r_edx, edx
      mov r_esi, esi
      mov r_edi, edi
      mov r_esp, esp
      mov r_ebp, ebp
      }
    /* Put the register-values into an array */
  reg[0] = (ATerm) r_eax;
  reg[1] = (ATerm) r_ebx;
  reg[2] = (ATerm) r_ecx;
  reg[3] = (ATerm) r_edx;
  reg[4] = (ATerm) r_esi;
  reg[5] = (ATerm) r_edi;
  reg[6] = (ATerm) r_esp;
  reg[7] = (ATerm) r_ebp;

  for(i=0; i<8; i++) {
    real_term = AT_isInsideValidTerm(reg[i]);
    if (real_term != NULL) {
      AT_markTerm(real_term);
    }
    if (AT_isValidSymbol((Symbol)reg[i])) {
      AT_markSymbol((Symbol)reg[i]);
    }
  }

    /* The register variables are on the stack aswell
       I set them to zero so they won't be processed again when
       the stack is traversed. The reg-array is also in the stack
       but that will be adjusted later */
  r_eax = 0;
  r_ebx = 0;
  r_ecx = 0;
  r_edx = 0;
  r_esi = 0;
  r_edi = 0;
  r_esp = 0;
  r_ebp = 0;

#else
  jmp_buf env;

  /* Traverse possible register variables */
  setjmp(env);

  start = (ATerm *)((char *)env);
  stop  = ((ATerm *)(((char *)env) + sizeof(jmp_buf)));
  mark_memory(start, stop,ATtrue);
#endif

  stackTop = stack_top();

  start = MIN(stackTop, stackBot);
  stop  = MAX(stackTop, stackBot);

  stack_size = stop-start;
  STATS(stack_depth, stack_size);

  // fprintf(stderr,"Mark memory 1\n");
  mark_memory(start, stop,ATtrue);

  /* Traverse protected terms */
  for(i=0; i<at_prot_table_size; i++) {
    ProtEntry *cur = at_prot_table[i];
    while(cur) {
      for(j=0; j<cur->size; j++) {
	if(cur->start[j])
	  AT_markTerm(cur->start[j]);
      }
      cur = cur->next;
    }
  }

  // fprintf(stderr,"Mark memory 2\n");
  for (prot=at_prot_memory; prot != NULL; prot=prot->next) {
    mark_memory((ATerm *)prot->start, (ATerm *)((prot->start) + prot->size),ATfalse);
  }
  
  // fprintf(stderr,"Mark memory 3\n");
  for (pblock=protected_blocks; pblock != NULL; pblock=pblock->next) {
    if (pblock->protsize>0)
      mark_memory(pblock->term, &pblock->term[pblock->protsize],ATfalse);
  }
  
  // fprintf(stderr,"Mark memory 4\n");
  at_mark_young = ATfalse;
  for (i=0; i<at_prot_functions_count; i++)
  {
    at_prot_functions[i]();
  }

  // fprintf(stderr,"Mark memory 5\n");
  AT_markProtectedSymbols();

  // fprintf(stderr,"Mark memory 6\n");
  /* Mark 'parked' symbol */
  if (AT_isValidSymbol(at_parked_symbol)) {
    AT_markSymbol(at_parked_symbol);
  }
}

/*}}}  */
/*{{{  VOIDCDECL mark_phase_young()  */

VOIDCDECL mark_phase_young() 
{
  unsigned int i,j;
  unsigned long stack_size;
  ATerm *stackTop;
  ATerm *start, *stop;
  ProtEntry *prot;
  ATprotected_block pblock;

  unsigned int count=0;
#if defined(_MSC_VER) && defined(WIN32)

  unsigned int r_eax, r_ebx, r_ecx, r_edx, \
    r_esi, r_edi, r_esp, r_ebp;
  ATerm reg[8], real_term;

  __asm {
      /* Get the registers into local variables to check them
         for aterms later. */
    mov r_eax, eax
      mov r_ebx, ebx
      mov r_ecx, ecx
      mov r_edx, edx
      mov r_esi, esi
      mov r_edi, edi
      mov r_esp, esp
      mov r_ebp, ebp
      }
    /* Put the register-values into an array */
  reg[0] = (ATerm) r_eax;
  reg[1] = (ATerm) r_ebx;
  reg[2] = (ATerm) r_ecx;
  reg[3] = (ATerm) r_edx;
  reg[4] = (ATerm) r_esi;
  reg[5] = (ATerm) r_edi;
  reg[6] = (ATerm) r_esp;
  reg[7] = (ATerm) r_ebp;

  for(i=0; i<8; i++) {
    real_term = AT_isInsideValidTerm(reg[i]);
    if (real_term != NULL) {
      AT_markTerm_young(real_term);
    }
    if (AT_isValidSymbol((Symbol)reg[i])) {
       AT_markSymbol_young((Symbol)reg[i]);
    }
  }

    /* The register variables are on the stack aswell
       I set them to zero so they won't be processed again when
       the stack is traversed. The reg-array is also in the stack
       but that will be adjusted later */
  r_eax = 0;
  r_ebx = 0;
  r_ecx = 0;
  r_edx = 0;
  r_esi = 0;
  r_edi = 0;
  r_esp = 0;
  r_ebp = 0;

#else
  jmp_buf env;

    /* Traverse possible register variables */
  setjmp(env);

  start = (ATerm *)((char *)env);
  stop  = ((ATerm *)(((char *)env) + sizeof(jmp_buf)));
  mark_memory_young(start, stop,ATtrue);
#endif

  stackTop = stack_top();
  start = MIN(stackTop, stackBot);
  stop  = MAX(stackTop, stackBot);

  stack_size = stop-start;
  STATS(stack_depth, stack_size);

  // fprintf(stderr,"Mark memory young 1\n");
  mark_memory_young(start, stop,ATtrue);

  // fprintf(stderr,"Mark memory young 2\n");
  /* Traverse protected terms */
  for(i=0; i<at_prot_table_size; i++) {
    ProtEntry *cur = at_prot_table[i];
    while(cur) {
      for(j=0; j<cur->size; j++) {
	if(cur->start[j])
	   AT_markTerm_young(cur->start[j]);
      }
      cur = cur->next;
    }
  }

  // fprintf(stderr,"Mark memory young 3\n");
  for (prot=at_prot_memory; prot != NULL; prot=prot->next) {
    mark_memory_young((ATerm *)prot->start, (ATerm *)((prot->start) + prot->size),ATfalse);
  }
  
  // fprintf(stderr,"Mark memory young 4\n");
  for (pblock=protected_blocks; pblock != NULL; pblock=pblock->next) {
  { if (pblock->protsize>0)
      mark_memory_young(pblock->term, &pblock->term[pblock->protsize], ATfalse);
      ++count;
  }

  }
  
  // fprintf(stderr,"Mark memory young 5 %d\n",count);
  at_mark_young = ATtrue;
  for (i=0; i<at_prot_functions_count; i++)
  {
    at_prot_functions[i]();
  }
    
  // fprintf(stderr,"Mark memory young 6\n");
  AT_markProtectedSymbols_young();
  // fprintf(stderr,"Mark memory young 7\n");

   /* Mark 'parked' symbol */
  if (AT_isValidSymbol(at_parked_symbol)) {
      /*fprintf(stderr,"mark_phase_young: AT_markSymbol_young(%d)\n",at_parked_symbol);*/
     AT_markSymbol_young(at_parked_symbol);
  }
}

/*}}}  */

#ifdef NDEBUG
#define CHECK_UNMARKED_BLOCK(blocks)
#else
#define CHECK_UNMARKED_BLOCK(blocks) check_unmarked_block(blocks)
#endif

/*{{{  void sweep_phase()  */

void sweep_phase() 
{
  unsigned int size;

  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    terminfo[size].at_freelist = NULL;
  }
  old_bytes_in_young_blocks_after_last_major = 0;
  old_bytes_in_old_blocks_after_last_major = 0;

  /* Warning: freelist[size] is empty*/
  /* Warning: do not sweep fresh promoted block*/
  major_sweep_phase_old();
  major_sweep_phase_young();
  CHECK_UNMARKED_BLOCK(AT_BLOCK);
  CHECK_UNMARKED_BLOCK(AT_OLD_BLOCK);
}

/*}}}  */
/*{{{  void AT_init_gc_parameters(ATbool low_memory)  */

void AT_init_gc_parameters(ATbool low_memory) 
{
  if(low_memory) {
    gc_min_number_of_blocks = 2;
    max_freeblocklist_size  = 30;
    min_nb_minor_since_last_major = 2;

    good_gc_ratio = 50;
    small_allocation_rate_ratio = 25;
    old_increase_rate_ratio = 50;

  } else {
      /* 20MB for 10 sizes in average*/
    gc_min_number_of_blocks = 2*(20*1024*1024)/(10*sizeof(Block));
    max_freeblocklist_size  = 100;
    min_nb_minor_since_last_major = 10;
    good_gc_ratio = 50;
    small_allocation_rate_ratio = 75;
    old_increase_rate_ratio = 50;

#ifdef GC_VERBOSE
    fprintf(stderr,"gc_min_number_of_blocks = %d\n",gc_min_number_of_blocks);
#endif
  }
}

/*}}}  */

/*{{{  static void reclaim_empty_block(Block **blocks, int size, Block *removed_block, Block *prev_block)  */

static void reclaim_empty_block(unsigned int blocks, int size, Block *removed_block, Block *prev_block) 
{ 
  TermInfo* ti = &terminfo[size];

  ti->nb_reclaimed_blocks_during_last_gc++;

    /*
     * Step 1:
     *
     * remove cells from terminfo[size].at_freelist
     * remove the block from terminfo[size].at_blocks[AT_BLOCK]
     *
     */
    
#ifdef GC_VERBOSE
  fprintf(stdout,"block %x is empty\n",(unsigned int)removed_block);
#endif
  ti->at_nrblocks--;
  removed_block->size = 0;
  if(prev_block == NULL) {
      /*fprintf(stderr,"restore_block: remove first\n");*/
    ti->at_blocks[blocks] = removed_block->next_by_size;
    if(blocks==AT_BLOCK && ti->at_blocks[AT_BLOCK]) {
      ti->top_at_blocks = ti->at_blocks[AT_BLOCK]->end;
    }
  } else {
      /*fprintf(stderr,"restore_block: remove middle\n");*/
    prev_block->next_by_size = removed_block->next_by_size;
  }

  /*
   * Step 2:
   *
   * put the block into at_freeblocklist
   *
   */
  removed_block->next_by_size = at_freeblocklist;
  at_freeblocklist = removed_block;
  at_freeblocklist_size++;
        
  /*
   * Step 3:
   *
   * remove the block from block_table
   * free the memory
   *
   */
  if(at_freeblocklist_size > max_freeblocklist_size) {
    int idx, next_idx;
    Block *cur;
    Block *prev = NULL;
          
    assert(removed_block != NULL);
          
    idx = ADDR_TO_BLOCK_IDX(removed_block);
    next_idx = (idx+1)%BLOCK_TABLE_SIZE;
    for(cur=block_table[idx].first_after; cur ; prev=cur, cur=cur->next_after) {
      if(removed_block == cur) {
        break;
      }
    }
    if(!cur) {
      ATabort("### block %d not found\n",removed_block);
    }

    if(prev==NULL) {
      block_table[idx].first_after       = removed_block->next_after;
      block_table[next_idx].first_before = removed_block->next_after;
    } else {
      prev->next_after  = removed_block->next_after;
      prev->next_before = removed_block->next_before;
    }
          
    at_freeblocklist_size--;
    at_freeblocklist = at_freeblocklist->next_by_size;
#ifdef GC_VERBOSE
    fprintf(stderr,"free block %d\n",(int)removed_block);
#endif
    AT_free(removed_block);
  }
}

/*}}}  */
/*{{{  static void promote_block_to_old(int size, Block *block, Block *prev_block)  */

static void promote_block_to_old(int size, Block *block, Block *prev_block) 
{
  TermInfo* ti = &terminfo[size];

#ifdef GC_VERBOSE
  printf("move block %x to old_blocks\n",(unsigned int)block);
#endif
  assert(block!=NULL);
  if(prev_block == NULL) {
    ti->at_blocks[AT_BLOCK] = block->next_by_size;
    if(ti->at_blocks[AT_BLOCK]) {
      ti->top_at_blocks = ti->at_blocks[AT_BLOCK]->end;
    }
   
  } else {
    prev_block->next_by_size = block->next_by_size;
  }
  block->next_by_size = ti->at_blocks[AT_OLD_BLOCK];
  ti->at_blocks[AT_OLD_BLOCK] = block;
}

/*}}}  */
/*{{{  static void promote_block_to_young(int size, Block *block, Block *prev_block)  */

static void promote_block_to_young(int size, Block *block, Block *prev_block) 
{
  TermInfo* ti = &terminfo[size];
	
#ifdef GC_VERBOSE
  printf("move block %x to young_blocks\n",(unsigned int)block);
#endif
  assert(block!=NULL);
  if(prev_block == NULL) {
    ti->at_blocks[AT_OLD_BLOCK] = block->next_by_size;
  } else {
    prev_block->next_by_size = block->next_by_size;
  }
  if(ti->at_blocks[AT_BLOCK]) {
    block->next_by_size = ti->at_blocks[AT_BLOCK]->next_by_size;
    ti->at_blocks[AT_BLOCK]->next_by_size = block;
  } else {
    block->next_by_size = NULL;
    ti->at_blocks[AT_BLOCK] = block;
    ti->top_at_blocks = block->end;
    assert(ti->at_blocks[AT_BLOCK] != NULL);
  }
}

/*}}}  */

/*{{{  void check_unmarked_block(Block **blocks)  */

void check_unmarked_block(unsigned int blocks) 
{
  unsigned int size;
  
  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    Block *block = terminfo[size].at_blocks[blocks];
    header_type *end = NULL;

    if(blocks == AT_BLOCK) {
      end = terminfo[size].top_at_blocks;
    } else {
      if(block) {
        end = block->end;
      } 
    }
    
    while(block) {
      header_type *cur;
      for(cur=block->data ; cur<end ; cur+=size) {
	ATerm t = (ATerm)cur;

        if(IS_MARKED(t->header)) {
#ifdef GC_VERBOSE
          fprintf(stderr,"block = %p\tdata = %p\tblock->end = %p\tend = %p\n",block,block->data,block->end,end);
          fprintf(stderr,"type = %d\n",GET_TYPE(t->header));
          fprintf(stderr,"t = %p\n",t);
#endif
        }

        if(blocks==AT_OLD_BLOCK) {
          assert(GET_TYPE(t->header)==AT_FREE || IS_OLD(t->header));
        }
        
        assert(!IS_MARKED(t->header));
      }
      block = block->next_by_size;
      if(block) {
        end = block->end;
      }
    }
  }
}

/*}}}  */

/*{{{  void major_sweep_phase_old()  */

void major_sweep_phase_old() 
{
  unsigned int size;
  int reclaiming = 0;
  int alive = 0;
#ifdef WITH_STATS
  int perc;
#endif

  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    Block *prev_block = NULL;
    Block *next_block;

    Block *block = terminfo[size].at_blocks[AT_OLD_BLOCK];

    while(block) {
      /* set empty = 0 to avoid recycling*/
      int empty = 1;
      int alive_in_block = 0;
      int dead_in_block  = 0;
      int free_in_block  = 0;
      int capacity = ((block->end)-(block->data))/size;
      header_type *cur;

      assert(block->size == size);

      for(cur=block->data ; cur<block->end ; cur+=size) {
          /* TODO: Optimisation*/
	ATerm t = (ATerm)cur;
	if(IS_MARKED(t->header)) {
	  CLR_MARK(t->header);
          alive_in_block++;
          empty = 0;
          assert(IS_OLD(t->header));
	} else {
	  switch(ATgetType(t)) {
              case AT_FREE:
                assert(IS_YOUNG(t->header));
                free_in_block++;
                break;
              case AT_INT:
              case AT_REAL:
              case AT_APPL:
              case AT_LIST:
              case AT_PLACEHOLDER:
              case AT_BLOB:
                assert(IS_OLD(t->header));
                AT_freeTerm(size, t);
                t->header=FREE_HEADER;
                dead_in_block++;
                break;
              case AT_SYMBOL:
                assert(IS_OLD(t->header));
                AT_freeSymbol((SymEntry)t);
                t->header=FREE_HEADER;
                dead_in_block++;
                break;
              default:
                ATabort("panic in sweep phase\n");
	  }
	}
      }
      assert(alive_in_block + dead_in_block + free_in_block == capacity);
      
      next_block = block->next_by_size;
      
#ifndef NDEBUG
      if(empty) {
        for(cur=block->data; cur<block->end; cur+=size) {
          assert(ATgetType((ATerm)cur) == AT_FREE);
        }
      }
#endif
      
      if(empty) {
          /* DO NOT RESTORE THE FREE LIST: free cells have not been inserted*/
          /* terminfo[size].at_freelist = old_freelist;*/
        assert(terminfo[size].top_at_blocks < block->data || terminfo[size].top_at_blocks > block->end);
#ifdef GC_VERBOSE
        fprintf(stderr,"MAJOR OLD: reclaim empty block %p\n",block);
#endif
        reclaim_empty_block(AT_OLD_BLOCK, size, block, prev_block);
      } else if(0 && 100*alive_in_block/capacity <= TO_YOUNG_RATIO) {
        promote_block_to_young(size, block, prev_block);
        old_bytes_in_young_blocks_after_last_major += (alive_in_block*SIZE_TO_BYTES(size));
      } else {
        old_bytes_in_old_blocks_after_last_major += (alive_in_block*SIZE_TO_BYTES(size));
        
        /* DO NOT FORGET THIS LINE*/
        /* update the previous block*/
        prev_block = block;
      }

      block = next_block;
      alive += alive_in_block;
      reclaiming += dead_in_block;
    }
  }
#ifdef WITH_STATS
  if(alive) {
    perc = (100*reclaiming)/alive;
    STATS(reclaim_perc, perc);
  }
#endif
}

/*}}}  */
/*{{{  void major_sweep_phase_young()  */

void major_sweep_phase_young() 
{
  int reclaiming = 0;
  int alive = 0;
  unsigned int size;
#ifdef WITH_STATS
  int perc;
#endif

  old_bytes_in_young_blocks_since_last_major = 0;
  
  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    Block *prev_block = NULL;
    Block *next_block;
    ATerm old_freelist;
    TermInfo* ti = &terminfo[size];

    Block *block      = ti->at_blocks[AT_BLOCK];
    header_type *end  = ti->top_at_blocks;

    while(block) {
      int empty = 1;
      int alive_in_block = 0;
      int dead_in_block  = 0;
      int free_in_block  = 0;
      int old_in_block   = 0;
      int young_in_block = 0;
      int capacity = (end-(block->data))/size;
      header_type *cur;
      
      assert(block->size == size);

      old_freelist = ti->at_freelist;
      for(cur=block->data ; cur<end ; cur+=size) {
	ATerm t = (ATerm)cur;
	if(IS_MARKED(t->header)) {
	  CLR_MARK(t->header);
          alive_in_block++;
          empty = 0;
          if(IS_OLD(t->header)) {
            old_in_block++;
          } else {
            young_in_block++;
            INCREMENT_AGE(t->header);
          }
	} else {
	  switch(ATgetType(t)) {
              case AT_FREE:
                t->aterm.next = ti->at_freelist;
                ti->at_freelist = t;
                free_in_block++;
                break;
              case AT_INT:
              case AT_REAL:
              case AT_APPL:
              case AT_LIST:
              case AT_PLACEHOLDER:
              case AT_BLOB:
                AT_freeTerm(size, t);
                t->header = FREE_HEADER;
                t->aterm.next  = ti->at_freelist;
                ti->at_freelist = t;
                dead_in_block++;
                break;
              case AT_SYMBOL:
                AT_freeSymbol((SymEntry)t);
                t->header = FREE_HEADER;
                t->aterm.next = ti->at_freelist;
                ti->at_freelist = t;
                
                dead_in_block++;
                break;
              default:
                ATabort("panic in sweep phase\n");
	  }
	}
      }
      assert(alive_in_block + dead_in_block + free_in_block == capacity);
      
      next_block = block->next_by_size;

#ifndef NDEBUG
      if(empty) {
        for(cur=block->data; cur<end; cur+=size) {
          assert(ATgetType((ATerm)cur) == AT_FREE);
        }
      }
#endif

#ifdef GC_VERBOSE
        /*fprintf(stderr,"old_cell_in_young_block ratio = %d\n",100*old_in_block/capacity);*/
#endif
       
      if(end==block->end && empty) {
#ifdef GC_VERBOSE
        fprintf(stderr,"MAJOR YOUNG: reclaim empty block %p\n",block);
#endif
        ti->at_freelist = old_freelist;
	reclaim_empty_block(AT_BLOCK, size, block, prev_block);
      } else if(end==block->end && 100*old_in_block/capacity >= TO_OLD_RATIO) {
        if(young_in_block == 0) {
#ifdef GC_VERBOSE
          fprintf(stderr,"MAJOR YOUNG: promote block %p to old\n",block);
#endif
          ti->at_freelist = old_freelist;
          promote_block_to_old(size, block, prev_block);
          old_bytes_in_old_blocks_after_last_major += (old_in_block*SIZE_TO_BYTES(size));
        } else {
#ifdef GC_VERBOSE
          fprintf(stderr,"MAJOR YOUNG: freeze block %p\n",block);
#endif
          SET_FROZEN(block);
          old_bytes_in_young_blocks_after_last_major += (old_in_block*SIZE_TO_BYTES(size));
          ti->at_freelist = old_freelist;
          prev_block = block;
        }
      } else {
        old_bytes_in_young_blocks_after_last_major += (old_in_block*SIZE_TO_BYTES(size));
        prev_block = block;
      }

      block = next_block;
      if(block) {
        end = block->end;
      }

      alive += alive_in_block;
      reclaiming += dead_in_block;
    }

#ifndef NDEBUG
    if(ti->at_freelist) {
      ATerm data;
      for(data = ti->at_freelist ; data ; data=data->aterm.next) {
        assert(EQUAL_HEADER(data->header,FREE_HEADER)); 
        assert(ATgetType(data) == AT_FREE);   
      } 
    }
#endif
    
  }
#ifdef WITH_STATS
  if(alive) {
    perc = (100*reclaiming)/alive;
    STATS(reclaim_perc, perc);
  }
#endif
}

/*}}}  */
/*{{{  void minor_sweep_phase_young()  */

void minor_sweep_phase_young() 
{
  unsigned int size;
  int reclaiming = 0;
  int alive = 0;
#ifdef WITH_STATS
  int perc;
#endif

  old_bytes_in_young_blocks_since_last_major = 0;
  
  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    Block *prev_block = NULL;
    Block *next_block;
    ATerm old_freelist;
    TermInfo* ti = &terminfo[size];

    Block *block = ti->at_blocks[AT_BLOCK];
    header_type *end = ti->top_at_blocks;

      /* empty the freelist*/
    ti->at_freelist = NULL;
        
    while(block) {
        /* set empty = 0 to avoid recycling*/
      int empty = 1;
      int alive_in_block = 0;
      int dead_in_block  = 0;
      int free_in_block  = 0;
      int old_in_block  = 0;
      int capacity = (end-(block->data))/size;
      header_type *cur;
      
      assert(block->size == size);
      
      old_freelist = ti->at_freelist;
      for(cur=block->data ; cur<end ; cur+=size) {
	ATerm t = (ATerm)cur;
	if(IS_MARKED(t->header) || IS_OLD(t->header)) {
          if(IS_OLD(t->header)) {
            old_in_block++;
          }else{
          	INCREMENT_AGE(t->header);
          }
          CLR_MARK(t->header);
          alive_in_block++;
          empty = 0;
          assert(!IS_MARKED(t->header));
	} else {
	  switch(ATgetType(t)) {
              case AT_FREE:
                /* ti->at_freelist is not empty: so DO NOT ADD t*/
                t->aterm.next = ti->at_freelist;
                ti->at_freelist = t;
                free_in_block++;
                break;
              case AT_INT:
              case AT_REAL:
              case AT_APPL:
              case AT_LIST:
              case AT_PLACEHOLDER:
              case AT_BLOB:
                AT_freeTerm(size, t);
                t->header = FREE_HEADER;
                t->aterm.next   = ti->at_freelist;
                ti->at_freelist = t;
                
                dead_in_block++;
                break;
              case AT_SYMBOL:
                AT_freeSymbol((SymEntry)t);
                t->header = FREE_HEADER;
                t->aterm.next   = ti->at_freelist;
                ti->at_freelist = t;
                dead_in_block++;
                break;

              default:
                ATabort("panic in sweep phase\n");
	  }
          assert(!IS_MARKED(t->header));
	}
      }

      assert(alive_in_block + dead_in_block + free_in_block == capacity);
      next_block    = block->next_by_size;

#ifndef NDEBUG
      if(empty) {
        for(cur=block->data; cur<end; cur+=size) {
          assert(ATgetType((ATerm)cur) == AT_FREE);
        }
      }
#endif

      /* Do not reclaim frozen blocks */
      if(IS_FROZEN(block)) {
        ti->at_freelist = old_freelist;
      }
      
       /* TODO: create freeList Old*/
      if(0 && empty) {
        ti->at_freelist = old_freelist;
        reclaim_empty_block(AT_BLOCK, size, block, prev_block);
      } else if(0 && 100*old_in_block/capacity >= TO_OLD_RATIO) {
        promote_block_to_old(size, block, prev_block);
      } else {
        old_bytes_in_young_blocks_since_last_major += (old_in_block*SIZE_TO_BYTES(size));
        prev_block = block;
      }

      block = next_block;
      if(block) {
        end = block->end;
      }
      alive += alive_in_block;
      reclaiming += dead_in_block;
    }

#ifndef NDEBUG
    if(ti->at_freelist) {
      ATerm data;
      /*fprintf(stderr,"minor_sweep_phase_young: ensure empty freelist[%d]\n",size);*/
      for(data = ti->at_freelist ; data ; data=data->aterm.next) {
        if(!EQUAL_HEADER(data->header,FREE_HEADER)) {
          fprintf(stderr,"data = %p header = %x\n",data,(unsigned int) data->header);
        }
        assert(EQUAL_HEADER(data->header,FREE_HEADER)); 
        assert(ATgetType(data) == AT_FREE);   
      }
    }
#endif
    
  }
#ifdef WITH_STATS
  if(alive) {
    perc = (100*reclaiming)/alive;
    STATS(reclaim_perc, perc);
  }
#endif
}

/*}}}  */

/* The timing/STATS parts haven't been tested yet (on NT)
 * but without the info things seem to work fine 
 */
#ifdef WIN32
/*{{{  void AT_collect() */

void AT_collect()
{
#ifdef WITH_STATS
  clock_t start, mark, sweep;
  clock_t user;
#endif

  FILE *file = gc_f;
  int size;

      /* snapshop*/
  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    TermInfo* ti = &terminfo[size];
    ti->nb_live_blocks_before_last_gc = ti->at_nrblocks;
    ti->nb_reclaimed_blocks_during_last_gc=0;
    ti->nb_reclaimed_cells_during_last_gc=0;
  }

  at_gc_count++;
  if (!silent)
  {
    fprintf(file, "collecting garbage..(%d)",at_gc_count);
    fflush(file);
  }

#ifdef WITH_STATS
  start = clock();
#endif

  mark_phase();
#ifdef WITH_STATS
  mark = clock();
  user = mark - start;
  STATS(mark_time, user);
#endif

  sweep_phase();

#ifdef WITH_STATS
  sweep = clock();
  user = sweep - mark;
  STATS(sweep_time, user);
#endif

  if (!silent)
    fprintf(file, "..\n");
}

/*}}}  */
/*{{{  void AT_collect_minor() */

void AT_collect_minor()
{
#ifdef WITH_STATS
  clock_t start, mark, sweep;
  clock_t user;
#endif
  FILE *file = gc_f;
  int size;

      /* snapshop*/
  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    TermInfo* ti = &terminfo[size];
    ti->nb_live_blocks_before_last_gc = ti->at_nrblocks;
    ti->nb_reclaimed_blocks_during_last_gc=0;
    ti->nb_reclaimed_cells_during_last_gc=0;
  }

  at_gc_count++;
  if (!silent)
  {
    fprintf(file, "young collecting garbage..(%d)yy",at_gc_count);
    fflush(file);
  }

#ifdef WITH_STATS
  start = clock();
#endif

  /* was minor_mark_phase_young(); this should be verified! */
  mark_phase_young();

#ifdef WITH_STATS
  mark = clock();
  user = mark - start;
  STATS(mark_time, user);
#endif

  minor_sweep_phase_young();

#ifdef WITH_STATS
  sweep = clock();
  user = sweep - mark;
  STATS(sweep_time, user);
#endif

  if (!silent)
    fprintf(file, "..\n");
}

/*}}}  */
#else
/*{{{  void AT_collect() */

void AT_collect()
{
#ifdef WITH_STATS
  struct tms start, mark, sweep;
  clock_t user;
#endif
  FILE *file = gc_f;
  unsigned int size;
  // fprintf(stderr,"begin AT collect phase\n");

  /* snapshot*/
  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    TermInfo* ti = &terminfo[size];
    ti->nb_live_blocks_before_last_gc = ti->at_nrblocks;
    ti->nb_reclaimed_blocks_during_last_gc=0;
    ti->nb_reclaimed_cells_during_last_gc=0;
  }
  
  at_gc_count++;
  if (!silent)
  {
    fprintf(file, "collecting garbage..(%d)",at_gc_count);
    fflush(file);
  }

#ifdef WITH_STATS
  times(&start);
#endif

  CHECK_UNMARKED_BLOCK(AT_BLOCK);
  CHECK_UNMARKED_BLOCK(AT_OLD_BLOCK);
  // fprintf(stderr,"begin mark phase\n");
  mark_phase();
  // fprintf(stderr,"end mark phase\n");
  
#ifdef WITH_STATS
  times(&mark);
  user = mark.tms_utime - start.tms_utime;
  STATS(mark_time, user);
#endif

  sweep_phase();
  // fprintf(stderr,"end sweep phase\n");

#ifdef WITH_STATS  
  times(&sweep);
  user = sweep.tms_utime - mark.tms_utime;
  STATS(sweep_time, user);
#endif

  if (!silent) {
    fprintf(file, "..\n");
  }
}

/*}}}  */
/*{{{  void AT_collect_minor() */

void AT_collect_minor()
{
  // fprintf(stderr,"begin AT collect phase\n");
#ifdef WITH_STATS
  struct tms start, mark, sweep;
  clock_t user;
#endif
  FILE *file = gc_f;
  unsigned int size;
  
    /* snapshop*/
  for(size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++) {
    TermInfo* ti = &terminfo[size];
    ti->nb_live_blocks_before_last_gc = ti->at_nrblocks;
    ti->nb_reclaimed_blocks_during_last_gc=0;
    ti->nb_reclaimed_cells_during_last_gc=0;
  }
  
  at_gc_count++;
  if (!silent)
  {
    fprintf(file, "young collecting garbage..(%d)",at_gc_count);
    fflush(file);
  }

#ifdef WITH_STATS
  times(&start);
#endif

  CHECK_UNMARKED_BLOCK(AT_BLOCK);
  CHECK_UNMARKED_BLOCK(AT_OLD_BLOCK);
    /*nb_cell_in_stack=0;*/
  // fprintf(stderr,"begin mark young phase\n");
  mark_phase_young();
    /*fprintf(stderr,"AT_collect_young: nb_cell_in_stack = %d\n",nb_cell_in_stack++);*/
  // fprintf(stderr,"end mark young phase\n");
    
#ifdef WITH_STATS
  times(&mark);
  user = mark.tms_utime - start.tms_utime;
  STATS(mark_time, user);
#endif

  minor_sweep_phase_young();
  CHECK_UNMARKED_BLOCK(AT_BLOCK);
  CHECK_UNMARKED_BLOCK(AT_OLD_BLOCK);
  // fprintf(stderr,"end sweep young phase\n");

#ifdef WITH_STATS
  times(&sweep);
  user = sweep.tms_utime - mark.tms_utime;
  STATS(sweep_time, user);
#endif

  if (!silent)
    fprintf(file, "..\n");
}

/*}}}  */
#endif

#define CLOCK_DIVISOR CLOCKS_PER_SEC

/*{{{  void AT_cleanupGC() */

void AT_cleanupGC()
{
  FILE *file = gc_f;
  if(flags & PRINT_GC_TIME) {
    fprintf(file, "%d garbage collects,\n", at_gc_count);
#ifdef WITH_STATS
    fprintf(file, "(all statistics are printed min/avg/max)\n");
    if(at_gc_count > 0) {
      if(nr_marks > 0) {
	fprintf(file, "  mark stack needed: %d/%d/%d (%d marks)\n", 
		mark_stats[IDX_MIN],
                mark_stats[IDX_TOTAL]/nr_marks, 
		mark_stats[IDX_MAX], nr_marks);
      }
      fprintf(file, "  marking  took %.2f/%.2f/%.2f seconds, total: %.2f\n", 
	      ((double)mark_time[IDX_MIN])/(double)CLOCK_DIVISOR,
	      (((double)mark_time[IDX_TOTAL])/(double)at_gc_count)/(double)CLOCK_DIVISOR,
	      ((double)mark_time[IDX_MAX])/(double)CLOCK_DIVISOR,
	      ((double)mark_time[IDX_TOTAL])/(double)CLOCK_DIVISOR);
      fprintf(file, "  sweeping took %.2f/%.2f/%.2f seconds, total: %.2f\n", 
	      ((double)sweep_time[IDX_MIN])/(double)CLOCK_DIVISOR,
	      (((double)sweep_time[IDX_TOTAL])/(double)at_gc_count)/(double)CLOCK_DIVISOR,
	      ((double)sweep_time[IDX_MAX])/(double)CLOCK_DIVISOR,
	      ((double)sweep_time[IDX_TOTAL])/(double)CLOCK_DIVISOR);
    }
#ifdef WIN32
    fprintf(file, "Note: WinNT times are absolute, and might be influenced by other processes.\n");
#endif
#endif
  }
  
  if(flags & PRINT_GC_STATS) {
    if(at_gc_count > 0) {
      fprintf(file, "\n  stack depth: %d/%d/%d words\n", 
	      stack_depth[IDX_MIN],  
	      stack_depth[IDX_TOTAL]/at_gc_count,
	      stack_depth[IDX_MAX]);
      fprintf(file, "\n  reclamation percentage: %d/%d/%d\n",
	      reclaim_perc[IDX_MIN],
	      reclaim_perc[IDX_TOTAL]/at_gc_count,
	      reclaim_perc[IDX_MAX]);
    }
  }
}

/*}}}  */

