// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_STACK_VECTOR_H
#define MCRL2_UTILITIES_STACK_VECTOR_H

#include "mcrl2/utilities/detail/memory_utility.h"

#include <cstddef>

namespace mcrl2
{
namespace utilities
{

/// \brief Provides the interface of std::array for a portion of allocated memory. Can be used to
///        interface with a portion of memory allocated on the stack, see MCRL2_DECLARE_STACK_VECTOR.
/// \details It default constructs the elements in the constructor and destroys them in its destructor.
template<typename T>
class stack_array
{
public:
  /// \brief The given pointer should be able to hold N element of sizeof(T) bytes.
  stack_array<T>(T* reserved_memory, std::size_t N)
    : m_reserved_memory(reserved_memory),
      m_size(N)
  {
    for (T& element : *this)
    {
      new (&element) T();
    }
  }

  ~stack_array()
  {
    for (T& element : *this)
    {
      element.~T();
    }
  }

  T* begin() { return m_reserved_memory; }
  const T* begin() const { return m_reserved_memory; }

  T* end() { return m_reserved_memory + m_size; }
  const T* end() const { return m_reserved_memory + m_size; }

  T& operator[](std::size_t index)
  {
    return m_reserved_memory[index];
  }

private:
  T* m_reserved_memory;
  std::size_t m_size;
};

}
}

/// \brief Define a (hopefully) unique name for the underlying reserved stack memory.
#define MCRL2_STACK_ARRAY_NAME(NAME) \
  NAME ## _reserved_stack_memory

/// \brief Allocates an of stack_array<TYPE> with the specified name that stores SIZE elements.
#define MCRL2_DECLARE_STACK_ARRAY(NAME, TYPE, SIZE) \
  TYPE* MCRL2_STACK_ARRAY_NAME(NAME) = MCRL2_SPECIFIC_STACK_ALLOCATOR(TYPE, SIZE); \
  mcrl2::utilities::stack_array<TYPE> NAME (MCRL2_STACK_ARRAY_NAME(NAME), SIZE)

#endif // MCRL2_UTILITIES_STACK_VECTOR_H
