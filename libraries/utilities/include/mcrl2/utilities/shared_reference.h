// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_SHARED_REFERENCE_H_
#define MCRL2_UTILITIES_SHARED_REFERENCE_H_

#include <assert.h>
#include <atomic>
#include <utility>

namespace mcrl2
{
namespace utilities
{

/// \brief Enable to count the number of reference count changes.
constexpr static bool EnableReferenceCountMetrics = false;

/// \brief Stores a reference count that can be incremented and decremented.
/// \details The template variable is such that reference_count_changes corresponds
///          to the amount of times the reference count of type T changed.
template<typename T, bool ThreadSafe = false>
class shared_reference_counted
{
public:
  shared_reference_counted()
    : m_reference_count(0)
  {}

  /// \brief Obtain the reference count.
  std::size_t reference_count() const
  {
    return m_reference_count;
  }

  /// \brief Increment the reference count by one.
  void increment_reference_count()
  {
    ++m_reference_count;
    increment_reference_count_changes();
  }

  /// \brief Decrement the reference count by one.
  void decrement_reference_count()
  {
    --m_reference_count;
    increment_reference_count_changes();
  }

  /// \brief Obtain the number of times that this reference count has changed.
  static std::size_t& reference_count_changes()
  {
    static std::size_t g_reference_count_changes;
    return g_reference_count_changes;
  }

  /// \brief Increment the number of reference count changes.
  static void increment_reference_count_changes()
  {
    if (EnableReferenceCountStatistics)
    {
      ++reference_count_changes();
    }
  }

protected:
  using SizeType = typename std::conditional<ThreadSafe, std::atomic<std::size_t>, std::size_t>::type;

  SizeType m_reference_count;
};

/// \brief A reference counted reference to a shared_reference_counted object.
/// \details Similar to a shared_ptr except that reference counts are only atomic when
///          thread safety is desired and that it stores the reference count in the
///          inherited object.
template<typename T>
class shared_reference
{
public:

  /// \brief The default constructor.
  shared_reference() noexcept
    : m_reference(nullptr)
  {}

  /// \brief Takes ownership of the passed reference, which means
  ///        that its reference count is incremented.
  shared_reference(T* reference) noexcept
    : m_reference(reference)
  {
    assert(defined());
    m_reference->increment_reference_count();
  }

  /// \brief Takes a reference, but do not changes the reference counter.
  shared_reference(T* reference, bool) noexcept
    : m_reference(reference)
  {}

  /// \brief Copy constructor.
  shared_reference(const shared_reference<T>& other) noexcept
    : m_reference(other.m_reference)
  {
    if (defined())
    {
      m_reference->increment_reference_count();
    }
  }

  /// \brief Move constructor.
  shared_reference(shared_reference<T>&& other) noexcept
    : m_reference(other.m_reference)
  {
    other.m_reference = nullptr;
  }

  /// \brief Copy assignment constructor.
  shared_reference<T>& operator=(const shared_reference<T>& other) noexcept
  {
    // Increment first to prevent the same reference from getting a reference count of zero temporarily.
    if (other.defined())
    {
      other.m_reference->increment_reference_count();
    }

    // Decrement the reference from the reference that is currently referred to.
    if (defined())
    {
      m_reference->decrement_reference_count();
    }

    m_reference=other.m_reference;
    return *this;
  }

  /// \brief Move assignment constructor.
  shared_reference<T>& operator=(shared_reference<T>&& other) noexcept
  {
    if (defined())
    {
      m_reference->decrement_reference_count();
    }

    m_reference = other.m_reference;
    other.m_reference = nullptr;
    return *this;
  }

  /// \brief Check whether the shared_reference has a valid reference.
  bool defined() const
  {
    return m_reference != nullptr;
  }

  bool operator ==(const shared_reference<T>& other) const noexcept
  {
    return m_reference == other.m_reference;
  }

  bool operator <(const shared_reference<T>& other) const noexcept
  {
    return m_reference < other.m_reference;
  }

  // Comparison operators follow from equivalence and less than.
  bool operator !=(const shared_reference<T>& other) const noexcept
  {
    return m_reference != other.m_reference;
  }

  bool operator <=(const shared_reference<T>& other) const noexcept
  {
    return m_reference <= other.m_reference;
  }

  bool operator >(const shared_reference<T>& other) const noexcept
  {
    return m_reference > other.m_reference;
  }

  bool operator >=(const shared_reference& other) const noexcept
  {
    return m_reference >= other.m_reference;
  }

  T* operator->() const noexcept
  {
    assert(defined());
    return m_reference;
  }

  T* get() const noexcept
  {
    assert(defined());
    return m_reference;
  }

  T& operator*() const noexcept
  {
    assert(defined());
    return *m_reference;
  }

  /// \brief Swaps *this with the other shared reference.
  /// \details Prevents the change of any reference count adaptations
  void swap(shared_reference<T>& other)
  {
    using std::swap;
    swap(m_reference, other.m_reference);
  }

private:
  T* m_reference;
};

} // namespace utilities
} // namespace mcrl2

namespace std
{

template<typename T>
void swap(mcrl2::utilities::shared_reference<T>& a, mcrl2::utilities::shared_reference<T>& b) noexcept
{
  a.swap(b);
}

} // namespace std

#endif // MCRL2_UTILITIES_SHARED_REFERENCE_H_
