// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once
#ifndef ATERMPP_DETAIL_ATERM_POOL_IMPLEMENTATION_H
#define ATERMPP_DETAIL_ATERM_POOL_IMPLEMENTATION_H

#include "aterm_pool.h"
#include "mcrl2/utilities/logger.h"

#include <chrono>

namespace atermpp
{
namespace detail
{

aterm_pool::aterm_pool() :
  m_int_storage(*this),
  m_appl_storage(
    *this,
    *this,
    *this,
    *this,
    *this,
    *this,
    *this,
    *this
  ),
  m_appl_dynamic_storage(*this)
{
  m_countUntilCollection = capacity();
}

aterm_pool::~aterm_pool()
{
  print_performance_statistics();
}

void aterm_pool::add_creation_hook(function_symbol sym, term_callback callback)
{
  const std::size_t arity = sym.arity();

  switch (arity)
  {
  case 0:
  {
    if (sym == get_symbol_pool().as_int())
    {
      m_int_storage.add_creation_hook(sym, callback);
    }
    else
    {
      return std::get<0>(m_appl_storage).add_creation_hook(sym, callback);
    }
    break;
  }
  case 1:
    std::get<1>(m_appl_storage).add_creation_hook(sym, callback);
    break;
  case 2:
    std::get<2>(m_appl_storage).add_creation_hook(sym, callback);
    break;
  case 3:
    std::get<3>(m_appl_storage).add_creation_hook(sym, callback);
    break;
  case 4:
    std::get<4>(m_appl_storage).add_creation_hook(sym, callback);
    break;
  case 5:
    std::get<5>(m_appl_storage).add_creation_hook(sym, callback);
    break;
  case 6:
    std::get<6>(m_appl_storage).add_creation_hook(sym, callback);
    break;
  case 7:
    std::get<7>(m_appl_storage).add_creation_hook(sym, callback);
    break;
  default:
    m_appl_dynamic_storage.add_creation_hook(sym, callback);
  }
}

void aterm_pool::add_deletion_hook(function_symbol sym, term_callback callback)
{
  const std::size_t arity = sym.arity();

  switch (arity)
  {
  case 0:
  {
    if (sym == get_symbol_pool().as_int())
    {
      m_int_storage.add_deletion_hook(sym, callback);
    }
    else
    {
      std::get<0>(m_appl_storage).add_deletion_hook(sym, callback);
    }
  }
    break;
  case 1:
    std::get<1>(m_appl_storage).add_deletion_hook(sym, callback);
    break;
  case 2:
    std::get<2>(m_appl_storage).add_deletion_hook(sym, callback);
    break;
  case 3:
    std::get<3>(m_appl_storage).add_deletion_hook(sym, callback);
    break;
  case 4:
    std::get<4>(m_appl_storage).add_deletion_hook(sym, callback);
    break;
  case 5:
    std::get<5>(m_appl_storage).add_deletion_hook(sym, callback);
    break;
  case 6:
    std::get<6>(m_appl_storage).add_deletion_hook(sym, callback);
    break;
  case 7:
    std::get<7>(m_appl_storage).add_deletion_hook(sym, callback);
    break;
  default:
    m_appl_dynamic_storage.add_deletion_hook(sym, callback);
  }
}

std::size_t aterm_pool::capacity() const noexcept
{
  // Determine the total number of terms in any storage.
  return m_int_storage.capacity()
    + std::get<0>(m_appl_storage).capacity()
    + std::get<1>(m_appl_storage).capacity()
    + std::get<2>(m_appl_storage).capacity()
    + std::get<3>(m_appl_storage).capacity()
    + std::get<4>(m_appl_storage).capacity()
    + std::get<5>(m_appl_storage).capacity()
    + std::get<6>(m_appl_storage).capacity()
    + std::get<7>(m_appl_storage).capacity()
    + m_appl_dynamic_storage.capacity();
}

void aterm_pool::trigger_collection()
{
  // Defer garbage collection when it happens too often.
  if (m_countUntilCollection > 0 || !EnableGarbageCollection)
  {
    --m_countUntilCollection;
  }
  else if (m_countUntilCollection == 0)
  {
    collect();

    // Use some heuristics to determine when the next collection is called.
    m_countUntilCollection = capacity();
  }
}

void aterm_pool::collect()
{
  auto timestamp = std::chrono::system_clock::now();

  if (m_creation_depth > 0)
  {
    m_deferred_garbage_collect = true;
    return;
  }

  m_deferred_garbage_collect = false;
  std::size_t old_size = size();

  // Marks all terms that are reachable via any reachable term to
  // not be garbage collected.
  // For integer and terms without arguments the marking is not needed, because
  // they do not have arguments that might have to be marked.
  std::get<1>(m_appl_storage).mark();
  std::get<2>(m_appl_storage).mark();
  std::get<3>(m_appl_storage).mark();
  std::get<4>(m_appl_storage).mark();
  std::get<5>(m_appl_storage).mark();
  std::get<6>(m_appl_storage).mark();
  std::get<7>(m_appl_storage).mark();
  m_appl_dynamic_storage.mark();

  assert(std::get<0>(m_appl_storage).verify_mark());
  assert(std::get<1>(m_appl_storage).verify_mark());
  assert(std::get<2>(m_appl_storage).verify_mark());
  assert(std::get<3>(m_appl_storage).verify_mark());
  assert(std::get<4>(m_appl_storage).verify_mark());
  assert(std::get<5>(m_appl_storage).verify_mark());
  assert(std::get<6>(m_appl_storage).verify_mark());
  assert(std::get<7>(m_appl_storage).verify_mark());
  assert(m_appl_dynamic_storage.verify_mark());

  // Collect all terms that are not reachable or marked.
  m_int_storage.sweep();
  std::get<0>(m_appl_storage).sweep();
  std::get<1>(m_appl_storage).sweep();
  std::get<2>(m_appl_storage).sweep();
  std::get<3>(m_appl_storage).sweep();
  std::get<4>(m_appl_storage).sweep();
  std::get<5>(m_appl_storage).sweep();
  std::get<6>(m_appl_storage).sweep();
  std::get<7>(m_appl_storage).sweep();
  m_appl_dynamic_storage.sweep();

  // Check that after sweeping the terms are consistent.
  assert(m_int_storage.verify_sweep());
  assert(std::get<0>(m_appl_storage).verify_sweep());
  assert(std::get<1>(m_appl_storage).verify_sweep());
  assert(std::get<2>(m_appl_storage).verify_sweep());
  assert(std::get<3>(m_appl_storage).verify_sweep());
  assert(std::get<4>(m_appl_storage).verify_sweep());
  assert(std::get<5>(m_appl_storage).verify_sweep());
  assert(std::get<6>(m_appl_storage).verify_sweep());
  assert(std::get<7>(m_appl_storage).verify_sweep());
  assert(m_appl_dynamic_storage.verify_sweep());

  // Print some statistics.
  if (EnableGarbageCollectionMetrics)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "g_term_pool(): Garbage collected " << old_size - size() << " terms in "
      << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timestamp).count() << " ms.\n";
    mCRL2log(mcrl2::log::debug, "Performance") << "g_term_pool(): There are " << size() << " terms stored.\n";
  }

  get_symbol_pool().print_performance_stats();
  print_performance_statistics();
}

aterm aterm_pool::create_int(size_t val)
{
  return m_int_storage.create_int(val);
}

aterm aterm_pool::create_term(const atermpp::function_symbol& sym)
{
  return std::get<0>(m_appl_storage).create_term(sym);
}

template<class ...Terms>
aterm aterm_pool::create_appl(const function_symbol& sym, const Terms&... arguments)
{
  return std::get<sizeof...(Terms)>(m_appl_storage).create_appl(sym, arguments...);
}

template<typename ForwardIterator>
aterm aterm_pool::create_appl_dynamic(const function_symbol& sym,
                            ForwardIterator begin,
                            ForwardIterator end)
{
  const std::size_t arity = sym.arity();

  switch(arity)
  {
  case 0:
    return std::get<0>(m_appl_storage).create_term(sym);
    break;
  case 1:
    return std::get<1>(m_appl_storage).template create_appl_iterator<ForwardIterator>(sym, begin, end);
    break;
  case 2:
    return std::get<2>(m_appl_storage).template create_appl_iterator<ForwardIterator>(sym, begin, end);
    break;
  case 3:
    return std::get<3>(m_appl_storage).template create_appl_iterator<ForwardIterator>(sym, begin, end);
    break;
  case 4:
    return std::get<4>(m_appl_storage).template create_appl_iterator<ForwardIterator>(sym, begin, end);
    break;
  case 5:
    return std::get<5>(m_appl_storage).template create_appl_iterator<ForwardIterator>(sym, begin, end);
    break;
  case 6:
    return std::get<6>(m_appl_storage).template create_appl_iterator<ForwardIterator>(sym, begin, end);
    break;
  case 7:
    return std::get<7>(m_appl_storage).template create_appl_iterator<ForwardIterator>(sym, begin, end);
    break;
  default:
    return m_appl_dynamic_storage.create_appl_dynamic(sym, begin, end);
  }
}

template<typename InputIterator, typename ATermConverter>
aterm aterm_pool::create_appl_dynamic(const function_symbol& sym,
                            ATermConverter converter,
                            InputIterator begin,
                            InputIterator end)
{
  ++m_creation_depth;

  const std::size_t arity = sym.arity();
  aterm result;

  switch(arity)
  {
  case 0:
    result = std::get<0>(m_appl_storage).create_term(sym);
    break;
  case 1:
    result = std::get<1>(m_appl_storage).template create_appl_iterator<InputIterator, ATermConverter>(sym, converter, begin, end);
    break;
  case 2:
    result = std::get<2>(m_appl_storage).template create_appl_iterator<InputIterator, ATermConverter>(sym, converter, begin, end);
    break;
  case 3:
    result = std::get<3>(m_appl_storage).template create_appl_iterator<InputIterator, ATermConverter>(sym, converter, begin, end);
    break;
  case 4:
    result = std::get<4>(m_appl_storage).template create_appl_iterator<InputIterator, ATermConverter>(sym, converter, begin, end);
    break;
  case 5:
    result = std::get<5>(m_appl_storage).template create_appl_iterator<InputIterator, ATermConverter>(sym, converter, begin, end);
    break;
  case 6:
    result = std::get<6>(m_appl_storage).template create_appl_iterator<InputIterator, ATermConverter>(sym, converter, begin, end);
    break;
  case 7:
    result = std::get<7>(m_appl_storage).template create_appl_iterator<InputIterator, ATermConverter>(sym, converter, begin, end);
    break;
  default:
    result = m_appl_dynamic_storage.create_appl_dynamic(sym, converter, begin, end);
  }

  --m_creation_depth;

  // Trigger a deferred garbage collection when it was requested and the term has been protected.
  if (m_creation_depth == 0 && m_deferred_garbage_collect)
  {
    if (EnableGarbageCollectionMetrics)
    {
      mCRL2log(mcrl2::log::debug, "Performance") << "g_term_pool(): Deferred garbage collection.\n";
    }
    collect();
  }

  return result;
}

void aterm_pool::print_performance_statistics() const
{
  m_int_storage.print_performance_stats("integral_storage");
  std::get<0>(m_appl_storage).print_performance_stats("term_storage");
  std::get<1>(m_appl_storage).print_performance_stats("function_application_storage_1");
  std::get<2>(m_appl_storage).print_performance_stats("function_application_storage_2");
  std::get<3>(m_appl_storage).print_performance_stats("function_application_storage_3");
  std::get<4>(m_appl_storage).print_performance_stats("function_application_storage_4");
  std::get<5>(m_appl_storage).print_performance_stats("function_application_storage_5");
  std::get<6>(m_appl_storage).print_performance_stats("function_application_storage_6");
  std::get<7>(m_appl_storage).print_performance_stats("function_application_storage_7");

  m_appl_dynamic_storage.print_performance_stats("arbitrary_function_application_storage");

  if (mcrl2::utilities::EnableReferenceCountMetrics)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "g_term_pool(): all reference counts changed " << _aterm::reference_count_changes() << " times.\n";
  }
}

std::size_t aterm_pool::size() const
{
  // Determine the total number of terms in any storage.
  return m_int_storage.size()
    + std::get<0>(m_appl_storage).size()
    + std::get<1>(m_appl_storage).size()
    + std::get<2>(m_appl_storage).size()
    + std::get<3>(m_appl_storage).size()
    + std::get<4>(m_appl_storage).size()
    + std::get<5>(m_appl_storage).size()
    + std::get<6>(m_appl_storage).size()
    + std::get<7>(m_appl_storage).size()
    + m_appl_dynamic_storage.size();
}

} // namespace detail
} // namespace atermpp

#endif // ATERMPP_DETAIL_ATERM_POOL_IMPLEMENTATION_H
