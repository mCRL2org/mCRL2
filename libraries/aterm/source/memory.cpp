/*{{{  includes */

#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdexcept>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/aterm/_aterm.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/memory.h"
#include "mcrl2/aterm/util.h"
#include "mcrl2/aterm/gc.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace aterm
{

/*{{{  globals */

char memory_id[] = "$Id$";

size_t maxTermSize = INITIAL_MAX_TERM_SIZE;

TermInfo* terminfo = NULL;

Block* at_freeblocklist = NULL;
size_t at_freeblocklist_size = 0;
BlockBucket block_table[BLOCK_TABLE_SIZE] = { { NULL, NULL } };

static size_t total_nodes = 0;

static size_t table_class = INITIAL_TERM_TABLE_CLASS;
static HashNumber table_size    = AT_TABLE_SIZE(INITIAL_TERM_TABLE_CLASS);
HashNumber table_mask    = AT_TABLE_MASK(INITIAL_TERM_TABLE_CLASS);

std::vector <_ATerm*> ATerm::hashtable;

extern void AT_initMemmgnt();

ATermList ATempty;
static _ATerm* address_of_ATempty;

size_t AT_getMaxTermSize()
{
  return maxTermSize;
}

/*{{{  static size_t term_size(ATerm t) */

/**
 * Calculate the size (in words) of a term.
 */

static size_t term_size(const _ATerm *t)
{
  size_t size = 0;

  switch (ATgetType(t))
  {
    case AT_INT:
      size = TERM_SIZE_INT;
      break;
    case AT_LIST:
      size = TERM_SIZE_LIST;
      break;
    case AT_APPL:
      size = TERM_SIZE_APPL(ATgetArity(ATgetAFun((_ATermAppl*)t)));
      break;
  }

  return size;
}

/*}}}  */

/*{{{  static HashNumber hash_number(ATerm t, size_t size) */

static HashNumber hash_number(const _ATerm *t, const size_t size)
{
  HashNumber hnr;

  // hnr = START(t->word[0]);
  hnr = START(t->header);

  for (size_t i=ARG_OFFSET; i<size; i++)
  {
    // hnr = COMBINE(hnr, t->word[i]);
    hnr = COMBINE(hnr, *(reinterpret_cast<const MachineWord *>(t) + i));
  }

  return FINISH(hnr);
}


/*}}}  */
/*{{{  HashNumber AT_hashnumber(ATerm t) */

HashNumber AT_hashnumber(const _ATerm *t)
{
  return hash_number(t, term_size(t));
}

/*}}}  */

/*{{{  static void resize_hashtable() */

/**
 * Resize the hashtable
 */

static void resize_hashtable()
{
  HashNumber oldsize;

  oldsize = table_size;
  table_class++;
  table_size = ((HashNumber)1)<<table_class;
  table_mask = table_size-1;
  std::vector < _ATerm* > new_hashtable;
fprintf(stderr,"RESIZE HASHTABLE %ld\n",table_class);

  /*{{{  Create new term table */
  try
  {
    new_hashtable.resize(table_size,NULL);
  }
  catch (std::bad_alloc &e)
  {
    mCRL2log(mcrl2::log::warning) << "could not resize hashtable to class " << table_class << ". " << e.what() << std::endl;
    table_class--;
    table_size = ((HashNumber)1)<<table_class;
    table_mask = table_size-1;
    return;
  }
  
  /*{{{  Rehash all old elements */

  for (std::vector < _ATerm*>::const_iterator p=ATerm::hashtable.begin(); p !=ATerm::hashtable.end(); p++)
  {
    _ATerm* aterm_walker=*p;

    while (aterm_walker)
    {
      assert(aterm_walker->reference_count>0);
      _ATerm* next = aterm_walker->next;
      const HashNumber hnr = hash_number(aterm_walker, term_size(aterm_walker)) & table_mask;
      assert(hnr<new_hashtable.size());
// fprintf(stderr,"HHHH %p   %p    %p\n",aterm_walker,new_hashtable[hnr],aterm_walker->next);
      aterm_walker->next = new_hashtable[hnr];
      new_hashtable[hnr] = aterm_walker;
assert(aterm_walker->next!=aterm_walker);
      aterm_walker = next;
    }
  }
  new_hashtable.swap(ATerm::hashtable);
fprintf(stderr,"RESIZE HASHTABLE END\n");

  /*}}}  */
}

/*}}}  */

/*{{{  void AT_initMemory() */
/**
 * Initialize memory allocation datastructures
 */

void AT_initMemory()
{
  /*{{{  Initialize blocks */

  terminfo = (TermInfo*) calloc(maxTermSize, sizeof(TermInfo));

  /*}}}  */
  /*{{{  Create term term table */

  try
  { 
    ATerm::hashtable.resize(table_size);
  }
  catch (std::bad_alloc &e)
  {
    std::runtime_error("AT_initMemory: cannot allocate term table of size " + to_string(table_size) + ". " + e.what());
  }

  for (size_t i=0; i<BLOCK_TABLE_SIZE; i++)
  {
    block_table[i].first_before = NULL;
    block_table[i].first_after  = NULL;
  }

  /*}}}  */
  /*{{{  Create the empty list */

  ATempty = reinterpret_cast<_ATermList*>(AT_allocate(TERM_SIZE_APPL(0)));
  // ATempty = static_cast<ATermList>(ATmakeAppl0(AS_EMPTY_LIST));
  address_of_ATempty=&*ATempty;
  
  ATempty->header = EMPTY_HEADER;
  ATempty->next = NULL;

  const HashNumber hnr = hash_number(&*ATempty, TERM_SIZE_LIST);
  ATerm::hashtable[hnr& table_mask] = &*ATempty; 

  /*}}}  */

}

static bool check_that_all_objects_are_free()
{
  return true;
  bool result=true;

  for(size_t size=0; size<maxTermSize; ++size)
  {
    TermInfo *ti=&terminfo[size];
    for(Block* b=ti->at_block; b!=NULL; b=b->next_by_size)
    {
      for(_ATerm* p=(_ATerm*)b->data; p!=NULL && ((b==ti->at_block && p<(_ATerm*)ti->top_at_blocks) || p<(_ATerm*)b->end); p=p + size)
      {
        if (p!=address_of_ATempty && ((p->header!=FREE_HEADER) || (p->reference_count!=0)))
        {
          if ((p->header!=FREE_HEADER))
          { 
            ATfprintf(stderr,"CHECK: Non free term %p,%t (size %d). ",&*p,&*p,size);
            // fprintf(stderr,"CHECK: Non free term %p (size %ld). ",&*p,size);
          }
          else
          {
            fprintf(stderr,"CHECK: Non free term %p with free header and positive reference count (%lX). ",p,(size_t)p);
          }
          fprintf(stderr,"Reference count %ld\n",p->reference_count);
          result=false;
        }
      }
    }
  }

  for(size_t i=0; i<AFun::at_lookup_table.size(); ++i)
  {
    if (i!=AS_EMPTY_LIST.number() && AFun::at_lookup_table[i]->reference_count>0)  // ATempty is not destroyed, so is AS_EMPTY_LIST.
    {
      result=false;
      fprintf(stderr,"Symbol %s has positive reference count (nr. %ld, ref.count %ld)\n",
                AFun::at_lookup_table[i]->name,AFun::at_lookup_table[i]->id,AFun::at_lookup_table[i]->reference_count);
    }

  }

  return result;
}

/*}}}  */
/*{{{  void AT_cleanupMemory() */

/**
 * Print hashtable info
 */

void AT_cleanupMemory()
{
  // AT_free(hashtable);
  // AT_free_protected_blocks();
  // check_that_all_objects_are_free();
  // free(terminfo); 
}

/*}}}  */

/**
 * Allocate a new block of a particular size class
 */

header_type* min_heap_address = (header_type*)(~0);
header_type* max_heap_address = 0;

/*{{{  static void allocate_block(size_t size)  */

static void allocate_block(size_t size)
{
  size_t idx;
  Block* newblock;
  bool init = false;
  TermInfo* ti;

  if (at_freeblocklist != NULL)
  {
    newblock = at_freeblocklist;
    at_freeblocklist = at_freeblocklist->next_by_size;
    at_freeblocklist_size--;
  }
  else
  {
    newblock = (Block*)calloc(1, sizeof(Block));
    if (newblock == NULL)
    {
      std::runtime_error("allocate_block: out of memory!");
    }
    init = true;
    min_heap_address = MIN(min_heap_address,(newblock->data));
    max_heap_address = MAX(max_heap_address,(newblock->data+BLOCK_SIZE));
    assert(min_heap_address < max_heap_address);
  }

  assert(size >= MIN_TERM_SIZE && size < maxTermSize);

  ti = &terminfo[size];

  newblock->end = (newblock->data) + (BLOCK_SIZE - (BLOCK_SIZE % size));

  newblock->size = size;
  newblock->next_by_size = ti->at_block;
  ti->at_block = newblock;
  ti->top_at_blocks = newblock->data;
  assert(ti->at_block != NULL);
  assert(((size_t)ti->top_at_blocks % MAX(sizeof(double), sizeof(void*))) == 0);

  /* [pem: Feb 14 02] TODO: fast allocation */
  assert(ti->at_freelist == NULL);

  if (init)
  {
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

static void AT_growMaxTermSize(size_t neededsize)
{
  TermInfo* newterminfo;
  size_t newsize;

  /* Grow to the minimum of what is needed and twice as the current size,
     to avoid the overhead of repeatedly growing chuncks of memory */
  newsize = (neededsize> 2*maxTermSize?neededsize:2*maxTermSize);

#ifndef NDEBUG
  mCRL2log(mcrl2::log::info) << "Growing administrative structures to accomodate terms of size " << newsize << std::endl;
#endif

  newterminfo = (TermInfo*)realloc((void*)terminfo, newsize*sizeof(TermInfo));
  if ((!newterminfo)&&(newsize>neededsize))
  {
    /* Realloc failed again; try with needed size */
    newsize = neededsize;
    newterminfo = (TermInfo*)realloc((void*)terminfo, newsize*sizeof(TermInfo));
  }
  if (!newterminfo)
  {
    std::runtime_error("AT_growMaxTermSize: cannot allocate " + to_string(newsize-maxTermSize) + " extra TermInfo elements.");
  }

  /* Clear new area */
  memset(&newterminfo[maxTermSize], 0, (newsize-maxTermSize)*sizeof(TermInfo));

  terminfo = newterminfo;
  maxTermSize = newsize;
}

/*{{{  ATerm AT_allocate(size_t size)  */

_ATerm* AT_allocate(const size_t size)
{
  if (size+1 > maxTermSize)
  {
    AT_growMaxTermSize(size+1);
  }

  if (total_nodes>=table_size)
  {
    // The hashtable is not big enough to hold nr_of_nodes_for_the_next_garbage_collect. So, resizing
    // is wise (although not necessary, due to the structure of the hastable, which allows is to contain
    // an arbitrary number of element, at some performance penalty.
    resize_hashtable();
  }

  _ATerm *at;
  TermInfo *ti = &terminfo[size];
  if (ti->at_block && ti->top_at_blocks < ti->at_block->end)
  {
    /* the first block is not full: allocate a cell */
    at = (_ATerm *)ti->top_at_blocks;
    ti->top_at_blocks += size;
    at->reference_count=0;
  }
  else if (ti->at_freelist)
  {
    /* the freelist is not empty: allocate a cell */
    at = ti->at_freelist;
    ti->at_freelist = ti->at_freelist->next;
    assert(ti->at_block != NULL);
    assert(ti->top_at_blocks == ti->at_block->end);
    assert(at->reference_count==0);
  }
  else
  {
    /* there is no more memory of the current size allocate a block */
    allocate_block(size);
    assert(ti->at_block != NULL);
    at = (_ATerm *)ti->top_at_blocks;
    ti->top_at_blocks += size;
    at->reference_count=0;
  }

  total_nodes++;
  return at;
} 

/*}}}  */

/*{{{  void AT_freeTerm(size_t size, ATerm t) */

/**
 * Free a term of a particular size.
 */

void AT_freeTerm(const size_t size, _ATerm *t)
{
  // fprintf(stderr,"Remove term from hashtable %p\n",t);
  _ATerm *prev=NULL, *cur;

  // terminfo[size].nb_reclaimed_cells_during_last_gc++;

  /* Remove the node from the hashtable */
  const HashNumber hnr = hash_number(t, size) & table_mask;
  cur = &*ATerm::hashtable[hnr];

  do
  {
    if (!cur)
    {
        std::runtime_error("AT_freeTerm: cannot find term in hashtable at pos " + to_string(hnr) + " header=" + to_string(t->header));
    }
    if (cur == t)
    {
      if (prev)
      {
        prev->next = cur->next;
      }
      else
      {
        ATerm::hashtable[hnr] = cur->next;
      }
      /* Put the node in the appropriate free list */
      total_nodes--;
      return;
    }
  }
  while (((prev=cur), (cur=cur->next)));
  assert(0);
}

/*}}}  */

void at_reduce_reference_count(_ATerm *t);

void at_free_term(_ATerm *t)
{
#ifndef NDEBUG
  if (t==address_of_ATempty) // When destroying ATempty, it appears that all other terms have been removed.
  {
    check_that_all_objects_are_free();
    return;
  }
#endif

  assert(t->reference_count==0);
  const size_t size=term_size(t);
  AT_freeTerm(size,t);  // Remove from hash_table

  // Reduce the reference count of subterms.
  if (ATgetType(t)==AT_APPL)
  {
    const size_t afun_number=ATgetAFun(reinterpret_cast<_ATermAppl*>(t));
    for(size_t i=0; i<ATgetArity(GET_SYMBOL(t->header)); ++i)
    {
      at_reduce_reference_count(reinterpret_cast<_ATermAppl*>(t)->arg[i]);
    }
    AFun::decrease_reference_count(afun_number);
  }
  else if ((ATgetType(t)==AT_LIST) && (t->header!=EMPTY_HEADER))
  {
    at_reduce_reference_count(reinterpret_cast<_ATermList*>(t)->head);
    at_reduce_reference_count(reinterpret_cast<_ATermList*>(t)->tail);
  }
  t->header = FREE_HEADER;
  TermInfo *ti = &terminfo[size];
  t->next  = ti->at_freelist;
  ti->at_freelist = t; 
}
          
void at_reduce_reference_count(_ATerm *t)
{
  assert(t->reference_count>0);
  if (0== --t->reference_count)
  {
    at_free_term(t);
  }
}


/*{{{  ATermAppl ATmakeAppl(AFun &sym, ...) */

/**
 * Create a new ATermAppl. The argument count can be found in the symbol.
 */

ATermAppl ATmakeAppl(const AFun &sym, ...)
{
  _ATerm* cur;
  size_t arity = ATgetArity(sym);
  bool found;
  header_type header;
  HashNumber hnr;
  va_list args;
  _ATerm* arg;
  // ATerm* buffer;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,_ATerm*,arity);

  // header = APPL_HEADER(arity > MAX_INLINE_ARITY ?  MAX_INLINE_ARITY+1 : arity, sym.number());
  header = APPL_HEADER(sym.number());

  // buffer = (ATerm*)AT_alloc_protected(arity);

  hnr = START(header);
  va_start(args, sym);
  for (size_t i=0; i<arity; i++)
  {
    arg = va_arg(args, _ATerm *);
    CHECK_TERM(&*arg);
    hnr = COMBINE(hnr, HN(&*arg));
    buffer[i] = arg;
  }
  va_end(args);
  hnr = FINISH(hnr);

  cur = (_ATermAppl *)&*ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->header==header)
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<_ATermAppl*>(cur)->arg[i]!=buffer[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = (_ATermAppl *)cur->next;
  }

  if (!cur)
  {
    cur = (_ATermAppl *) &*AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    AFun::increase_reference_count<true>(sym.number());
    for (size_t i=0; i<arity; i++)
    {
      buffer[i]->reference_count++;
      reinterpret_cast<_ATermAppl*>(cur)->arg[i] = buffer[i];
    }
    cur->next = &*ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  // AT_free_protected(buffer);

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl0(AFun &sym) */

ATermAppl ATmakeAppl0(const AFun &sym)
{
  _ATerm *cur, *prev, **hashspot;
  header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;


  CHECK_ARITY(ATgetArity(sym), 0);

  hnr = FINISH(START(header));

  prev = NULL;
  hashspot = &(ATerm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->header==header)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next = cur->next;
        cur->next = (_ATerm*) &**hashspot;
        *hashspot = cur;
      }

      return reinterpret_cast<_ATermAppl*>(cur);
    }
    prev = cur;
    cur = cur->next;
  }

  cur = (_ATermAppl*) &*AT_allocate(TERM_SIZE_APPL(0));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  AFun::increase_reference_count<true>(sym.number());
  cur->next = &*ATerm::hashtable[hnr];
  ATerm::hashtable[hnr] = cur;

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl1(AFun &sym, ATerm &arg0) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl1(const AFun &sym, const ATerm &arg0)
{
  _ATerm* cur, *prev, **hashspot;
  header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 1);
  CHECK_TERM(&*arg0);

  hnr = START(header);
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = &(ATerm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if ((header==cur->header) && 
         reinterpret_cast<_ATermAppl*>(cur)->arg[0] == &*arg0)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next = cur->next;
        cur->next = *hashspot;
        *hashspot = cur;
      }
      return reinterpret_cast<_ATermAppl*>(cur);
    }
    prev = cur;
    cur = cur->next;
  }

  cur = AT_allocate(TERM_SIZE_APPL(1));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  AFun::increase_reference_count<true>(sym.number());
  arg0->reference_count++;
  reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
  cur->next = ATerm::hashtable[hnr];
  ATerm::hashtable[hnr] = cur;

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl2(AFun &sym, &arg0, &arg1) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl2(const AFun &sym, const ATerm &arg0, const ATerm &arg1)
{
  _ATerm* cur, *prev, **hashspot;
  header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 2);

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  hnr = START(header);
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = FINISH(hnr);

  prev = NULL;
  hashspot = &(ATerm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->header== header && 
        reinterpret_cast<_ATermAppl*>(cur)->arg[0] == &*arg0 && 
        reinterpret_cast<_ATermAppl*>(cur)->arg[1] == &*arg1)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next = cur->next;
        cur->next = *hashspot;
        *hashspot = cur;
      }
      return reinterpret_cast<_ATermAppl*>(cur);
    }
    prev = cur;
    cur = cur->next;
  }

  cur = AT_allocate(TERM_SIZE_APPL(2));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->header = header;
  AFun::increase_reference_count<true>(sym.number());
  arg0->reference_count++;
  reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
  arg1->reference_count++;
  reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;

  cur->next = ATerm::hashtable[hnr];
  ATerm::hashtable[hnr] = cur;

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl3(AFun &sym, ATerm &arg0, &arg1, &arg2) */

/**
 * Create an ATermAppl with one argument.
 */

ATermAppl ATmakeAppl3(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2)
{
  _ATerm *cur;
  header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 3);

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  hnr = START(header);
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->header!=header ||
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] != &*arg0 ||
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] != &*arg1 ||
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] != &*arg2))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(3));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    AFun::increase_reference_count<true>(sym.number());
    arg0->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] = &*arg2;

    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl4(AFun &sym, ATerm &arg0, &arg1, &arg2, &a3) */

/**
 * Create an ATermAppl with four arguments.
 */

ATermAppl ATmakeAppl4(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2, const ATerm &arg3)
{
  _ATerm* cur;
  header_type header;
  HashNumber hnr;

  header = APPL_HEADER(sym.number());

  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  CHECK_TERM(&*arg3);
  CHECK_ARITY(ATgetArity(sym), 4);

  hnr = START(header);
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = COMBINE(hnr, HN(&*arg3));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->header!=header ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[0] != &*arg0 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[1] != &*arg1 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[2] != &*arg2 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[3] != &*arg3))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(4));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    AFun::increase_reference_count<true>(sym.number());
    arg0->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[3] = &*arg3;

    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl5(AFun &sym, ATerm &arg0, &arg1, &arg2, &a3, &a4) */

/**
 * Create an ATermAppl with five arguments.
 */

ATermAppl ATmakeAppl5(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                                       const ATerm &arg3, const ATerm &arg4)
{
  _ATerm *cur;
  header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 5);
  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  CHECK_TERM(&*arg3);


  hnr = START(header);
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = COMBINE(hnr, HN(&*arg3));
  hnr = COMBINE(hnr, HN(&*arg4));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->header!=header ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[0] != &*arg0 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[1] != &*arg1 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[2] != &*arg2 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[3] != &*arg3 ||
     reinterpret_cast<_ATermAppl*>(cur)->arg[4] != &*arg4))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(5));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    AFun::increase_reference_count<true>(sym.number());
    arg0->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[3] = &*arg3;
    arg4->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[4] = &*arg4;

    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeAppl6(AFun &sym, ATerm &arg0, &arg1, &arg2, &a3, &a4, &a5) */

/**
 * Create an ATermAppl with six arguments.
 */

ATermAppl ATmakeAppl6(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                                      const ATerm &arg3, const ATerm &arg4, const ATerm &arg5)
{
  _ATerm* cur;
  header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  CHECK_ARITY(ATgetArity(sym), 6);
  CHECK_TERM(&*arg0);
  CHECK_TERM(&*arg1);
  CHECK_TERM(&*arg2);
  CHECK_TERM(&*arg3);
  CHECK_TERM(&*arg4);
  CHECK_TERM(&*arg5);

  hnr = START(header);
  hnr = COMBINE(hnr, HN(&*arg0));
  hnr = COMBINE(hnr, HN(&*arg1));
  hnr = COMBINE(hnr, HN(&*arg2));
  hnr = COMBINE(hnr, HN(&*arg3));
  hnr = COMBINE(hnr, HN(&*arg4));
  hnr = COMBINE(hnr, HN(&*arg5));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->header!=header ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[0] != &*arg0 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[1] != &*arg1 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[2] != &*arg2 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[3] != &*arg3 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[4] != &*arg4 ||
  reinterpret_cast<_ATermAppl*>(cur)->arg[5] != &*arg5))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(6));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    AFun::increase_reference_count<true>(sym.number());
    arg0->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[0] = &*arg0;
    arg1->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[1] = &*arg1;
    arg2->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[2] = &*arg2;
    arg3->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[3] = &*arg3;
    arg4->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[4] = &*arg4;
    arg5->reference_count++;
    reinterpret_cast<_ATermAppl*>(cur)->arg[5] = &*arg5;

    cur->next =ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeApplList(AFun &sym, ATermList &args) */

/**
 * Build a function application from a symbol and a list of arguments.
 */

ATermAppl ATmakeApplList(const AFun &sym, const ATermList &args)
{
  _ATerm *cur;
  size_t arity = ATgetArity(sym);
  bool found;
  // header_type header = APPL_HEADER(arity > MAX_INLINE_ARITY ?  MAX_INLINE_ARITY+1 : arity, sym.number());
  header_type header = APPL_HEADER(sym.number());
  HashNumber hnr;

  assert(arity == ATgetLength(args));

  _ATermList* argptr = &*args;
  hnr = START(header);
  for (size_t i=0; i<arity; i++)
  {
    CHECK_TERM(&*argptr->head);
    hnr = COMBINE(hnr, HN(&*(argptr->head)));
    argptr = argptr->tail;
  }
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->header==header)
    {
      found = true;
      argptr = &*args;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<_ATermAppl*>(cur)->arg[i] != argptr->head)
        {
          found = false;
          break;
        }
        argptr = argptr->tail;
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(arity));

    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    AFun::increase_reference_count<true>(sym.number());
    argptr = &*args;
    for (size_t i=0; i<arity; i++)
    {
      argptr->head->reference_count++;
      reinterpret_cast<_ATermAppl*>(cur)->arg[i] = argptr->head;
      argptr = argptr->tail;
    }
    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */
/*{{{  ATermAppl ATmakeApplArray(AFun &sym, ATerm args[]) */

/**
 * Build a function application from a symbol and an array of arguments.
 */

ATermAppl ATmakeApplArray(const AFun &sym, const ATerm args[])
{
  _ATerm* cur;
  size_t arity = ATgetArity(sym);
  bool found;
  HashNumber hnr;
  // header_type header = APPL_HEADER(arity > MAX_INLINE_ARITY ?  MAX_INLINE_ARITY+1 : arity, sym.number());
  header_type header = APPL_HEADER(sym.number());

  hnr = START(header);
  for (size_t i=0; i<arity; i++)
  {
    CHECK_TERM(&*args[i]);
    hnr = COMBINE(hnr, (HashNumber)&*args[i]);
  }
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->header==header)
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<_ATermAppl*>(cur)->arg[i] != &*args[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(arity));

    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    AFun::increase_reference_count<true>(sym.number());
    for (size_t i=0; i<arity; i++)
    {
      args[i]->reference_count++;
      reinterpret_cast<_ATermAppl*>(cur)->arg[i] = &*args[i];
    }
    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */

/*{{{  ATermInt ATmakeInt(int val) */

/**
 * Create an ATermInt
 */

ATermInt ATmakeInt(const int val)
{
  _ATerm* cur;
  /* The following emulates the encoding trick that is also used in the definition
   * of ATermInt. Not using a union here leads to incorrect hashing results.
   */
  union
  {
    int value;
    MachineWord reserved;
  } _val;

  _val.reserved = 0;
  _val.value = val;

  header_type header = INT_HEADER;
  HashNumber hnr;

  hnr = START(header);
  hnr = COMBINE(hnr, HN(_val.reserved));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->header!=header || (reinterpret_cast<_ATermInt*>(cur)->value != _val.value)))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_INT);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    reinterpret_cast<_ATermInt*>(cur)->reserved = _val.reserved;
    reinterpret_cast<_ATermInt*>(cur)->value = _val.value;

    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  assert((hnr & table_mask) == (hash_number(cur, TERM_SIZE_INT) & table_mask));

  return reinterpret_cast<_ATermInt*>(cur);
}

/*}}}  */

/*{{{  ATermList ATmakeList1(ATerm el) */

/**
 * Build a list with one element.
 */

ATermList ATmakeList1(const ATerm &el)
{
  return ATinsert(ATempty,el);
}

/*}}}  */
/*{{{  ATermList ATinsert(ATermList tail, ATerm el) */

/**
 * Insert an element at the front of a list.
 */

ATermList ATinsert(const ATermList &tail, const ATerm &el)
{
  // size_t curLength = GET_LENGTH(tail->header);
  // size_t newLength;
  header_type header;
  HashNumber hnr;
  _ATerm* cur;

  /* If length exceeds the maximum length that can be stored in the header,
     store MAX_LENGTH-1 in the header. ATgetLength will then count the length of the
     list instead of using on the header
  */
  /* if (curLength >= MAX_LENGTH-1)
  {
    newLength = MAX_LENGTH-1;
  }
  else
  {
    newLength = curLength+1;
  } */

  header = LIST_HEADER();


  assert(ATgetType(tail) == AT_LIST);

  hnr = START(header);
  hnr = COMBINE(hnr, HN(&*el));
  hnr = COMBINE(hnr, HN(&*tail));
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur && (cur->header!=header
                      || reinterpret_cast<_ATermList*>(cur)->head != &*el
                      || reinterpret_cast<_ATermList*>(cur)->tail != &*tail))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_LIST);
    /* Hashtable might be resized, so delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = header;
    el->reference_count++;
    reinterpret_cast<_ATermList*>(cur)->head = &*el;
    tail->reference_count++;
    reinterpret_cast<_ATermList*>(cur)->tail = &*tail;
    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermList*>(cur);
}

/*}}}  */

/*{{{  ATermAppl ATsetArgument(ATermAppl appl, ATerm arg, size_t n) */

/**
 * Change one argument of an application.
 */

ATermAppl ATsetArgument(const ATermAppl &appl, const ATerm &arg, const size_t n)
{
  const size_t sym_number = ATgetAFun(appl);
  
  bool found;

  size_t arity = ATgetArity(sym_number);
  assert(n < arity);

  HashNumber hnr = START(appl->header);
  for (size_t i=0; i<arity; i++)
  {
    if (i!=n)
    {
      hnr = COMBINE(hnr, (HashNumber)(appl->arg[i]));
    }
    else
    {
      hnr = COMBINE(hnr, (HashNumber)&*arg);
    }
  }

  hnr = FINISH(hnr);

  _ATerm *cur = ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->header==appl->header)
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (i!=n)
        {
          if (reinterpret_cast<_ATermAppl*>(cur)->arg[i]!=(&*appl)->arg[i])
          {
            found = false;
            break;
          }
        }
        else
        {
          if (reinterpret_cast<_ATermAppl*>(cur)->arg[i]!=&*arg)
          {
            found = false;
            break;
          }
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->header = appl->header;
    AFun::increase_reference_count<true>(sym_number);
    for (size_t i=0; i<arity; i++)
    {
      if (i!=n)
      {
        reinterpret_cast<_ATermAppl*>(cur)->arg[i] = appl->arg[i];
      }
      else
      {
        reinterpret_cast<_ATermAppl*>(cur)->arg[i] = &*arg;
      }
      reinterpret_cast<_ATermAppl*>(cur)->arg[i]->reference_count++;
    }
    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */

/*{{{  ATbool ATisValidTerm(ATerm term) */

/**
 * Determine if a given term is valid.
 */

bool AT_isValidTerm(const _ATerm *term)
{
  Block* cur;
  // header_type header;
  bool inblock = false;
  size_t idx = ADDR_TO_BLOCK_IDX(term);
  // size_t type;
  ptrdiff_t offset = 0;

  assert(block_table[idx].first_after == block_table[(idx+1)%BLOCK_TABLE_SIZE].first_before);

  for (cur=block_table[idx].first_after; cur; cur=cur->next_after)
  {
    if (cur->size)
    {
      assert(cur->next_before == cur->next_after);
      offset  = ((char*)term) - (char*)(&cur->data);
      if (offset >= 0 && offset < (ptrdiff_t)(BLOCK_SIZE * sizeof(header_type)))
      {
        inblock = true;
        break;
      }
    }
  }

  if (!inblock)
  {
    for (cur=block_table[idx].first_before; cur; cur=cur->next_before)
    {
      if (cur->size)
      {
        assert(cur->next_before == cur->next_after);
        offset  = ((char*)term) - ((char*)&cur->data);
        if (offset >= 0 && offset < (ptrdiff_t)(BLOCK_SIZE * sizeof(header_type)))
        {
          inblock = true;
          break;
        }
      }
    }
  }

  if (!inblock)
  {
    return false;
  }

  /* Check if we point to the start of a term. Pointers inside terms
     are not allowed.
     */

  if (offset % (cur->size*sizeof(header_type)))
  {
    return false;
  }

  /* header = term->header;

  type = GET_TYPE(header); */

  /* The only possibility left for an invalid term is AT_FREE */
  // return (term->type() != AT_FREE) && (term->type() != AT_SYMBOL);
  return term->reference_count!=0;
} 

/*}}}  */

/**
 * Determine if a given term is valid.
 */


/*{{{  ATerm AT_isInsideValidTerm(ATerm term) */

/* ATerm AT_isInsideValidTerm(ATerm term)
{
  Block* cur;
  TermInfo* ti;
  bool inblock = false;
  size_t idx = ADDR_TO_BLOCK_IDX(&*term);
  size_t type;

  assert(block_table[idx].first_after == block_table[(idx+1)%BLOCK_TABLE_SIZE].first_before);

  for (cur=block_table[idx].first_after; cur; cur=cur->next_after)
  {
    header_type* end;
    if (cur->size)
    {
      assert(cur->next_before == cur->next_after);
      ti = &terminfo[cur->size];

      if (cur != ti->at_block)
      {
        end = cur->end;
      }
      else
      {
        assert(ti->at_block != NULL);
        end = ti->top_at_blocks;
      }

      if (cur->data <= (header_type*)&*term && (header_type*)&*term < end)
      {
        inblock = true;
        break;
      }
    }
  }

  if (!inblock)
  {
    for (cur=block_table[idx].first_before; cur; cur=cur->next_before)
    {
      header_type* end;
      if (cur->size)
      {
        ti = &terminfo[cur->size];
        assert(cur->next_before == cur->next_after);

        if (cur != ti->at_block)
        {
          end = cur->end;
        }
        else
        {
          assert(ti->at_block != NULL);
          end = ti->top_at_blocks;
        }

        if (cur->data <= (header_type*)&*term && (header_type*)&*term < end)
        {
          inblock = true;
          break;
        }
      }
    }
  }

  if (!inblock)
  {
    return ATerm();
  }


  term = (_ATerm*)(((char*)&*term)-(((char*)&*term-((char*)cur->data))%(cur->size*sizeof(header_type))));

  type = GET_TYPE(term->header);

  / * The only possibility left for an invalid term is AT_FREE * /
  return (((type == AT_FREE) || (type == AT_SYMBOL)) ? ATerm() : term);
} */

/*}}}  */

/*{{{  size_t AT_inAnyFreeList(ATerm t) */

/**
 * Check if a term is in any free list.
 */

size_t AT_inAnyFreeList(const _ATerm *t)
{
  for (size_t i=MIN_TERM_SIZE; i<maxTermSize; i++)
  {
    _ATerm* cur = terminfo[i].at_freelist;

    while (cur)
    {
      if (cur == t)
      {
        return i;
      }
      cur = cur->next;
    }
  }
  return 0;
}

/*}}}  */

} // namespace aterm

