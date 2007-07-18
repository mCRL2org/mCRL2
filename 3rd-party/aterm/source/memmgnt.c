#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "memory.h"
#include "aterm2.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

char memmgnt_id[] = "$Id: memmgnt.c 23071 2007-07-02 10:06:17Z eriks $";

extern ATbool low_memory;
void free_unused_blocks();

#define MAX_UNUSED_BLOCKS 16
#define MIN_BLOCK_SIZE 256

/* Basic memory management functions mimicing the functionality of malloc, calloc, realloc and free
 */

void *AT_malloc(size_t size)
{
  void* ptr=malloc(size);
  if (!ptr) {
    free_unused_blocks();
    ptr=malloc(size);
  }
  return ptr;
}

void *AT_calloc(size_t nmemb, size_t size)
{
  void* ptr=calloc(nmemb, size);
  if (!ptr) {
    free_unused_blocks();
    ptr=calloc(nmemb, size);
  }
  return ptr;
}

void *AT_realloc(void *ptr, size_t size)
{
  void* newptr=realloc(ptr, size);
  if (!newptr) {
    free_unused_blocks();
    newptr=realloc(ptr, size);
  }
  return newptr;
}

void AT_free(void* ptr)
{
  free(ptr);
}

/* Memory management functions for protected blocks
 */

ATprotected_block protected_blocks, unused_blocks;

void AT_initMemmgnt()
{
  protected_blocks = NULL;
  unused_blocks = NULL;
}

size_t new_block_size(size_t old_size, size_t min_size, size_t max_size)
{
  size_t m;
	
  if (low_memory)  /* In low memory mode acquire the least ammount of memory needed */
    return min_size;
    
  if (max_size < min_size) /* maxsize >= minsize */
    max_size = min_size;
  
  if (max_size < old_size) /* Don't shrink blocks */
    return old_size;
    
  /* Reserve a bit extra space for future growth */
  m = max_size + (max_size - old_size)/2;
  if (m < MIN_BLOCK_SIZE)
    return MIN_BLOCK_SIZE;
    
  return m;
}

ATprotected_block find_best_unused_block(size_t minsize, size_t maxsize) 
{
  /* Returns a block that has at least minsize room, and is the closest match
   * to maxsize
   */
  ATprotected_block block = unused_blocks, tail, best = NULL;
  size_t bestsize = -1;
  size_t unused_blocks_count = 0;
  size_t optsize = maxsize + maxsize/2;
  
  if (maxsize < minsize) maxsize = minsize;
  
  while ( (block) && (bestsize!=optsize) ) {
    if (block->size >= minsize) {
      if (!best) {
      	/* Found a first match */
      	best = block;
      	bestsize = block->size;
      }
      else if (bestsize < optsize) {
      	/* Current best block is smaller than optsize, find one that is bigger than current block */
      	if (block->size > bestsize) {
      	  best = block;
      	  bestsize = block->size;
      	}
      }
      else {
      	/* Current best block is bigger than optsize, find the smallest block greater or equal to optsize */
        if ((block->size >= optsize) && (block->size < bestsize)) {
          best = block;
          bestsize = block->size;
        }
      }
    }
    tail = block;
    block = block->next;
    unused_blocks_count = 0;
  }
  
  if (best) {
    /* Found a matching block. Unlink it from unused blocks chain */
  	
    if (best->prev) {
      best->prev->next = best->next;
      if (best->next) {
        best->next->prev = best->prev;
      }
    }
    else {
      unused_blocks = best->next;
      if (best->next) {
        best->next->prev = NULL;
      }
    }
  }
  else if (unused_blocks_count > MAX_UNUSED_BLOCKS) {
    /* Remove the oldest block from the unused chain */
  	
    tail->prev->next = NULL;
    AT_free(tail);
  }
  
  return best;
}

#define malloc_size(s) (s*sizeof(ATerm) + sizeof(struct _ATprotected_block))

ATprotected_block find_free_block(size_t minsize, size_t maxsize)
{
  ATprotected_block block;
  size_t blocksize;
  
  /* Try to find an unused block that fits  */
  block = find_best_unused_block(minsize, maxsize);
  if (!block) {
    /* No existing block matches, create a new block */
    blocksize = new_block_size(0, minsize, maxsize);
    
    block = (ATprotected_block) AT_malloc(malloc_size(blocksize));
    if ((!block) && (blocksize > maxsize)) {
      /* Out of memory, try again with maximum block size */
      blocksize = maxsize;
      block = (ATprotected_block) AT_malloc(malloc_size(blocksize));
    }
    if ((!block) && (blocksize > minsize)) {
      /* Out of memory, try again with minimum block size */
      blocksize = minsize;
      block = (ATprotected_block) AT_malloc(malloc_size(blocksize));
    }
    if (!block) {
      return NULL;
    }
    
    block->term = (ATerm*)((void*)block + sizeof(struct _ATprotected_block));
    block->size = blocksize;
  }
  
  assert(block->size >= minsize);

  /* Clear the protected area */
  if (minsize > 0) {
    memset((void*)block->term, 0, minsize*sizeof(ATerm));
  }
  block->protsize = minsize;

  /* Link block to protected blocks chain head */
  if (protected_blocks)
    protected_blocks->prev = block;
  block->next = protected_blocks;
  block->prev = NULL;
  protected_blocks = block;
  
  return block;
}

ATprotected_block find_block(ATerm* term)
{
  /* Reconstruct the pointer to the block from the pointer to the terms */
  ATprotected_block block = (ATprotected_block)((void*)term - sizeof(struct _ATprotected_block));
  assert(block->term == term);
  return block;
}

void free_blocks(ATprotected_block block)
{
  /* Free given block and all blocks from this block onward */
  ATprotected_block next;
  while (block) {
    next = block->next;
    AT_free(block);
    block = next;
  }
}

void free_unused_blocks()
{
  /* Free all blocks in the unused blocks chain */
  free_blocks(unused_blocks); 
  unused_blocks = NULL;
}

void free_protected_blocks()
{
  /* Free all blocks in the protected blocks chain */
  free_blocks(protected_blocks); 
  protected_blocks = NULL;
}

void free_block(ATprotected_block block)
{
  /* Free a single block */

  /* Unlink the block from the chain */  
  if (block->prev) {
    block->prev->next = block->next;
    if (block->next)
      block->next->prev = block->prev;
  }
  else {
    if (block->protsize >= 0)
      protected_blocks = block->next;
    else
      unused_blocks = block->next;
      
    if (block->next)
      block->next->prev = NULL;
  }
    
  if (!low_memory) {
    /* Don't actually free the block, but move it to the unused blocks chain */
    block->protsize = -1;
    
    if (unused_blocks)
      unused_blocks->prev = block;
    block->next = unused_blocks;
    block->prev = NULL;
    unused_blocks = block;
  }
  else {
    AT_free((void*)block);
  }
}

ATprotected_block resize_block(ATprotected_block block, size_t minsize, size_t maxsize)
{
  ATprotected_block newblock;
  
  /* Calculate new block size */
  size_t blocksize = new_block_size(block->size, minsize, maxsize);

  if (blocksize != block->size) {
    /* New block size differs from old block, reallocate the block */
    newblock = (ATprotected_block)AT_realloc((void*)block, malloc_size(blocksize));

    if ((!newblock)&&(blocksize > maxsize)) {
      /* Realloc failed; try with maximum size */ 
      blocksize = maxsize;
      newblock = (ATprotected_block)AT_realloc((void*)block, malloc_size(blocksize));
    }

    if ((!newblock)&&(blocksize > minsize)) {
      /* Realloc failed; try with minimum size */ 
      blocksize = minsize;
      newblock = (ATprotected_block)AT_realloc((void*)block, malloc_size(blocksize));
    }

    if (!newblock) {
      return NULL;
    }
    
    newblock->term = (ATerm*)((void*)newblock + sizeof(struct _ATprotected_block));
    newblock->size = blocksize;

    /* Update the chain links */    
    if (newblock->prev) {
      newblock->prev->next = newblock;
    }
    else {
      protected_blocks = newblock;
    }

    if (newblock->next)
      newblock->next->prev = newblock;
  }
  else {
    /* No change in blocksize */
    newblock = block;
  }

  /* Clear the newly protected part of the block */
  if (newblock->protsize < minsize) {
    memset((void*)(&newblock->term[newblock->protsize]), 0, (minsize-newblock->protsize)*sizeof(ATerm));
  }
  newblock->protsize = minsize;
  
  return newblock;
}

ATerm *AT_alloc_protected(size_t size)
{
  /* Allocate a protected block of ATerms with the exact size given 
   */
  ATprotected_block block = find_free_block(size, size);
  if (!block)
    return NULL;
  return block->term;
}

ATerm *AT_alloc_protected_minmax(size_t minsize, size_t maxsize)
{
  /* Allocate a protected block of ATerms with minsize protected items, but create room to
   * grow to maxsize efficiently
   */
  ATprotected_block block = find_free_block(minsize, maxsize);
  if (!block)
    return NULL;
  return block->term;
}

ATerm *AT_realloc_protected(ATerm *term, size_t size)
{
  /* Resize an existing block to the new protected size
   */
  ATprotected_block block;
  
  if (!term)
    return AT_alloc_protected(size);

  block = find_block(term);
  assert(block);
    
  block = resize_block(block, size, size);
  if (!block)
    return NULL;

  return block->term;
}

ATerm *AT_realloc_protected_minmax(ATerm *term, size_t minsize, size_t maxsize)
{
  /* Resize a protected block of ATerms to minsize protected items, but create room to
   * grow to maxsize efficiently
   */
  ATprotected_block block;
  
  if (!term)
    return AT_alloc_protected_minmax(minsize, maxsize);

  block = find_block(term);
  assert(block);
    
  block = resize_block(block, minsize, maxsize);
  if (!block)
    return NULL;

  return block->term;
}

ATerm *AT_grow_protected(ATerm* term, size_t size)
{
  /* Increase the protected area of the block to size
   */
  ATprotected_block block;
  
  if (!term)
    return AT_alloc_protected(size);

  block = find_block(term);
  assert(block);
  
  if (block->protsize < size) {
    block = resize_block(block, size, block->size);
    if (!block)
      return NULL;
  }

  return block->term;
} 

void AT_free_protected(ATerm* term)
{
  /* Free a protected block */
  if (term) {
    ATprotected_block block = find_block(term);
    assert(block);
    free_block(block);
  }
}

void AT_free_protected_blocks()
{
  /* Free all blocks in use */
  free_unused_blocks();
  free_protected_blocks();
}
