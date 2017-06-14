// Author(s): Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_appl_implementation.h 
/// \brief This file contains functions to generate aterm_appl's
///        on the basis of a function symbol and a sequence of arguments. 

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/aterm_appl.h"
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

namespace detail
{

template <class Term, class InputIterator, class ATermConverter>
_aterm* local_term_appl_with_converter(const function_symbol& sym, 
                                       const InputIterator begin, 
                                       const InputIterator end, 
                                       const ATermConverter& convert_to_aterm)
{
  const std::size_t arity = sym.arity();

  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = function_symbol_hasher(sym);
  
  aterm* temporary_args = MCRL2_SPECIFIC_STACK_ALLOCATOR(aterm, arity);
  std::size_t j=0;
  for (InputIterator i=begin; i!=end; ++i, ++j)
  {
    new (&(temporary_args[j])) Term(convert_to_aterm(*i));
    CHECK_TERM(temporary_args[j]);
    hnr = COMBINE(hnr, temporary_args[j]);
  }
  assert(j==arity); 


  _aterm* cur = detail::aterm_hashtable[hnr&  detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      for (std::size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[i] != temporary_args[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        for(std::size_t i=0; i<arity; ++i)
        {
          temporary_args[i].~aterm();
        }
        return cur;
      }
    }
    cur = cur->next();
  }

  assert(cur==nullptr);
  detail::_aterm* new_term = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));

  // We copy the content of the temporary_args, without destruction/construction and adapting the reference counts.
  for(std::size_t i=0; i<arity; ++i)
  {
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(new_term))->arg[i])) _aterm*(detail::address(temporary_args[i]));
  }
  new (&const_cast<detail::_aterm*>(const_cast<detail::_aterm*>(new_term))->function()) function_symbol(sym);

  // Apply the table mask after allocate_term, which may resize the table.
  insert_in_hashtable(new_term,hnr&  detail::aterm_table_mask);
  call_creation_hook(new_term);

  return new_term;
}

template <class Term, class ForwardIterator>
_aterm* local_term_appl(const function_symbol& sym, const ForwardIterator begin, const ForwardIterator end)
{
  const std::size_t arity = sym.arity();
  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = function_symbol_hasher(sym); 
  
  aterm* temporary_args=MCRL2_SPECIFIC_STACK_ALLOCATOR(aterm, arity);
  std::size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    new (&(temporary_args[j])) Term(*i);
    CHECK_TERM(temporary_args[j]);
    hnr = COMBINE(hnr, temporary_args[j]);
  }
  assert(j==arity);

  _aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      for (std::size_t i=0; i<arity; ++i)
      {
        if (reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[i] != temporary_args[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        for(std::size_t i=0; i<arity; ++i)
        {
          temporary_args[i].~aterm();
        }
        return cur;
      }
    }
    cur = cur->next();
  }

  assert(cur==nullptr);
  _aterm* new_term = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));

  // We copy the content of the temporary_args, without destruction/construction and adapting the reference counts.
  for(std::size_t i=0; i<arity; ++i)
  {
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(new_term))->arg[i])) _aterm*(detail::address(temporary_args[i]));
  }

  new (&const_cast<detail::_aterm*>(const_cast<detail::_aterm*>(new_term))->function()) function_symbol(sym);

  // Apply the table_mask after applying allocate_term, which may resize the hash_table.
  insert_in_hashtable(new_term,hnr & detail::aterm_table_mask);
  call_creation_hook(new_term);
  
  return new_term;
}

template <class Term>
_aterm* term_appl1(const function_symbol& sym, const Term& arg0)
{
  assert(sym.arity()==1);
  CHECK_TERM(arg0);

  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = COMBINE(function_symbol_hasher(sym), arg0);

  _aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if ((sym==cur->function()) &&
         reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[0] == arg0)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(1));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;

  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
_aterm* term_appl2(const function_symbol& sym, const Term& arg0, const Term& arg1)
{
  assert(sym.arity()==2);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = COMBINE(COMBINE(function_symbol_hasher(sym), arg0),arg1);

  _aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[1] == arg1)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(2));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
_aterm* term_appl3(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2)
{

  assert(sym.arity()==3);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = COMBINE(COMBINE(COMBINE(function_symbol_hasher(sym), arg0),arg1),arg2);

  _aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[2] == arg2)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(3));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
_aterm *term_appl4(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2, const Term& arg3)
{
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  assert(sym.arity()==4);

  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = COMBINE(COMBINE(COMBINE(COMBINE(function_symbol_hasher(sym), arg0), arg1), arg2), arg3);

  _aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[2] == arg2 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[3] == arg3)
    {
      return cur;
    }
    cur = cur->next();
  }

  assert(!cur);

  cur = detail::allocate_term(TERM_SIZE_APPL(4));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[3])) Term(arg3);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
_aterm* term_appl5(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2,
                                      const Term& arg3, const Term& arg4)
{

  assert(sym.arity()==5);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);

  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(function_symbol_hasher(sym), arg0), arg1), arg2), arg3), arg4);

  _aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[2] == arg2 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[3] == arg3 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[4] == arg4)
    {
      return cur;
    }
    cur = cur->next();
  }

  assert(!cur);

  cur = detail::allocate_term(TERM_SIZE_APPL(5));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[3])) Term(arg3);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[4])) Term(arg4);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
_aterm *term_appl6(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2,
                                      const Term& arg3, const Term& arg4, const Term& arg5)
{
  assert(sym.arity()==6);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);
  CHECK_TERM(arg5);

  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(function_symbol_hasher(sym), arg0), arg1), arg2), arg3), arg4), arg5);

  _aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[2] == arg2 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[3] == arg3 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[4] == arg4 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[5] == arg5)
    {
      return cur;
    }
    cur = cur->next();
  }

  assert(!cur);

  cur = detail::allocate_term(TERM_SIZE_APPL(6));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;

  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[3])) Term(arg3);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[4])) Term(arg4);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[5])) Term(arg5);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}

template <class Term>
_aterm* term_appl7(const function_symbol& sym, const Term& arg0, const Term& arg1, const Term& arg2,
                                      const Term& arg3, const Term& arg4, const Term& arg5, const Term& arg6)
{
  assert(sym.arity()==7);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);
  CHECK_TERM(arg5);
  CHECK_TERM(arg6);

  const std::hash<function_symbol> function_symbol_hasher;
  std::size_t hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(function_symbol_hasher(sym), arg0), arg1), arg2), arg3), arg4), arg5), arg6);

  _aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[2] == arg2 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[3] == arg3 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[4] == arg4 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[5] == arg5 &&
        reinterpret_cast<_aterm_appl<Term>*>(cur)->arg[6] == arg6)
    {
      return cur;
    }
    cur = cur->next();
  }

  assert(!cur);

  cur = detail::allocate_term(TERM_SIZE_APPL(7));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;

  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[2])) Term(arg2);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[3])) Term(arg3);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[4])) Term(arg4);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[5])) Term(arg5);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(const_cast<detail::_aterm*>(cur))->arg[6])) Term(arg6);

  insert_in_hashtable(cur,hnr);

  call_creation_hook(cur);

  return cur;
}
} //namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H
