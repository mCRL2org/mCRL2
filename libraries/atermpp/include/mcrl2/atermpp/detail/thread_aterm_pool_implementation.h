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

function_symbol thread_aterm_pool::create_function_symbol(const std::string& name, const std::size_t arity, const bool check_for_registered_functions)
{
  lock_shared();
  function_symbol symbol = m_pool.create_function_symbol(name, arity, check_for_registered_functions);
  unlock_shared();
  return symbol;
}

function_symbol thread_aterm_pool::create_function_symbol(std::string&& name, const std::size_t arity, const bool check_for_registered_functions)
{
  lock_shared();
  function_symbol symbol = m_pool.create_function_symbol(std::forward<std::string>(name), arity, check_for_registered_functions);
  unlock_shared();
  return symbol;
}

void thread_aterm_pool::create_int(aterm& term, size_t val)
{
  lock_shared();
  bool added = m_pool.create_int(term, val);
  unlock_shared();
  if (added) { m_pool.created_term(m_lock_depth == 0, this); }
}

void thread_aterm_pool::create_term(aterm& term, const atermpp::function_symbol& sym)
{
  lock_shared();
  bool added = m_pool.create_term(term, sym);
  unlock_shared();
  if (added) { m_pool.created_term(m_lock_depth == 0, this); }
}

template<class ...Terms>
void thread_aterm_pool::create_appl(aterm& term, const function_symbol& sym, const Terms&... arguments)
{
  lock_shared();
  bool added = m_pool.create_appl(term, sym, arguments...);
  unlock_shared();
  if (added) { m_pool.created_term(m_lock_depth == 0, this); }
}

template<class Term, class INDEX_TYPE, class ...Terms>
void thread_aterm_pool::create_appl_index(aterm& term, const function_symbol& sym, const Terms&... arguments)
{
  lock_shared();

  std::array<unprotected_aterm, sizeof...(arguments)> argument_array;
  store_in_argument_array(argument_array, arguments...);

  bool added;
  if constexpr (sizeof...(arguments)==1)
  {
    /* Code below is more elegant than succeeding code, but it unnecessarily copies and protects a term.
       m_pool.create_int(term, atermpp::detail::index_traits<Term, INDEX_TYPE, 1>::
           insert(static_cast<INDEX_TYPE>(static_cast<aterm>(address(argument_array[0]))))); */
    m_pool.create_int(term, 
                      atermpp::detail::index_traits<Term, INDEX_TYPE, 1>::
                               insert(*reinterpret_cast<INDEX_TYPE*>(&(argument_array[0]))));
    added = m_pool.create_appl(term, sym, argument_array[0], term);
  }
  else
  {
    /* Code below is more elegant than the actual succeeding code, but it unnecessarily creates an extra pair, which is costly. 
       m_pool.create_int(
         term,
         atermpp::detail::index_traits<Term, INDEX_TYPE, 2>::
           insert(std::make_pair(static_cast<typename INDEX_TYPE::first_type>(static_cast<aterm>(address(argument_array[0]))),
                                 static_cast<typename INDEX_TYPE::second_type>(static_cast<aterm>(address(argument_array[1])))))); */
    m_pool.create_int(term,
                      atermpp::detail::index_traits<Term, INDEX_TYPE, 2>::
                               insert(*reinterpret_cast<INDEX_TYPE*>(&argument_array[0])));
    added = m_pool.create_appl(term, sym, argument_array[0], argument_array[1], term);
  }

  unlock_shared();

  if (added) { m_pool.created_term(m_lock_depth == 0, this); }
}

template<typename InputIterator>
void thread_aterm_pool::create_appl_dynamic(aterm& term,
                            const function_symbol& sym,
                            InputIterator begin,
                            InputIterator end)
{
  lock_shared();  
  bool added = m_pool.create_appl_dynamic(term, sym, begin, end);
  unlock_shared();
  
  if (added) { m_pool.created_term(m_lock_depth == 0, this); }
}

template<typename InputIterator, typename ATermConverter>
void thread_aterm_pool::create_appl_dynamic(aterm& term,
                            const function_symbol& sym,
                            ATermConverter convert_to_aterm,
                            InputIterator begin,
                            InputIterator end)
{
  lock_shared();
  bool added = m_pool.create_appl_dynamic(term, sym, convert_to_aterm, begin, end);
  unlock_shared();

  if (added) { m_pool.created_term(m_lock_depth == 0, this); }
}

void thread_aterm_pool::register_variable(aterm* variable)
{
  if constexpr (EnableVariableRegistrationMetrics) { ++m_variable_insertions; }

  lock_shared();
    
  /* Resizing of the protection set should not interfere with garbage collection and rehashing */
  if (m_variables->must_resize())
  {
    m_variables->resize();
  }

  auto [it, inserted] = m_variables->insert(variable);

  // The variable must be inserted.
  assert(inserted);
  mcrl2::utilities::mcrl2_unused(it);
  mcrl2::utilities::mcrl2_unused(inserted);

  unlock_shared();
}

void thread_aterm_pool::deregister_variable(aterm* variable)
{
  lock_shared();
  m_variables->erase(variable);
  unlock_shared();
}

void thread_aterm_pool::register_container(_aterm_container* container)
{
  if constexpr (EnableVariableRegistrationMetrics) { ++m_container_insertions; }

  lock_shared();

  if (m_containers->must_resize())
  {
    m_containers->resize();
  }
  auto [it, inserted] = m_containers->insert(container);

  // The container must be inserted.
  assert(inserted);
  mcrl2::utilities::mcrl2_unused(it);
  mcrl2::utilities::mcrl2_unused(inserted);
  unlock_shared();
}

void thread_aterm_pool::deregister_container(_aterm_container* container)
{
  lock_shared();
  m_containers->erase(container);
  unlock_shared();
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

  for (const _aterm_container* container : *m_containers)
  {
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

bool thread_aterm_pool::is_busy() const
{
  return m_busy_flag.load();
}

void thread_aterm_pool::lock_shared()
{
  if (GlobalThreadSafe && m_lock_depth == 0)
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

  ++m_lock_depth;
}

/// The function lock_shared that does not access thread local variables directly.
/// Instead it uses the necessary variables through explicit access of pointers stored
/// in rewriters. This is faster, but may be removed in due time when access to thread
/// local variables is sped up. 
void inline lock_shared(std::atomic<bool>* busy_flag,
                        std::atomic<bool>* forbidden_flag,
                        std::size_t* lock_depth)
{
  assert(busy_flag != nullptr && forbidden_flag != nullptr);
  if (GlobalThreadSafe && *lock_depth == 0)
  {
    assert(!*busy_flag);
    busy_flag->store(true);

    // Wait for the forbidden flag to become false.
    while (forbidden_flag->load())
    {
      *busy_flag = false;
      atermpp::detail::g_thread_term_pool().wait();
      *busy_flag = true;
    }
  }

  *lock_depth = *lock_depth + 1;
}


void thread_aterm_pool::unlock_shared()
{
  assert(m_lock_depth > 0);

  --m_lock_depth;
  if (GlobalThreadSafe && m_lock_depth == 0)
  {
    assert(m_busy_flag);
    m_busy_flag.store(false, std::memory_order_release);
  }
}

/// An alternative to unlock shared access.. 
void inline unlock_shared(std::atomic<bool>* busy_flag,
                          std::size_t* lock_depth)
{
  *lock_depth = *lock_depth - 1;
  if (GlobalThreadSafe && *lock_depth == 0)
  {
    assert(*busy_flag);
    busy_flag->store(false, std::memory_order_release);
  }
}

// Obtain exclusive access to the busy-forbidden lock.
void inline thread_aterm_pool::lock()
{
  m_pool.lock(this);
}

// Release exclusive access to the busy-forbidden lock.
void inline thread_aterm_pool::unlock()
{
  m_pool.unlock();
}


void thread_aterm_pool::wait()
{
  m_pool.wait();
}

void thread_aterm_pool::set_forbidden(bool value)
{
  m_forbidden_flag.store(value);
}

} // namespace detail
} // namespace atermpp

#endif // ATERMPP_DETAIL_ATERM_POOL_IMPLEMENTATION_H
