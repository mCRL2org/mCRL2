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

extern ATprotected_block protected_blocks;

AFun at_parked_symbol = (size_t)-1; // Init

/*}}}  */

#ifdef WIN32
#define VOIDCDECL static void __cdecl
#else
#define VOIDCDECL static void
#endif

/*{{{  void AT_initGC(int argc, char *argv[], ATerm *bottomOfStack) */

void AT_initGC(ATerm* bottomOfStack)
{
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

static void mark_memory(const ATerm* start, const ATerm* stop, const bool check_term) 
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
        if (&*real_term != NULL)
        {
          if (!IS_MARKED((real_term)->header))
          {
            assert(AT_isValidTerm(real_term));
            AT_markTerm(real_term);
          }
        }
      }
      else if (AT_isValidAFun((AFun)&**cur))
      {
        AT_markAFun((AFun)&**cur);
      }
    }
  }
  else
  {
    for (cur=start; cur<stop; cur++)
    {
      if ((&**cur!=NULL) && (!IS_MARKED((*cur)->header)))
      {
        assert(AT_isValidTerm(*cur));
        AT_markTerm(*cur);
      }
    }
  }
}

/*}}}  */

void ATmarkTerm(const ATerm t)
{
  const ATerm* start=&t;
  mark_memory(start,start+1,false); 
}

/*{{{  VOIDCDECL mark_phase() */

VOIDCDECL mark_phase()
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
  mark_memory((ATerm*)((char*)env), (ATerm*)((char*)env) + 12, true);
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
      AT_markTerm(real_term);
    }
    if (AT_isValidAFun((AFun)reg[i]))
    {
      AT_markAFun((AFun)reg[i]);
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
  mark_memory(start, stop,true);
#endif

  stackTop = stack_top();

  start = MIN(stackTop, stackBot);
  stop  = MAX(stackTop, stackBot);

  mark_memory(start, stop,true);

  /* Traverse protected terms */
  for (i=0; i<at_prot_table_size; i++)
  {
    ProtEntry* cur = at_prot_table[i];
    while (cur)
    {
      for (j=0; j<cur->size; j++)
      {
        if (&*(cur->start[j]))
        {
          assert(AT_isValidTerm(cur->start[j]));
          AT_markTerm(cur->start[j]);
        }
      }
      cur = cur->next;
    }
  }

  for (prot=at_prot_memory; prot != NULL; prot=prot->next)
  {
    mark_memory((ATerm*)prot->start, (ATerm*)((prot->start) + prot->size),false);
  }

  for (pblock=protected_blocks; pblock != NULL; pblock=pblock->next)
  {
    if (pblock->protsize>0)
    {
      mark_memory(pblock->term, &pblock->term[pblock->protsize],false);
    }
  }

  for (i=0; i<at_prot_functions_count; i++)
  {
    at_prot_functions[i]();
  }

  AT_markProtectedAFuns();

  /* Mark 'parked' symbol */
  if (AT_isValidAFun(at_parked_symbol))
  {
    AT_markAFun(at_parked_symbol);
  }
}

/*}}}  */

/*{{{  static void reclaim_empty_block(int size, Block *removed_block, Block *prev_block)  */

static void reclaim_empty_block(size_t size, Block* removed_block, Block* prev_block)
{
  TermInfo* ti = &terminfo[size];

  /*
   * Step 1:
   *
   * remove cells from terminfo[size].at_freelist
   * remove the block from terminfo[size].at_block
   *
   */

  removed_block->size = 0;
  if (prev_block == NULL)
  {
    ti->at_block = removed_block->next_by_size;
    if (ti->at_block)
    {
      ti->top_at_blocks = ti->at_block->end;
    }
  }
  else
  {
    prev_block->next_by_size = removed_block->next_by_size;
  }

  if (at_freeblocklist_size > max_freeblocklist_size)
  { 
    // put the block into at_freeblocklist
    removed_block->next_by_size = at_freeblocklist;
    at_freeblocklist = removed_block;
    at_freeblocklist_size++;
  }
  else
  {
    // free the memory
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

    AT_free(removed_block);
  }
}

/*}}}  */

/*{{{  void check_unmarked_block(Block **blocks)  */

static void check_unmarked_block()
{
#ifndef NDEBUG
  size_t size;

  for (size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++)
  {
    Block* block = terminfo[size].at_block;
    header_type* end = NULL;

    end = terminfo[size].top_at_blocks;

    while (block)
    {
      header_type* cur;
      for (cur=block->data ; cur<end ; cur+=size)
      {
        ATerm t = (_ATerm*)cur;
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
/*{{{  void sweep_phase()  */

static void sweep_phase()
{
size_t total_nr_terms=0;
size_t total_nr_free_terms=0;
size_t total_aterm_memory=0;

  for (size_t size=MIN_TERM_SIZE; size<AT_getMaxTermSize(); size++)
  {
    Block* prev_block = NULL;
    Block* next_block;
    TermInfo* ti = &terminfo[size];

    Block* block = ti->at_block;
    header_type* end = ti->top_at_blocks;

    /* empty the freelist*/
    ti->at_freelist = NULL;

    while (block)
    {
      bool empty = true;
      header_type* cur;

      assert(block->size == size);

      const ATerm old_freelist = ti->at_freelist;
      for (cur=block->data ; cur<end ; cur+=size)
      {
        ATerm t = (_ATerm*)cur;
        if (IS_MARKED(t->header))
        {
++total_nr_terms;
total_aterm_memory +=size*8;
          CLR_MARK(t->header);
          empty = false;
          assert(!IS_MARKED(t->header));
        }
        else
        {
++total_nr_free_terms;
          switch (ATgetType(t))
          {
            case AT_FREE:
              t->aterm.next = ti->at_freelist;
              ti->at_freelist = &*t;
              break;
            case AT_INT:
            case AT_APPL:
            case AT_LIST:
              AT_freeTerm(size, t);
              t->header = FREE_HEADER;
              t->aterm.next  = ti->at_freelist;
              ti->at_freelist = &*t;
              break;
            case AT_SYMBOL:
              AT_freeAFun((SymEntry)&*t);
              t->header = FREE_HEADER;
              t->aterm.next = ti->at_freelist;
              ti->at_freelist = &*t;

              break;
            default:
              std::runtime_error("panic in sweep phase");
          }
          assert(!IS_MARKED(t->header));
        }
      }

      next_block = block->next_by_size;

#ifndef NDEBUG
      if (empty)
      {
        for (cur=block->data; cur<end; cur+=size)
        {
          assert(ATgetType((_ATerm*)cur) == AT_FREE);
        }
      }
#endif

      if (end==block->end && empty)
      {
        ti->at_freelist = &*old_freelist;
        reclaim_empty_block(size, block, prev_block);
      }
      else
      {
        prev_block = block;
      }

      block = next_block;
      if (block)
      {
        end = block->end;
      }
    }


#ifndef NDEBUG
    _ATerm* data;
    for (data = ti->at_freelist ; data ; data=data->aterm.next)
    {
      if (!EQUAL_HEADER(data->header,FREE_HEADER))
      {
        mCRL2log(mcrl2::log::error) << "data = " << (void*)data << " header = " << (size_t) data->header << std::endl;
      }
      assert(EQUAL_HEADER(data->header,FREE_HEADER));
      assert(ATgetType(data) == AT_FREE);
    }
    
#endif

  }
fprintf(stderr,"Total memory aterms: %ld free %ld memory   %ld\n",total_nr_terms,total_nr_free_terms,total_aterm_memory);
}

/*}}}  */

/*{{{  void AT_collect() */

void AT_collect()
{
  check_unmarked_block();
  mark_phase();
  sweep_phase();
  check_unmarked_block();
}

/*}}}  */

} // namespace aterm
