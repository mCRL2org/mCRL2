#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mcrl2/aterm/memory.h"
#include "mcrl2/aterm/aterm2.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace aterm
{

char memmgnt_id[] = "$Id$";

void free_unused_blocks();

static const size_t MAX_UNUSED_BLOCKS = 16;
static const size_t MIN_BLOCK_SIZE = 256;

/* Basic memory management functions mimicking the functionality of malloc, calloc, realloc and free
 */

/* static int total_count=0; */

void* AT_malloc(const size_t size)
{
  void* ptr=malloc(size);
/*  if (!ptr)
  {
    free_unused_blocks();
    ptr=malloc(size);
  } */
  return ptr;
}

void* AT_calloc(const size_t nmemb, const size_t size)
{
  void* ptr=calloc(nmemb, size);
/*  if (!ptr)
  {
    free_unused_blocks();
    ptr=calloc(nmemb, size);
  } 
*/
  return ptr;
}

void* AT_realloc(void* ptr, const size_t size)
{
  void* newptr=realloc(ptr, size);
/*  if (!newptr)
  {
    free_unused_blocks();
    newptr=realloc(ptr, size);
  } 
*/
  return newptr;
}

void AT_free(void* ptr)
{
  free(ptr);
}

/* Memory management functions for protected blocks
 */

/* ATprotected_block protected_blocks, unused_blocks;

void AT_initMemmgnt()
{
  protected_blocks = NULL;
  unused_blocks = NULL;
} */

/* static
size_t new_block_size(size_t old_size, size_t new_size)
{
  if (new_size < old_size) / * Don't shrink blocks * /
  {
    return old_size;
  }

  / * Reserve a bit extra space for future growth * /
  / * m = max_size + (max_size - old_size)/2;  JFG ODD TO RESERVE 1.5 times the memory.
     tenzij er sprake is van groeiend geheugen.  * /
  if (new_size < MIN_BLOCK_SIZE)
  {
    return MIN_BLOCK_SIZE;
  }

  return new_size;
} */

/* static
ATprotected_block find_best_unused_block(size_t new_size)
{
  / * Returns a block that has at least minsize room, and is the closest match
   * to maxsize
   * /
  ATprotected_block block = unused_blocks, tail, best = NULL;
  size_t bestsize = (size_t)-1; // Assigned on all paths
  size_t unused_blocks_count = 0;
  size_t optsize = new_size + new_size/2;  // TODO THIS IS ODD.

  while ((block) && (bestsize!=optsize))
  {
    if (block->size >= new_size)
    {
      if (!best)
      {
        / * Found a first match * /
        best = block;
        bestsize = block->size;
      }
      else if (bestsize < optsize)
      {
        / * Current best block is smaller than optsize, find one that is bigger than current block * /
        if (block->size > bestsize)
        {
          best = block;
          bestsize = block->size;
        }
      }
      else
      {
        / * Current best block is bigger than optsize, find the smallest block greater or equal to optsize * /
        if ((block->size >= optsize) && (block->size < bestsize))
        {
          best = block;
          bestsize = block->size;
        }
      }
    }
    tail = block;
    block = block->next;
    unused_blocks_count = 0;
  }

  if (best)
  {
    / * Found a matching block. Unlink it from unused blocks chain * /

    if (best->prev)
    {
      best->prev->next = best->next;
      if (best->next)
      {
        best->next->prev = best->prev;
      }
    }
    else
    {
      unused_blocks = best->next;
      if (best->next)
      {
        best->next->prev = NULL;
      }
    }
  }
  else if (unused_blocks_count > MAX_UNUSED_BLOCKS)
  {
    / * Remove the oldest block from the unused chain * /

    tail->prev->next = NULL;
    AT_free(tail);
  }

  return best;
} */

/* inline
size_t malloc_size(const size_t s)
{
  return (s*sizeof(ATerm) + sizeof(struct _ATprotected_block));
} */
//#define malloc_size(s) (s*sizeof(ATerm) + sizeof(struct _ATprotected_block))

/* static
ATprotected_block find_free_block(size_t new_size)
{
  ATprotected_block block;
  size_t blocksize;

  / * Try to find an unused block that fits  * /
  block = find_best_unused_block(new_size);
  if (!block)
  {
    / * No existing block matches, create a new block * /
    blocksize = new_block_size(0, new_size);

    block = (ATprotected_block) AT_malloc(malloc_size(blocksize));
    if ((!block) && (blocksize > new_size))
    {
      / * Out of memory, try again with maximum block size * /
      blocksize = new_size;
      block = (ATprotected_block) AT_malloc(malloc_size(blocksize));
    }
    if (!block)
    {
      return NULL;
    }

    block->term = (ATerm*)((char*)block + sizeof(struct _ATprotected_block));
    block->size = blocksize;
  }

  assert(block->size >= new_size);

  / * Clear the protected area * /
  if (new_size > 0)
  {
    memset((void*)block->term, 0, new_size*sizeof(ATerm));
  }
  block->protsize = new_size;

  / * Link block to protected blocks chain head * /
  if (protected_blocks)
  {
    protected_blocks->prev = block;
  }
  block->next = protected_blocks;
  block->prev = NULL;
  protected_blocks = block;

  return block;
} */

/* static
ATprotected_block find_block(ATerm* term)
{
  / * Reconstruct the pointer to the block from the pointer to the terms * /
  ATprotected_block block = (ATprotected_block)((char*)term - sizeof(struct _ATprotected_block));
  assert(block->term == term);
  return block;
} */

/* static
void free_blocks(ATprotected_block block)
{
  / * Free given block and all blocks from this block onward * /
  ATprotected_block next;
  while (block)
  {
    next = block->next;
    AT_free(block);
    block = next;
  }
} */

/* void free_unused_blocks()
{
  / * Free all blocks in the unused blocks chain * /
  free_blocks(unused_blocks);
  unused_blocks = NULL;
} */

/* static
void free_protected_blocks()
{
  / * Free all blocks in the protected blocks chain * /
  free_blocks(protected_blocks);
  protected_blocks = NULL;
} */

/* static
void free_block(ATprotected_block block)
{
  / * Free a single block * /

  / * Unlink the block from the chain * /
  if (block->prev)
  {
    block->prev->next = block->next;
    if (block->next)
    {
      block->next->prev = block->prev;
    }
  }
  else
  {
    / * Blocks that are freed should not be part of the unused blocks
    if (block->protsize >= 0) [This succeeds always....]
    {  * /
    protected_blocks = block->next;
    / * }
    else
    {
      unused_blocks = block->next;
    } * /

    if (block->next)
    {
      block->next->prev = NULL;
    }
  }

  / * Don't actually free the block, but move it to the unused blocks chain * /
  block->protsize = -1;

  if (unused_blocks)
  {
    unused_blocks->prev = block;
  }
  block->next = unused_blocks;
  block->prev = NULL;
  unused_blocks = block;
} */

/* ATerm* AT_alloc_protected(const size_t size)
{
  / * Allocate a protected block of ATerms with the exact size given
   * /
  ATprotected_block block = find_free_block(size);
  if (!block)
  {
    return NULL;
  }
  return block->term;
} */

/* void AT_free_protected(ATerm* term)
{
  / * Free a protected block * /
  if (term)
  {
    ATprotected_block block = find_block(term);
    assert(block);
    free_block(block);
  }
}

void AT_free_protected_blocks()
{
  / * Free all blocks in use * /
  free_unused_blocks();
  free_protected_blocks();
} */

} // namespace aterm
