/*{{{  includes */

#include <cstdlib>
#include <ctime>
#include <climits>
#include <cassert>
#include <csetjmp>
#include <stdexcept>

#ifndef WIN32
#include <unistd.h>
#ifndef __MINGW32__
#include <sys/times.h>
#endif
#endif

#include "mcrl2/utilities/logger.h"
#include "mcrl2/aterm/_aterm.h"
#include "mcrl2/aterm/afun.h"
#include "mcrl2/aterm/memory.h"
#include "mcrl2/aterm/util.h"
#include "mcrl2/aterm/gc.h"

/*}}}  */

namespace aterm
{

/*{{{  global variables */

char gc_id[] = "$Id$";

static ATerm* stackBot = NULL;


static const size_t TO_OLD_RATIO = 65;
static const size_t TO_YOUNG_RATIO = 25;

extern ATprotected_block protected_blocks;

AFun at_parked_symbol = (size_t)-1; // Init

size_t gc_min_number_of_blocks;
size_t max_freeblocklist_size;
size_t min_nb_minor_since_last_major;
size_t good_gc_ratio;
size_t small_allocation_rate_ratio;
size_t old_increase_rate_ratio;
static bool at_mark_young = false;

/*}}}  */

#ifdef WIN32
#define VOIDCDECL static void __cdecl
#else
#define VOIDCDECL static void
#endif

/*{{{  local functions */

void major_sweep_phase_old();
void major_sweep_phase_young();
void minor_sweep_phase_young();
static void check_unmarked_block(size_t blocks);

/*}}}  */

/*{{{  void AT_initGC(int argc, char *argv[], ATerm *bottomOfStack) */

void AT_initGC(int argc, char* argv[], ATerm* bottomOfStack)
{
  /* Suppress unused variable warning */
  (void)argc;
  (void)argv;

  stackBot = bottomOfStack;
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

void AT_setBottomOfStack(ATerm* bottomOfStack)
{
  stackBot = bottomOfStack;
}

/*}}}  */
/*{{{  ATerm *stack_top() */

static
ATerm* stack_top()
{
  ATerm topOfStack;
  ATerm* top = &topOfStack;

  return top;
}

/*}}}  */

/*{{{  static void mark_memory(ATerm *start, ATerm *stop) */

static void mark_memory(const ATerm* start, const ATerm* stop, const bool check_term, const bool only_mark_young) /* CHANGED BY JFG */
{
  const ATerm* cur;
  /* Traverse the stack */
  if (check_term)
  {
    ATerm real_term;
    for (cur=start; cur<stop; cur++)
    {
      if (AT_isPotentialTerm(*cur))
      {
        real_term = AT_isInsideValidTerm(*cur);
        if (real_term != NULL)
        {
          if (!IS_MARKED((real_term)->header))
          {
            assert(AT_isValidTerm(real_term));
            AT_markTerm(real_term,only_mark_young);
          }
        }
      }
      else if (AT_isValidAFun((AFun)*cur))
      {
        AT_markAFun((AFun)*cur,only_mark_young);
      }
    }
  }
  else
  {
    for (cur=start; cur<stop; cur++)
    {
      if ((*cur!=NULL) && (!IS_MARKED((*cur)->header)))
      {
        assert(AT_isValidTerm(*cur));
        AT_markTerm(*cur,only_mark_young);
      }
    }
  }
}

/*}}}  */

void ATmarkTerm(const ATerm t)
{
  const ATerm* start=&t;
  mark_memory(start,start+1,false,at_mark_young); // Note that at_mark_young is a global variable.
}

/*{{{  VOIDCDECL mark_phase() */

VOIDCDECL mark_phase(const bool only_mark_young)
{
  size_t i,j;
  ATerm* stackTop;
  ATerm* start, *stop;
  ProtEntry* prot;
  ATprotected_block pblock;


#if defined(_MSC_VER) && defined(_M_X64)
  /* Time for a little explanation here: The aim of the following three lines is to read
     relevant registers from the CPU, even if we can't execute inline assembly (as is
   done for _MSC_VER && WIN32). The setjmp routine saves the call environment to a
   buffer, yielding a nicely packaged list of registers. In the AMD64 case, only the
   first 12 registers are integer registers, which is the only type of register we
   expect to find ATerms in. */
  jmp_buf env; /* Buffer for registers */
  setjmp(env); /* Save registers to buffer */
  /* Now check buffer for ATerms and mark them */
  mark_memory((ATerm*)((char*)env), (ATerm*)((char*)env) + 12, true,only_mark_young);
#elif defined(_MSC_VER) && defined(WIN32)
  size_t r_eax, r_ebx, r_ecx, r_edx, \
  r_esi, r_edi, r_esp, r_ebp;
  ATerm reg[8], real_term;

  __asm
  {
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

  for (i=0; i<8; i++)
  {
    real_term = AT_isInsideValidTerm(reg[i]);
    if (real_term != NULL)
    {
      assert(AT_isValidTerm(real_term));
      AT_markTerm(real_term,only_mark_young);
    }
    if (AT_isValidAFun((AFun)reg[i]))
    {
      AT_markAFun((AFun)reg[i],only_mark_young);
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

  start = (ATerm*)((char*)env);
  stop  = ((ATerm*)(((char*)env) + sizeof(jmp_buf)));
  mark_memory(start, stop,true,only_mark_young);
#endif

  stackTop = stack_top();

  start = MIN(stackTop, stackBot);
  stop  = MAX(stackTop, stackBot);

  mark_memory(start, stop,true,only_mark_young);

  /* Traverse protected terms */
  for (i=0; i<at_prot_table_size; i++)
  {
    ProtEntry* cur = at_prot_table[i];
    while (cur)
    {
      for (j=0; j<cur->size; j++)
      {
        if (cur->start[j])
        {
          assert(AT_isValidTerm(cur->start[j]));
          AT_markTerm(cur->start[j],only_mark_young);
        }
      }
      cur = cur->next;
    }
  }

  for (prot=at_prot_memory; prot != NULL; prot=prot->next)
  {
    mark_memory((ATerm*)prot->start, (ATerm*)((prot->start) + prot->size),false,only_mark_young);
  }

  for (pblock=protected_blocks; pblock != NULL; pblock=pblock->next)
  {
    if (pblock->protsize>0)
    {
      mark_memory(pblock->term, &pblock->term[pblock->protsize],false,only_mark_young);
    }
  }

  at_mark_young = only_mark_young; // Set this global variable to be used inside at_prot_functions.
  for (i=0; i<at_prot_functions_count; i++)
  {
    at_prot_functions[i]();
  }

  AT_markProtectedAFuns(only_mark_young);

  /* Mark 'parked' symbol */
  if (AT_isValidAFun(at_parked_symbol))
  {
    AT_markAFun(at_parked_symbol,only_mark_young);
  }
}

/*}}}  */

/*{{{  void sweep_phase()  */

static
void sweep_phase()
{
  size_t size;

  for (size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++)
  {
    terminfo[size].at_freelist = NULL;
  }
  old_bytes_in_young_blocks_after_last_major = 0;
  old_bytes_in_old_blocks_after_last_major = 0;

  /* Warning: freelist[size] is empty*/
  /* Warning: do not sweep fresh promoted block*/
  major_sweep_phase_old();
  major_sweep_phase_young();
  check_unmarked_block(AT_BLOCK);
  check_unmarked_block(AT_OLD_BLOCK);
}

/*}}}  */
/*{{{  void AT_init_gc_parameters()  */

void AT_init_gc_parameters()
{
  /* 20MB for 10 sizes in average*/
  gc_min_number_of_blocks = 2*(20*1024*1024)/(10*sizeof(Block));
  max_freeblocklist_size  = 100;
  min_nb_minor_since_last_major = 10;
  good_gc_ratio = 50;
  small_allocation_rate_ratio = 75;
  old_increase_rate_ratio = 50;
}

/*}}}  */

/*{{{  static void reclaim_empty_block(Block **blocks, int size, Block *removed_block, Block *prev_block)  */

static void reclaim_empty_block(size_t blocks, size_t size, Block* removed_block, Block* prev_block)
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

  ti->at_nrblocks--;
  removed_block->size = 0;
  if (prev_block == NULL)
  {
    ti->at_blocks[blocks] = removed_block->next_by_size;
    if (blocks==AT_BLOCK && ti->at_blocks[AT_BLOCK])
    {
      ti->top_at_blocks = ti->at_blocks[AT_BLOCK]->end;
    }
  }
  else
  {
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
  if (at_freeblocklist_size > max_freeblocklist_size)
  {
    size_t idx, next_idx;
    Block* cur;
    Block* prev = NULL;

    assert(removed_block != NULL);

    idx = ADDR_TO_BLOCK_IDX(removed_block);
    next_idx = (idx+1)%BLOCK_TABLE_SIZE;
    for (cur=block_table[idx].first_after; cur ; prev=cur, cur=cur->next_after)
    {
      if (removed_block == cur)
      {
        break;
      }
    }
    if (!cur)
    {
      std::runtime_error("### block " + to_string(removed_block) + " not found");
    }

    if (prev==NULL)
    {
      block_table[idx].first_after       = removed_block->next_after;
      block_table[next_idx].first_before = removed_block->next_after;
    }
    else
    {
      prev->next_after  = removed_block->next_after;
      prev->next_before = removed_block->next_before;
    }

    at_freeblocklist_size--;
    at_freeblocklist = at_freeblocklist->next_by_size;
    AT_free(removed_block);
  }
}

/*}}}  */
/*{{{  static void promote_block_to_old(int size, Block *block, Block *prev_block)  */

static void promote_block_to_old(size_t size, Block* block, Block* prev_block)
{
  TermInfo* ti = &terminfo[size];

  assert(block!=NULL);
  if (prev_block == NULL)
  {
    ti->at_blocks[AT_BLOCK] = block->next_by_size;
    if (ti->at_blocks[AT_BLOCK])
    {
      ti->top_at_blocks = ti->at_blocks[AT_BLOCK]->end;
    }

  }
  else
  {
    prev_block->next_by_size = block->next_by_size;
  }
  block->next_by_size = ti->at_blocks[AT_OLD_BLOCK];
  ti->at_blocks[AT_OLD_BLOCK] = block;
}

/*}}}  */
/*{{{  static void promote_block_to_young(int size, Block *block, Block *prev_block)  */

static void promote_block_to_young(size_t size, Block* block, Block* prev_block)
{
  TermInfo* ti = &terminfo[size];

  assert(block!=NULL);
  if (prev_block == NULL)
  {
    ti->at_blocks[AT_OLD_BLOCK] = block->next_by_size;
  }
  else
  {
    prev_block->next_by_size = block->next_by_size;
  }
  if (ti->at_blocks[AT_BLOCK])
  {
    block->next_by_size = ti->at_blocks[AT_BLOCK]->next_by_size;
    ti->at_blocks[AT_BLOCK]->next_by_size = block;
  }
  else
  {
    block->next_by_size = NULL;
    ti->at_blocks[AT_BLOCK] = block;
    ti->top_at_blocks = block->end;
    assert(ti->at_blocks[AT_BLOCK] != NULL);
  }
}

/*}}}  */

/*{{{  void check_unmarked_block(Block **blocks)  */

static void check_unmarked_block(size_t blocks)
{
#ifndef NDEBUG
  size_t size;

  for (size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++)
  {
    Block* block = terminfo[size].at_blocks[blocks];
    header_type* end = NULL;

    if (blocks == AT_BLOCK)
    {
      end = terminfo[size].top_at_blocks;
    }
    else
    {
      if (block)
      {
        end = block->end;
      }
    }

    while (block)
    {
      header_type* cur;
      for (cur=block->data ; cur<end ; cur+=size)
      {
        ATerm t = (ATerm)cur;
        if (blocks==AT_OLD_BLOCK)
        {
          assert(GET_TYPE(t->header)==AT_FREE || IS_OLD(t->header));
        }
        assert(!IS_MARKED(t->header));
      }
      block = block->next_by_size;
      if (block)
      {
        end = block->end;
      }
    }
  }
#endif // NDEBUG
}

/*}}}  */

/*{{{  void major_sweep_phase_old()  */

void major_sweep_phase_old()
{
  size_t size;

  for (size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++)
  {
    Block* prev_block = NULL;
    Block* next_block;

    Block* block = terminfo[size].at_blocks[AT_OLD_BLOCK];

    while (block)
    {
      /* set empty = 0 to avoid recycling*/
      int empty = 1;
      size_t alive_in_block = 0;
      size_t dead_in_block  = 0;
      size_t free_in_block  = 0;
      size_t capacity = ((block->end)-(block->data))/size;
      header_type* cur;

      assert(block->size == size);

      for (cur=block->data ; cur<block->end ; cur+=size)
      {
        /* TODO: Optimisation*/
        ATerm t = (ATerm)cur;
        if (IS_MARKED(t->header))
        {
          CLR_MARK(t->header);
          alive_in_block++;
          empty = 0;
          assert(IS_OLD(t->header));
        }
        else
        {
          switch (ATgetType(t))
          {
            case AT_FREE:
              assert(IS_YOUNG(t->header));
              free_in_block++;
              break;
            case AT_INT:
            case AT_APPL:
            case AT_LIST:
              assert(IS_OLD(t->header));
              AT_freeTerm(size, t);
              t->header=FREE_HEADER;
              dead_in_block++;
              break;
            case AT_SYMBOL:
              assert(IS_OLD(t->header));
              AT_freeAFun((SymEntry)t);
              t->header=FREE_HEADER;
              dead_in_block++;
              break;
            default:
              std::runtime_error("panic in sweep phase");
          }
        }
      }
      assert(alive_in_block + dead_in_block + free_in_block == capacity);

      next_block = block->next_by_size;

#ifndef NDEBUG
      if (empty)
      {
        for (cur=block->data; cur<block->end; cur+=size)
        {
          assert(ATgetType((ATerm)cur) == AT_FREE);
        }
      }
#endif

      if (empty)
      {
        /* DO NOT RESTORE THE FREE LIST: free cells have not been inserted*/
        /* terminfo[size].at_freelist = old_freelist;*/
        assert(terminfo[size].top_at_blocks < block->data || terminfo[size].top_at_blocks > block->end);
        reclaim_empty_block(AT_OLD_BLOCK, size, block, prev_block);
      }
      else if (0 && 100*alive_in_block/capacity <= TO_YOUNG_RATIO)
      {
        promote_block_to_young(size, block, prev_block);
        old_bytes_in_young_blocks_after_last_major += (alive_in_block*SIZE_TO_BYTES(size));
      }
      else
      {
        old_bytes_in_old_blocks_after_last_major += (alive_in_block*SIZE_TO_BYTES(size));

        /* DO NOT FORGET THIS LINE*/
        /* update the previous block*/
        prev_block = block;
      }

      block = next_block;
    }
  }
}

/*}}}  */
/*{{{  void major_sweep_phase_young()  */

void major_sweep_phase_young()
{
  old_bytes_in_young_blocks_since_last_major = 0;

  for (size_t size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++)
  {
    Block* prev_block = NULL;
    Block* next_block;
    ATerm old_freelist;
    TermInfo* ti = &terminfo[size];

    Block* block      = ti->at_blocks[AT_BLOCK];
    header_type* end  = ti->top_at_blocks;

    while (block)
    {
      int empty = 1;
      size_t alive_in_block = 0;
      size_t dead_in_block  = 0;
      size_t free_in_block  = 0;
      size_t old_in_block   = 0;
      size_t young_in_block = 0;
      size_t capacity = (end-(block->data))/size;
      header_type* cur;

      assert(block->size == size);

      old_freelist = ti->at_freelist;
      for (cur=block->data ; cur<end ; cur+=size)
      {
        ATerm t = (ATerm)cur;
        if (IS_MARKED(t->header))
        {
          CLR_MARK(t->header);
          alive_in_block++;
          empty = 0;
          if (IS_OLD(t->header))
          {
            old_in_block++;
          }
          else
          {
            young_in_block++;
            INCREMENT_AGE(t->header);
          }
        }
        else
        {
          switch (ATgetType(t))
          {
            case AT_FREE:
              t->aterm.next = ti->at_freelist;
              ti->at_freelist = t;
              free_in_block++;
              break;
            case AT_INT:
            case AT_APPL:
            case AT_LIST:
              AT_freeTerm(size, t);
              t->header = FREE_HEADER;
              t->aterm.next  = ti->at_freelist;
              ti->at_freelist = t;
              dead_in_block++;
              break;
            case AT_SYMBOL:
              AT_freeAFun((SymEntry)t);
              t->header = FREE_HEADER;
              t->aterm.next = ti->at_freelist;
              ti->at_freelist = t;

              dead_in_block++;
              break;
            default:
              std::runtime_error("panic in sweep phase");
          }
        }
      }
      assert(alive_in_block + dead_in_block + free_in_block == capacity);

      next_block = block->next_by_size;

#ifndef NDEBUG
      if (empty)
      {
        for (cur=block->data; cur<end; cur+=size)
        {
          assert(ATgetType((ATerm)cur) == AT_FREE);
        }
      }
#endif

      if (end==block->end && empty)
      {
        ti->at_freelist = old_freelist;
        reclaim_empty_block(AT_BLOCK, size, block, prev_block);
      }
      else if (end==block->end && 100*old_in_block/capacity >= TO_OLD_RATIO)
      {
        if (young_in_block == 0)
        {
          ti->at_freelist = old_freelist;
          promote_block_to_old(size, block, prev_block);
          old_bytes_in_old_blocks_after_last_major += (old_in_block*SIZE_TO_BYTES(size));
        }
        else
        {
          SET_FROZEN(block);
          old_bytes_in_young_blocks_after_last_major += (old_in_block*SIZE_TO_BYTES(size));
          ti->at_freelist = old_freelist;
          prev_block = block;
        }
      }
      else
      {
        old_bytes_in_young_blocks_after_last_major += (old_in_block*SIZE_TO_BYTES(size));
        prev_block = block;
      }

      block = next_block;
      if (block)
      {
        end = block->end;
      }
    }

#ifndef NDEBUG
    if (ti->at_freelist)
    {
      ATerm data;
      for (data = ti->at_freelist ; data ; data=data->aterm.next)
      {
        assert(EQUAL_HEADER(data->header,FREE_HEADER));
        assert(ATgetType(data) == AT_FREE);
      }
    }
#endif

  }
}

/*}}}  */
/*{{{  void minor_sweep_phase_young()  */

void minor_sweep_phase_young()
{
  old_bytes_in_young_blocks_since_last_major = 0;

  for (size_t size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++)
  {
    Block* prev_block = NULL;
    Block* next_block;
    ATerm old_freelist;
    TermInfo* ti = &terminfo[size];

    Block* block = ti->at_blocks[AT_BLOCK];
    header_type* end = ti->top_at_blocks;

    /* empty the freelist*/
    ti->at_freelist = NULL;

    while (block)
    {
      /* set empty = 0 to avoid recycling*/
      int empty = 1;
      size_t alive_in_block = 0;
      size_t dead_in_block  = 0;
      size_t free_in_block  = 0;
      size_t old_in_block  = 0;
      size_t capacity = (end-(block->data))/size;
      header_type* cur;

      assert(block->size == size);

      old_freelist = ti->at_freelist;
      for (cur=block->data ; cur<end ; cur+=size)
      {
        ATerm t = (ATerm)cur;
        if (IS_MARKED(t->header) || IS_OLD(t->header))
        {
          if (IS_OLD(t->header))
          {
            old_in_block++;
          }
          else
          {
            INCREMENT_AGE(t->header);
          }
          CLR_MARK(t->header);
          alive_in_block++;
          empty = 0;
          assert(!IS_MARKED(t->header));
        }
        else
        {
          switch (ATgetType(t))
          {
            case AT_FREE:
              /* ti->at_freelist is not empty: so DO NOT ADD t*/
              t->aterm.next = ti->at_freelist;
              ti->at_freelist = t;
              free_in_block++;
              break;
            case AT_INT:
            case AT_APPL:
            case AT_LIST:
              AT_freeTerm(size, t);
              t->header = FREE_HEADER;
              t->aterm.next   = ti->at_freelist;
              ti->at_freelist = t;

              dead_in_block++;
              break;
            case AT_SYMBOL:
              AT_freeAFun((SymEntry)t);
              t->header = FREE_HEADER;
              t->aterm.next   = ti->at_freelist;
              ti->at_freelist = t;
              dead_in_block++;
              break;

            default:
              std::runtime_error("panic in sweep phase");
          }
          assert(!IS_MARKED(t->header));
        }
      }

      assert(alive_in_block + dead_in_block + free_in_block == capacity);
      next_block    = block->next_by_size;

#ifndef NDEBUG
      if (empty)
      {
        for (cur=block->data; cur<end; cur+=size)
        {
          assert(ATgetType((ATerm)cur) == AT_FREE);
        }
      }
#endif

      /* Do not reclaim frozen blocks */
      if (IS_FROZEN(block))
      {
        ti->at_freelist = old_freelist;
      }

      /* TODO: create freeList Old*/
      if (0 && empty)
      {
        ti->at_freelist = old_freelist;
        reclaim_empty_block(AT_BLOCK, size, block, prev_block);
      }
      else if (0 && 100*old_in_block/capacity >= TO_OLD_RATIO)
      {
        promote_block_to_old(size, block, prev_block);
      }
      else
      {
        old_bytes_in_young_blocks_since_last_major += (old_in_block*SIZE_TO_BYTES(size));
        prev_block = block;
      }

      block = next_block;
      if (block)
      {
        end = block->end;
      }
    }

#ifndef NDEBUG
    if (ti->at_freelist)
    {
      ATerm data;
      for (data = ti->at_freelist ; data ; data=data->aterm.next)
      {
        if (!EQUAL_HEADER(data->header,FREE_HEADER))
        {
          mCRL2log(mcrl2::log::error) << "data = " << (void*)data << " header = " << (size_t) data->header << std::endl;
        }
        assert(EQUAL_HEADER(data->header,FREE_HEADER));
        assert(ATgetType(data) == AT_FREE);
      }
    }
#endif

  }
}

/*}}}  */

/*{{{  void AT_collect() */

void AT_collect(const bool only_collect_young)
{
  size_t size;

  /* snapshot*/
  for (size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++)
  {
    TermInfo* ti = &terminfo[size];
    ti->nb_live_blocks_before_last_gc = ti->at_nrblocks;
    ti->nb_reclaimed_blocks_during_last_gc=0;
    ti->nb_reclaimed_cells_during_last_gc=0;
  }

  check_unmarked_block(AT_BLOCK);
  check_unmarked_block(AT_OLD_BLOCK);
  mark_phase(only_collect_young);
  if (only_collect_young)
  { 
    minor_sweep_phase_young();
  }
  else
  {
    sweep_phase();
  }
  check_unmarked_block(AT_BLOCK);
  check_unmarked_block(AT_OLD_BLOCK);
}

/*}}}  */

} // namespace aterm
