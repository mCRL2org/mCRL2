// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_BLOCK_ALLOCATOR_H_
#define MCRL2_UTILITIES_BLOCK_ALLOCATOR_H_

#include "memory_pool.h"

#include <memory>
#include <stddef.h>
#include <utility>

namespace mcrl2
{
namespace utilities
{

/// \brief The block allocator provides the allocator interface for the memory pool class.
///        As such is can be used as an allocator for allocator aware storages.
/// \details Also provides several non-standard allocate functions specifically for the term pool.
template <class T,
          std::size_t ElementsPerBlock = 1024,
          bool ThreadSafe = false>
class block_allocator : public memory_pool<T, ElementsPerBlock, ThreadSafe>
{
public:

  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T value_type;
  using base = std::allocator<T>;

  template <class U>
  struct rebind
  {
      typedef block_allocator<U, ElementsPerBlock, ThreadSafe> other;
  };

  block_allocator() = default;

  /// \details The unused parameter is to make the interface equivalent
  ///          to the allocator.
  pointer allocate(size_type n, std::allocator<void>::const_pointer hint = nullptr)
  {
    if (n != 1 || hint)
    {
      throw std::bad_alloc();
    }

    return memory_pool<T, ElementsPerBlock, ThreadSafe>::allocate();
  }

  /// \details The unused parameter is to make the interface equivalent
  ///          to the allocator.
  void deallocate(pointer p, size_type)
  {
    memory_pool<T, ElementsPerBlock, ThreadSafe>::deallocate(p);
  }

  template<typename... Args>
  void construct(pointer p, Args&&... args)
  {
    new (p) T(std::forward<Args>(args)...);
  }

  // Nonstandard part of the interface.
  template<typename... Args>
  pointer allocate_and_construct(Args&&... args)
  {
    pointer p = allocate(1);
    construct(p, std::forward<Args>(args)...);
    return p;
  }

  // Move assignment and construction is possible.
  block_allocator(block_allocator&&) = default;
  block_allocator& operator=(block_allocator&&) = default;
};

} // namespace utilities
} // namespace mcrl2

#endif
