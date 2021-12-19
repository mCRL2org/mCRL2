// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ATERMPP_DETAIL_THREAD_ATERM_POOL_IMPLEMENTATION_H
#define ATERMPP_DETAIL_THREAD_ATERM_POOL_IMPLEMENTATION_H
#pragma once

#include <chrono>

#include "thread_aterm_pool.h"
#include "mcrl2/atermpp/detail/index_traits.h"


namespace atermpp
{
namespace detail
{

/// \brief A reference to the thread local term pool storage
thread_aterm_pool& g_thread_term_pool();

function_symbol thread_aterm_pool::create_function_symbol(const std::string& name, const std::size_t arity, const bool check_for_registered_functions)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  function_symbol symbol = m_pool.create_function_symbol(name, arity, check_for_registered_functions);
  if constexpr (GlobalThreadSafe) unlock_shared();
  return symbol;
}

void thread_aterm_pool::create_int(aterm& term, size_t val)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  bool added = m_pool.create_int(term, val);
  if constexpr (GlobalThreadSafe) unlock_shared();
  if (added) { m_pool.created_term(m_creation_depth == 0, this); }
}

void thread_aterm_pool::create_term(aterm& term, const atermpp::function_symbol& sym)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  bool added = m_pool.create_term(term, sym);
  if constexpr (GlobalThreadSafe) unlock_shared();
  if (added) { m_pool.created_term(m_creation_depth == 0, this); }
}

template<class ...Terms>
void thread_aterm_pool::create_appl(aterm& term, const function_symbol& sym, const Terms&... arguments)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  bool added = m_pool.create_appl(term, sym, arguments...);
  if constexpr (GlobalThreadSafe) unlock_shared();
  if (added) { m_pool.created_term(m_creation_depth == 0, this); }
}

template<class Term, class ...Terms>
void thread_aterm_pool::create_appl_index(aterm& term, const function_symbol& sym, const Terms&... arguments)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  ++m_creation_depth;

  static_assert(sizeof...(arguments)==1 || sizeof...(arguments)==2);
  if constexpr (sizeof...(arguments)==1)
  {
    m_pool.create_int(term, atermpp::detail::index_traits<Term, Terms..., 1>::insert(arguments...));
  }
  else
  {
    m_pool.create_int(term, atermpp::detail::index_traits<Term, std::pair<Terms...>, 2>::insert(std::make_pair(arguments...)));
  }
  bool added = m_pool.create_appl(term, sym, arguments..., term);

  --m_creation_depth;
  if constexpr (GlobalThreadSafe) unlock_shared();

  if (added) { m_pool.created_term(m_creation_depth == 0, this); }
}

template<typename InputIterator>
void thread_aterm_pool::create_appl_dynamic(aterm& term,
                            const function_symbol& sym,
                            InputIterator begin,
                            InputIterator end)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  ++m_creation_depth;
  
  bool added = m_pool.create_appl_dynamic(term, sym, begin, end);
  --m_creation_depth;

  if constexpr (GlobalThreadSafe) unlock_shared();
  
  if (added) { m_pool.created_term(m_creation_depth == 0, this); }
}

template<typename InputIterator, typename ATermConverter>
void thread_aterm_pool::create_appl_dynamic(aterm& term,
                            const function_symbol& sym,
                            ATermConverter convert_to_aterm,
                            InputIterator begin,
                            InputIterator end)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  ++m_creation_depth;

  bool added = m_pool.create_appl_dynamic(term, sym, convert_to_aterm, begin, end);
  --m_creation_depth;
  if constexpr (GlobalThreadSafe) unlock_shared();

  if (added) { m_pool.created_term(m_creation_depth == 0, this); }
}

void thread_aterm_pool::register_variable(aterm* variable)
{
  if constexpr (EnableVariableRegistrationMetrics) { ++m_variable_insertions; }

  /* Resizing of the protection set should not interfere with garbage collection and rehashing */
  if (m_variables->must_resize())
  {
    if constexpr (GlobalThreadSafe) lock_shared();
    m_variables->resize();
    if constexpr (GlobalThreadSafe) unlock_shared();
  }

  if constexpr (GlobalThreadSafe) lock_shared();
  auto [it, inserted] = m_variables->insert(variable);

  // The variable must be inserted.
  assert(inserted);
  mcrl2::utilities::mcrl2_unused(it);
  mcrl2::utilities::mcrl2_unused(inserted);

  if constexpr (GlobalThreadSafe) unlock_shared();
}

void thread_aterm_pool::deregister_variable(aterm* variable)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  m_variables->erase(variable);
  if constexpr (GlobalThreadSafe) unlock_shared();
}

void thread_aterm_pool::register_container(_aterm_container* container)
{
  if constexpr (EnableVariableRegistrationMetrics) { ++m_container_insertions; }

  if constexpr (GlobalThreadSafe) lock_shared();
  auto [it, inserted] = m_containers->insert(container);

  // The container must be inserted.
  assert(inserted);
  mcrl2::utilities::mcrl2_unused(it);
  mcrl2::utilities::mcrl2_unused(inserted);
  if constexpr (GlobalThreadSafe) unlock_shared();
}

void thread_aterm_pool::deregister_container(_aterm_container* container)
{
  if constexpr (GlobalThreadSafe) lock_shared();
  m_containers->erase(container);
  if constexpr (GlobalThreadSafe) unlock_shared();
}

void thread_aterm_pool::mark()
{

#ifndef MCRL2_ATERMPP_REFERENCE_COUNTED
  for (const aterm* variable : *m_variables)
  {
    if (variable != nullptr)
    {
      // Mark all terms (and their subterms) that are reachable, i.e the root set.
      _aterm* term = detail::address(*variable);
      if (term != nullptr && !term->is_marked())
      {
        // This variable is not a default term and that term has not been marked.
        mark_term(*term, m_todo);
      }
    }
  }
#endif // NOT MCRL2_ATERMPP_REFERENCE_COUNTED

  for (auto it = m_containers->begin(); it != m_containers->end(); ++it)
  {
    const _aterm_container* container = *it;

    if (container != nullptr)
    {
      // The container marks the contained terms itself.
      container->mark(m_todo);
    }
  }
}

void thread_aterm_pool::print_local_performance_statistics() const
{
  if constexpr (EnableVariableRegistrationMetrics)
  {
    mCRL2log(mcrl2::log::info, "Performance") << "thread_aterm_pool: " << m_variables->size() << " variables in root set (" << m_variable_insertions << " total insertions)"
                                              << " and " << m_containers->size() << " containers in root set (" << m_container_insertions << " total insertions).\n";
  }
}

void thread_aterm_pool::wait_for_busy() const
{
  while (m_busy_flag.load());
}

void thread_aterm_pool::lock_shared()
{
  if (GlobalThreadSafe && m_creation_depth == 0)
  {
    assert(!m_busy_flag);
    m_busy_flag.store(true);

    // Wait for the forbidden flag to become false.
    while (m_forbidden_flag.load())
    {
      m_busy_flag = false;
      m_pool.wait();
      m_busy_flag = true;
    }
  }
}

/// The function lock_shared that does not access thread local variables directly.
/// Instead it uses the necessary variables through explicit access of pointers stored
/// in rewriters. This is faster, but may be removed in due time when access to thread
/// local variables is sped up. 
void inline lock_shared(std::atomic<bool>* busy_flag,
                        std::atomic<bool>* forbidden_flag,
                        std::size_t creation_depth)
{
  assert(busy_flag != nullptr && forbidden_flag != nullptr);
  if (GlobalThreadSafe && creation_depth == 0)
  {
    assert(!*busy_flag);
    busy_flag->store(true);

    // Wait for the forbidden flag to become false.
    if (forbidden_flag->load())
    {
      *busy_flag = false;
      atermpp::detail::g_thread_term_pool().lock_shared();
    }
  }
}


void thread_aterm_pool::unlock_shared()
{
  if (GlobalThreadSafe && m_creation_depth == 0)
  {
    assert(m_busy_flag);
    m_busy_flag.store(false, std::memory_order_release);
  }
}

/// An alternative to unlock shared access.. 
void inline unlock_shared(std::atomic<bool>* busy_flag,
                   std::size_t creation_depth)
{
  if (GlobalThreadSafe && creation_depth == 0)
  {
    assert(*busy_flag);
    busy_flag->store(false, std::memory_order_release);
  }
}


void thread_aterm_pool::set_forbidden(bool value)
{
  m_forbidden_flag.store(value);
}

} // namespace detail
} // namespace atermpp

#endif // ATERMPP_DETAIL_ATERM_POOL_IMPLEMENTATION_H
