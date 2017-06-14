// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/source/aterm_implementation.cpp
/// \brief This file provides basic routines for garbage collection of aterms.
///        Furthermore, it contains a hooking mechanism that is used to call 
///        auxiliary functions when a term is created or destroyed. 

#include <cctype>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <stdexcept>

#include <set>
#include <cstring>
#include <sstream>
#include <algorithm>


#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/detail/aterm_implementation.h"
#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/aterm_appl.h"


#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace atermpp
{

typedef std::vector<std::pair<const function_symbol*,term_callback> > hook_table;

static hook_table& creation_hooks()
{
  static hook_table creation_hooks;
  return creation_hooks;
}

static hook_table& deletion_hooks()
{
  static hook_table deletion_hooks;
  return deletion_hooks;
}

void add_creation_hook(const function_symbol& sym, term_callback callback)
{
#ifndef NDEBUG
  // The code handling the hooks is currently assuming that there is only one
  // hook per function symbol. If more hooks are allowed, then this code
  // should be changed.
  for (hook_table::const_iterator it = creation_hooks().begin(); it != creation_hooks().end(); ++it)
  {
    assert(it->first != &sym);
  }
#endif
  creation_hooks().push_back(std::make_pair(&sym, callback));
}

void add_deletion_hook(const function_symbol& sym, term_callback callback)
{
#ifndef NDEBUG
  // See the comments at add_creation_hook.
  for (hook_table::const_iterator it = deletion_hooks().begin(); it != deletion_hooks().end(); ++it)
  {
    assert(it->first != &sym);
  }
#endif
  deletion_hooks().push_back(std::make_pair(&sym, callback));
}


// The terms below are initialised in initialise_aterm_administration.
detail::_aterm* aterm::static_undefined_aterm=nullptr;
detail::_aterm* aterm::static_empty_aterm_list=nullptr; 

namespace detail
{

// The hashtables are not vectors to prevent them from being
// destroyed prematurely.

static const std::size_t INITIAL_TERM_TABLE_SIZE = 1<<17;  // Must be a power of 2.
static const std::size_t INITIAL_MAX_TERM_SIZE = 16;

std::size_t aterm_table_size=INITIAL_TERM_TABLE_SIZE;
std::size_t aterm_table_mask=INITIAL_TERM_TABLE_SIZE-1;
_aterm* * aterm_hashtable;

// The following is not a vector to avoid that it is prematurely destroyed.
std::size_t terminfo_size=INITIAL_MAX_TERM_SIZE;
std::size_t garbage_collect_count_down=0;
TermInfo *terminfo;

std::size_t total_nodes_in_hashtable = 0;

void call_creation_hook(detail::_aterm* term)
{
  const function_symbol& sym = term->function();
  for (hook_table::const_iterator it = creation_hooks().begin(); it != creation_hooks().end(); ++it)
  {
    if (*it->first == sym)
    {
      it->second(aterm(term));
    }
  }
}

static void call_deletion_hook(detail::_aterm* term)
{
  const function_symbol& sym = term->function();
  for (hook_table::const_iterator it = deletion_hooks().begin(); it != deletion_hooks().end(); ++it)
  {
    if (*it->first == sym)
    {
      it->second(aterm(term));
    }
  }
}

void free_term_aux(detail::_aterm* t, detail::_aterm*& terms_to_be_removed)
{
  assert(t->reference_count()==0);

  call_deletion_hook(t);

  const function_symbol& f=t->function();
  const std::size_t arity=f.arity();

  const std::size_t size=detail::TERM_SIZE_APPL(arity);

  detail::TermInfo& ti = detail::terminfo[size];
  t->set_reference_count_indicates_in_freelist();
  t->set_next(ti.at_freelist);
  ti.at_freelist = t;

  if (f!=detail::function_adm.AS_INT)
  {
    for(std::size_t i=0; i<arity; ++i)
    {
      aterm& a= reinterpret_cast<detail::_aterm_appl<aterm> *>(t)->arg[i];  
      if  (0==a.decrease_reference_count())
      {
        remove_from_hashtable(a.m_term);
        a.m_term->set_next(terms_to_be_removed);
        terms_to_be_removed=a.m_term;
      }
    }
  }

  f.~function_symbol();
}

/* Remove terms, but do not use the stack, because
 * the stack is not always sufficiently large, esp. if limit stacksize
 * is not set. On OSX the stack can only be 65Mbyte big, which is not enough
 * to remove a large aterm list. */
void free_term(detail::_aterm* t)
{
  detail::_aterm* terms_to_be_removed=t;
  remove_from_hashtable(t);
  t->set_next(nullptr);
  while (terms_to_be_removed!=nullptr)
  {
    detail::_aterm* u=terms_to_be_removed;
    terms_to_be_removed=terms_to_be_removed->next();
    free_term_aux(u,terms_to_be_removed);
  }
}


void resize_aterm_hashtable()
{
  static bool resizing_aterm_hashtable_has_failed=false;
  if (resizing_aterm_hashtable_has_failed)
  {
    // Not increasing the hashtable has only a slight performance penalty,
    // as the hashtables get fuller. But it saves memory, and does not lead
    // to incorrect behaviour.
    return;
  }
  const std::size_t old_size=aterm_table_size;
  aterm_table_size <<=1; // Double the size.
  // Intentionally do not throw the old hashtable away before allocating the new one.
  // It is better when the extra memory is used for blocks of aterms, than for increasing the
  // hashtable.
  _aterm* * new_hashtable=reinterpret_cast<_aterm**>(calloc(aterm_table_size,sizeof(_aterm*)));

  if (new_hashtable==nullptr)
  {
    resizing_aterm_hashtable_has_failed=true;
    mCRL2log(mcrl2::log::warning) << "could not resize hashtable to size " << aterm_table_size << ". ";
    aterm_table_size = old_size;
    return;
  }
  aterm_table_mask = aterm_table_size-1;

  /*  Rehash all old elements */
  for (std::size_t p=0; p<old_size; ++p)
  {
    _aterm* aterm_walker=aterm_hashtable[p];

    while (aterm_walker)
    {
      assert(!aterm_walker->reference_count_indicates_is_in_freelist());
      _aterm* next = aterm_walker->next();
      const std::size_t hnr = hash_number(aterm_walker) & aterm_table_mask;
      aterm_walker->set_next(new_hashtable[hnr]);
      new_hashtable[hnr] = aterm_walker;
      assert(aterm_walker->next()!=aterm_walker);
      aterm_walker = next;
    }
  }
  free(aterm_hashtable);
  aterm_hashtable=new_hashtable;
}

void collect_terms_with_reference_count_0()
{
  // This function puts all with reference count==0 in the freelist, in the reverse order as
  // the sequence of blocks.


  // First put all terms with reference count 0 in the freelist.
  for(std::size_t size=TERM_SIZE; size<terminfo_size; ++size)
  {
    TermInfo& ti=terminfo[size];

    for(Block* b=ti.at_block; b!=nullptr; b=b->next_by_size)
    {
      for(std::size_t *p=b->data; p<b->end; p=p+size)
      {
        _aterm* p1=reinterpret_cast<_aterm*>(p);
        if (p1->reference_count()==0)
        {
          // Put term in freelist, freeing subterms also.
          free_term(p1);
        }
      }
    }
  }

  // Reconstruct the freelists for all terms, freeing empty blocks.
  std::size_t number_of_blocks=0;
  for(std::size_t size=TERM_SIZE; size<terminfo_size; ++size)
  {
    TermInfo& ti=terminfo[size];
    Block* previous_block=nullptr;
    ti.at_freelist=nullptr;
    for(Block* b=ti.at_block; b!=nullptr; )
    {
      Block* next_block=b->next_by_size;
      bool block_is_empty_up_till_now=true;
      _aterm* freelist_of_previous_block=ti.at_freelist;
      for(std::size_t *p=b->data; p<b->end; p=p+size)
      {
        _aterm* p1=reinterpret_cast<_aterm*>(p);
        assert(p1->reference_count()!=0);
        if (p1->reference_count_indicates_is_in_freelist())
        {
          p1->set_next(ti.at_freelist);
          ti.at_freelist=p1;
        }
        else
        {
          block_is_empty_up_till_now=false;
        }
      }

      if (block_is_empty_up_till_now)
      {
        ti.at_freelist=freelist_of_previous_block;
        if (previous_block==nullptr)
        {
          ti.at_block=next_block;
        }
        else
        {
          previous_block->next_by_size=next_block;
        }
        free(b);
      }
      else
      {
        previous_block=b;
        number_of_blocks++;
      }
      b=next_block;
    }
  }
  garbage_collect_count_down=(1+number_of_blocks)*(BLOCK_SIZE/(sizeof(std::size_t)*16));
}

#ifdef MCRL2_CHECK_ATERMPP_CLEANUP
static void check_that_all_objects_are_free()
{
  collect_terms_with_reference_count_0();

  bool result=true;

  for(std::size_t size=TERM_SIZE; size<terminfo_size; ++size)
  {
    const TermInfo& ti=terminfo[size];
    for(Block* b=ti.at_block; b!=NULL; b=b->next_by_size)
    {
      for(std::size_t* p=b->data; p<b->end; p=p+size)
      {
        _aterm* p1=reinterpret_cast<_aterm*>(p);
        if (!p1->reference_count_is_zero() && !p1->reference_count_indicates_is_in_freelist() &&
            ((p1->function()!=function_adm.AS_DEFAULT && p1->function()!=function_adm.AS_EMPTY_LIST) || p1->reference_count()>1))
        {
          mCRL2log(mcrl2::log::error) << "CHECK: Non free term " << p1 << " (size " << size << "). ";
          mCRL2log(mcrl2::log::error) << "Reference count " << p1->reference_count() << ". ";
          mCRL2log(mcrl2::log::error) << "Func: " << p1->function().name() << ". Arity: " << p1->function().arity() << ".\n";
          result=false;
        }

      }
    }
  }
}
#endif

void initialise_aterm_administration()
{
  /* Check for reasonably sized aterm (at least 32 bits, 4 bytes). This check might break on
   * perfectly valid architectures that have char == 2 bytes, and sizeof(header_type) == 2 */
  static_assert(sizeof(std::size_t) == sizeof(aterm*) && sizeof(std::size_t) >= 4,"pointers and std::size_t must be equal and larger than four bytes for the aterm library");

  detail::function_adm.initialise_function_symbols();

  /* Explict initialisation on first use. This first use is when a function symbol is created for
   * the first time, which may be due to the initialisation of a global variable in a .cpp file, or
   * due to the initialisation of a pre-main initialisation of a static variable, which some
   * compilers do. */

  aterm_hashtable=reinterpret_cast<_aterm**>(calloc(aterm_table_size,sizeof(_aterm*)));
  if (aterm_hashtable==nullptr)
  {
    throw std::runtime_error("Out of memory. Cannot create an aterm symbol hashtable.");
  }

  terminfo=reinterpret_cast<TermInfo*>(malloc(terminfo_size*sizeof(TermInfo)));
  if (terminfo==nullptr)
  {
    throw std::runtime_error("Out of memory. Failed to allocate the terminfo array.");
  }

  for(std::size_t i=TERM_SIZE; i<terminfo_size; ++i)
  {
    new (&terminfo[i]) TermInfo();
  }

  /* Check at exit that all function symbols and terms have been cleaned up properly.
   * TODO: on windows it turns out that the reference counts do not reduce to 0. The reason for it
   *       is unclear. It could either be due to an unforeseen sequence of destroying static and
   *       global variables, in relation to the execution of the exit function defined below. Or it
   *       could be that on windows global variables are not properly cleaned up. This requires
   *       further investigation. */
#ifdef MCRL2_CHECK_ATERMPP_CLEANUP
  assert(atexit(check_that_all_objects_are_free) == 0);
#endif

  detail::initialise_function_map_administration();

  aterm::static_undefined_aterm=detail::term_appl0(detail::function_adm.AS_DEFAULT);
  aterm::static_undefined_aterm->increase_reference_count();
  aterm::static_empty_aterm_list=detail::term_appl0(detail::function_adm.AS_EMPTY_LIST);
  aterm::static_empty_aterm_list->increase_reference_count();

}

/* allocate a block of memory to contain terms consisting of `size' objects
 * of type std::size_t or pointer */
void allocate_block(const std::size_t size)
{
  const std::size_t block_header_size=sizeof(struct Block*)+sizeof(std::size_t*);
  std::size_t number_of_terms_in_data_block=(BLOCK_SIZE-block_header_size) / (size*sizeof(std::size_t));
  if (number_of_terms_in_data_block==0) number_of_terms_in_data_block=1; // Take care that there is room for at least one term.

  Block* newblock = (Block*)malloc(block_header_size+number_of_terms_in_data_block*size*sizeof(std::size_t));
  if (newblock == nullptr)
  {
    throw std::runtime_error("Out of memory. Could not allocate a block of memory to store terms.");
  }

  assert(size>=TERM_SIZE);
  assert(size < terminfo_size);
  TermInfo& ti = terminfo[size];

  newblock->end = newblock->data + number_of_terms_in_data_block*size;

  // Put new terms in the block in the freelist.

  for(std::size_t *p=newblock->data; p<newblock->end; p=p+size)
  {
    _aterm* p1=reinterpret_cast<_aterm*>(p);
    p1->set_next(ti.at_freelist);
    ti.at_freelist = p1;
    p1->set_reference_count_indicates_in_freelist(false);
  }

  newblock->next_by_size = ti.at_block;
  ti.at_block = newblock;
  assert(ti.at_block != nullptr);
  assert(ti.at_freelist != nullptr);
}

} // namespace detail

} // namespace atermpp

