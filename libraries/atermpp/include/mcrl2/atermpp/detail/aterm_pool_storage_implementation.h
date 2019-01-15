// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once
#ifndef MCRL2_ATERMPP_ATERM_POOL_STORAGE_IMPLEMENTION_H
#define MCRL2_ATERMPP_ATERM_POOL_STORAGE_IMPLEMENTION_H

#include "mcrl2/atermpp/detail/aterm_pool_storage.h"
#include "mcrl2/atermpp/detail/aterm_pool.h"

#include "mcrl2/atermpp/detail/aterm_appl.h"
#include "mcrl2/utilities/unused.h"
#include "mcrl2/utilities/logger.h"

#include <algorithm>
#include <assert.h>
#include <cstring>

namespace atermpp
{
namespace detail
{

/// \brief Construct the proxy where its arguments are given by applying the converter
///        to each element in the iterator.
template<std::size_t N,
         typename InputIterator,
         typename TermConverter,
         typename std::enable_if<is_iterator<InputIterator>::value, void>::type* = nullptr>
static std::array<unprotected_aterm, N> construct_arguments(InputIterator it, TermConverter converter)
{
  // Copy the arguments into this array. Doesn't change any reference count, because they are unprotected terms.
  std::array<unprotected_aterm, N> arguments;
  for (size_t i = 0; i < N; ++i)
  {
    arguments[i] = std::forward<TermConverter>(converter)(*it);
    ++it;
  }

  return arguments;
}

#define ATERM_POOL_STORAGE_TEMPLATES template<typename Element, typename Hash, typename Equals, std::size_t N, bool ThreadSafe>
#define ATERM_POOL_STORAGE aterm_pool_storage<Element, Hash, Equals, N, ThreadSafe>

ATERM_POOL_STORAGE_TEMPLATES
ATERM_POOL_STORAGE::aterm_pool_storage(aterm_pool& pool) :
  m_pool(pool),
  m_term_set(1 << 14)
{}

ATERM_POOL_STORAGE_TEMPLATES
void ATERM_POOL_STORAGE::add_creation_hook(atermpp::function_symbol sym, term_callback callback)
{
  // The code handling the hooks is currently assuming that there is only one
  // hook per function symbol. If more hooks are allowed, then this code
  // should be changed.
  for (const auto& hook : m_creation_hooks)
  {
    mcrl2::utilities::mcrl2_unused(hook);
    assert(hook.first != sym);
  }
  m_creation_hooks.emplace_back(sym, callback);
}

ATERM_POOL_STORAGE_TEMPLATES
void ATERM_POOL_STORAGE::add_deletion_hook(atermpp::function_symbol sym, term_callback callback)
{
  // See the comments at add_creation_hook.
  for (const auto& hook : m_deletion_hooks)
  {
    mcrl2::utilities::mcrl2_unused(hook);
    assert(hook.first != sym);
  }
  m_deletion_hooks.emplace_back(sym, callback);
}

ATERM_POOL_STORAGE_TEMPLATES
aterm ATERM_POOL_STORAGE::create_int(std::size_t value)
{
  return insert(value);
}

ATERM_POOL_STORAGE_TEMPLATES
aterm ATERM_POOL_STORAGE::create_term(const atermpp::function_symbol& symbol)
{
  return insert(symbol);
}

ATERM_POOL_STORAGE_TEMPLATES
template<class ...Terms>
aterm ATERM_POOL_STORAGE::create_appl(const function_symbol& sym, const Terms&... arguments)
{
  return insert(sym, arguments...);
}

ATERM_POOL_STORAGE_TEMPLATES
template<typename ForwardIterator>
aterm ATERM_POOL_STORAGE::create_appl_iterator(const function_symbol& symbol,
                                        ForwardIterator begin,
                                        ForwardIterator)
{
  return insert(symbol, begin);
}

ATERM_POOL_STORAGE_TEMPLATES
template<typename InputIterator, typename TermConverter>
aterm ATERM_POOL_STORAGE::create_appl_iterator(const function_symbol& symbol,
                                        TermConverter converter,
                                        InputIterator begin,
                                        InputIterator)
{
  std::array<unprotected_aterm, N> arguments = construct_arguments<N>(begin, converter);
  return insert(symbol, arguments);
}

ATERM_POOL_STORAGE_TEMPLATES
template<typename ForwardIterator>
aterm ATERM_POOL_STORAGE::create_appl_dynamic(const function_symbol& symbol,
                                        ForwardIterator begin,
                                        ForwardIterator)
{
  return insert(symbol, begin);
}

ATERM_POOL_STORAGE_TEMPLATES
template<typename InputIterator,
          typename TermConverter>
aterm ATERM_POOL_STORAGE::create_appl_dynamic(const function_symbol& symbol,
                                        TermConverter converter,
                                        InputIterator it,
                                        InputIterator)
{
  // Construct a proxy object on the stack.
  unprotected_aterm* arguments = MCRL2_SPECIFIC_STACK_ALLOCATOR(unprotected_aterm, symbol.arity());
  for (std::size_t i = 0; i < symbol.arity(); ++i)
  {
    arguments[i] = converter(*it);
    ++it;
  }

  // Find or create a new term and return it.
  return insert(symbol, arguments);
}

ATERM_POOL_STORAGE_TEMPLATES
void ATERM_POOL_STORAGE::print_performance_stats(const char* identifier) const
{
  if (EnableTermHashtableMetrics)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "g_term_pool(" << identifier << ") hashtable:\n";
    m_term_set.print_performance_statistics();
  }

  if (EnableGarbageCollectionMetrics && m_erasedBlocks > 0)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "g_term_pool(" << identifier << "): Consolidate removed " << m_erasedBlocks << " blocks.\n";
  }

  if (EnableTermCreationMetrics)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "g_term_pool(" << identifier << "): insert() found "
                                << m_term_hits
                                << " out of "
                                << m_term_creates
                                << " times ("
                                << static_cast<double>(m_term_hits) / static_cast<double>(m_term_creates) * 100
                                << " %)\n";
  }
}


ATERM_POOL_STORAGE_TEMPLATES
void ATERM_POOL_STORAGE::mark()
{
  for (Element& term : m_term_set)
  {
    // If a term is marked its arguments have been marked as well.
    if (term.is_reachable() && !term.is_marked())
    {
      // Mark all terms (and their subterms) that are reachable, i.e the root set.
      mark_term(term);
    }
  }
}

ATERM_POOL_STORAGE_TEMPLATES
void ATERM_POOL_STORAGE::sweep()
{
  // Iterate over all terms and removes the ones that are marked.
  for (auto it = m_term_set.begin(); it != m_term_set.end(); )
  {
    Element& term = *it;

    if (!term.is_reachable())
    {
      it = destroy(it);
    }
    else
    {
      // Reset terms that have been marked.
      if (term.is_marked())
      {
        term.reset();
      }
      ++it;
    }
  }

  // Clean up unnecessary blocks.
  //m_erasedBlocks = m_term_set.allocator().consolidate();
}

/// PRIVATE FUNCTIONS

ATERM_POOL_STORAGE_TEMPLATES
void ATERM_POOL_STORAGE::call_creation_hook(aterm term)
{
  for (auto& pair : m_creation_hooks)
  {
    if (pair.first == term.function())
    {
      pair.second(term);
    }
  }
}

ATERM_POOL_STORAGE_TEMPLATES
void ATERM_POOL_STORAGE::call_deletion_hook(_aterm* term)
{
  for (auto& pair : m_deletion_hooks)
  {
    if (pair.first == term->function())
    {
      // Create a temporary unprotected term to upcast to an actual term.
      unprotected_aterm actual_term(term);
      pair.second(static_cast<const aterm&>(actual_term));
    }
  }
}


ATERM_POOL_STORAGE_TEMPLATES
bool ATERM_POOL_STORAGE::verify_mark()
{
  // Check for consistency that if a term is reachable its arguments are as well.
  for (Element& term : m_term_set)
  {
    if (term.is_reachable() && term.function().arity() > 0)
    {
       _term_appl& ta = reinterpret_cast<_term_appl&>(term);
       for (std::size_t i = 0; i < ta.function().arity(); ++i)
       {
         assert(detail::address(ta.arg(i))->is_reachable());
       }
    }
  }
  return true;
}

ATERM_POOL_STORAGE_TEMPLATES
bool ATERM_POOL_STORAGE::verify_sweep()
{
  // Check that no argument was removed from a reachable term.
  for (Element& term : m_term_set)
  {
    (void)term;
    assert(verify_term(term));
  }
  return true;
}

/// Private definitions

ATERM_POOL_STORAGE_TEMPLATES
typename ATERM_POOL_STORAGE::iterator ATERM_POOL_STORAGE::destroy(iterator it)
{
  // Store the term temporarily to be able to deallocate it after removing it from the set.
  Element& term = *it;
  assert(!term.is_reachable());

  // Trigger the deletion hook before the term is actually destroyed.
  call_deletion_hook(&term);

  // Remove them from the hash table, will also destroy terms with fixed arity.
  return m_term_set.erase(it);
}

ATERM_POOL_STORAGE_TEMPLATES
template<typename ...Args>
aterm ATERM_POOL_STORAGE::insert(Args&&... args)
{
  // Moving this existence check out of emplace matters for performance.
  auto it = m_term_set.find(args...);
  if (it != m_term_set.end())
  {
    return aterm(&(*it));
  }
  else
  {
    return emplace(args...);
  }
}

ATERM_POOL_STORAGE_TEMPLATES
template<typename ...Args>
aterm ATERM_POOL_STORAGE::emplace(Args&&... args)
{
  m_pool.trigger_collection();

  aterm term(&m_term_set.emplace(args...));
  call_creation_hook(term);

  return term;
}

ATERM_POOL_STORAGE_TEMPLATES
constexpr bool ATERM_POOL_STORAGE::is_dynamic_storage() const
{
  return N == DynamicNumberOfArguments;
}

ATERM_POOL_STORAGE_TEMPLATES
void ATERM_POOL_STORAGE::mark_term(_aterm& root)
{
  // Do not use the stack, because this might run out of stack memory for large lists.
  todo.push(&root);

  // Mark the term depth-first to reduce the maximum todo size required.
  while (!todo.empty())
  {
    _aterm& term = *todo.top();
    todo.pop();

    // (Statically) determine the arity of the function application.
    const std::size_t arity = term.function().arity();
    _term_appl& term_appl = reinterpret_cast<_term_appl&>(term);

    for (std::size_t i = 0; i < arity; ++i)
    {
      // Marks all arguments that are not already (marked as) reachable, because the current
      // term is reachable and as such its arguments are reachable as well.
      _aterm& argument = *detail::address(term_appl.arg(i));
      if (!argument.is_reachable())
      {
        // Only mark this term if it current is unreachable, because the marking is applied to
        // the reference counter.
        argument.mark();

        // Add the argument to be explored as well.
        todo.push(&argument);
      }
    }

    // Each argument should be reachable.
    assert(term.is_reachable());
  }
}

ATERM_POOL_STORAGE_TEMPLATES
template<std::size_t Arity>
bool ATERM_POOL_STORAGE::verify_term(_aterm& term)
{
  // Check that a valid function symbol was used and that its arity belongs to this storage.
  assert(term.function().defined());
  assert(Arity == DynamicNumberOfArguments ? true : term.function().arity() == N);

  // Check that all of its arguments are defined.
  if (term.function().arity() > 0)
  {
    _term_appl& ta = reinterpret_cast<_term_appl&>(term);
    for (std::size_t i = 0; i < ta.function().arity(); ++i)
    {
      assert(ta.arg(i).defined());
      verify_term<DynamicNumberOfArguments>(*detail::address(ta.arg(0)));
    }
  }
  return true;
}

#undef ATERM_POOL_STORAGE_TEMPLATES
#undef ATERM_POOL_STORAGE

} // namespace detail
} // namespace atermpp

#endif
