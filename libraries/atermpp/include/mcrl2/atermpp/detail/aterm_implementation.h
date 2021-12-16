// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_ATERM_IMPLEMENTATION_H
#define MCRL2_ATERMPP_ATERM_IMPLEMENTATION_H
#pragma once

#include "mcrl2/atermpp/detail/global_aterm_pool.h"
#include "mcrl2/atermpp/detail/aterm_container.h"

namespace atermpp
{

namespace detail
{
  template<typename T>
  const reference_aterm<T, typename std::enable_if_t<std::is_base_of<aterm, T>::value>>&
    reference_aterm<T, typename std::enable_if_t<std::is_base_of<aterm, T>::value>>::operator=(const unprotected_aterm& other) noexcept
  {
    g_thread_term_pool().lock_shared();
    m_term = address(other);
    g_thread_term_pool().unlock_shared();
    return *this;
  }

  template<typename T>
  const reference_aterm<T, typename std::enable_if_t<std::is_base_of<aterm, T>::value>>&
    reference_aterm<T, typename std::enable_if_t<std::is_base_of<aterm, T>::value>>::operator=(unprotected_aterm&& other) noexcept
  {
    g_thread_term_pool().lock_shared();
    m_term = address(other);
    g_thread_term_pool().unlock_shared();
    return *this;
  }

  template<typename T, typename Allocator>
  void aterm_allocator<T,Allocator>::deallocate(T* p, size_type n)
  {
    g_thread_term_pool().lock_shared();
    m_allocator.deallocate(p, n);
    g_thread_term_pool().unlock_shared();
  }

  _aterm_container::_aterm_container()
  {
    g_thread_term_pool().register_container(this);
  }

  _aterm_container::~_aterm_container()
  {
    g_thread_term_pool().deregister_container(this);
  }

  _aterm_container::_aterm_container(const _aterm_container&)
  { 
    g_thread_term_pool().register_container(this);
  }
  
  _aterm_container::_aterm_container(_aterm_container&&)
  { 
    g_thread_term_pool().register_container(this);
  }
}

inline aterm::aterm() noexcept
{
#ifndef MCRL2_ATERMPP_REFERENCE_COUNTED
  detail::g_thread_term_pool().register_variable(this);
#endif
}

inline aterm::~aterm() noexcept
{
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  decrement_reference_count();
#else
  detail::g_thread_term_pool().deregister_variable(this);
#endif
}

inline aterm::aterm(const detail::_aterm *t) noexcept
{
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  t->increment_reference_count();
#else
  detail::g_thread_term_pool().register_variable(this);
#endif
  m_term = t;
}

inline aterm::aterm(const aterm& other) noexcept
 : unprotected_aterm(other.m_term)
{
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  increment_reference_count();
#else
  detail::g_thread_term_pool().register_variable(this);
#endif
}

inline aterm::aterm(aterm&& other) noexcept
 : unprotected_aterm(other.m_term)
{
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  other.m_term=nullptr;   // This is not needed when using protection sets. 
#else
  detail::g_thread_term_pool().register_variable(this);
#endif
}

inline aterm& aterm::operator=(const aterm& other) noexcept
{
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  // Increment first to prevent the same term from becoming reference zero temporarily.
  other.increment_reference_count();
  // Decrement the reference from the term that is currently referred to.
  decrement_reference_count();
  m_term = other.m_term;
#else
  detail::g_thread_term_pool().lock_shared();
  m_term = other.m_term;
  detail::g_thread_term_pool().unlock_shared();
#endif
  return *this;
}

inline aterm& aterm::assign(const aterm& other,
                            std::atomic<bool>* busy_flag,
                            std::atomic<bool>* forbidden_flag,
                            std::size_t creation_depth) noexcept
{
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  // Increment first to prevent the same term from becoming reference zero temporarily.
  other.increment_reference_count();
  // Decrement the reference from the term that is currently referred to.
  decrement_reference_count();
  m_term = other.m_term;
#else
  detail::lock_shared(busy_flag,forbidden_flag,creation_depth);
  m_term = other.m_term;
  detail::unlock_shared(busy_flag,creation_depth);
#endif
  return *this;
}


inline aterm& aterm::operator=(aterm&& other) noexcept
{
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  std::swap(m_term, other.m_term);
#else
  detail::g_thread_term_pool().lock_shared();
  m_term = other.m_term;    // Using hash set protection it is cheaper just to move the value to the new term.
  detail::g_thread_term_pool().unlock_shared();
#endif
  return *this;
}


} // namespace atermpp

#endif // MCRL2_ATERMPP_TERM_IMPLEMENTATION_H
