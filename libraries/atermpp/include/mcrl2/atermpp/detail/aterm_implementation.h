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

  aterm_container::aterm_container()
  {
    g_thread_term_pool().register_container(this);
  }

  aterm_container::~aterm_container()
  {
    g_thread_term_pool().deregister_container(this);
  }

  aterm_container::aterm_container(const aterm_container&)
  { 
    g_thread_term_pool().register_container(this);
  }
  
  aterm_container::aterm_container(aterm_container&&)
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
#ifndef MCRL2_ATERMPP_REFERENCE_COUNTED
  detail::g_thread_term_pool().register_variable(this);
#endif
  other.m_term=nullptr;
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_TERM_IMPLEMENTATION_H
