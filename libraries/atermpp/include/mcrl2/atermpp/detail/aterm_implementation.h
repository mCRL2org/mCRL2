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
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    m_term = address(other);
    return *this;
  }

  template<typename T>
  const reference_aterm<T, typename std::enable_if_t<std::is_base_of<aterm, T>::value>>&
    reference_aterm<T, typename std::enable_if_t<std::is_base_of<aterm, T>::value>>::operator=(unprotected_aterm&& other) noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    m_term = address(other);
    return *this;
  }

  template<typename T, typename Allocator>
  void aterm_allocator<T,Allocator>::deallocate(T* p, size_type n)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    m_allocator.deallocate(p, n);
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
  detail::g_thread_term_pool().register_variable(this);
}

inline aterm::~aterm() noexcept
{
  detail::g_thread_term_pool().deregister_variable(this);
}

inline aterm::aterm(const detail::_aterm *t) noexcept
{
  detail::g_thread_term_pool().register_variable(this);
  m_term = t;
}

inline aterm::aterm(const aterm& other) noexcept
 : unprotected_aterm(other.m_term)
{
  detail::g_thread_term_pool().register_variable(this);
}

inline aterm::aterm(aterm&& other) noexcept
 : unprotected_aterm(other.m_term)
{
  detail::g_thread_term_pool().register_variable(this);
}

inline aterm& aterm::operator=(const aterm& other) noexcept
{
  mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
  m_term = other.m_term;
  return *this;
}

inline aterm& aterm::assign(const aterm& other,
  detail::thread_aterm_pool& pool) noexcept
{
  mcrl2::utilities::shared_guard guard = pool.lock_shared();
  m_term = other.m_term;
  return *this;
}


template <bool CHECK_BUSY_FLAG /* =true*/>
inline aterm& aterm::unprotected_assign(const aterm& other) noexcept
{
  if constexpr (mcrl2::utilities::detail::GlobalThreadSafe && CHECK_BUSY_FLAG) assert(detail::g_thread_term_pool().is_shared_locked());
  m_term = other.m_term;
  return *this;
}


inline aterm& aterm::operator=(aterm&& other) noexcept
{
  mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
  // Using hash set protection it is cheaper just to move the value to the new term.
  m_term = other.m_term;    
  return *this;
}


} // namespace atermpp

#endif // MCRL2_ATERMPP_TERM_IMPLEMENTATION_H
