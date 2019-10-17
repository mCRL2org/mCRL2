// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_UNORDERED_SET_ITERATOR_H
#define MCRL2_UTILITIES_UNORDERED_SET_ITERATOR_H

#include "mcrl2/utilities/detail/bucket_list.h"

#include <vector>

namespace mcrl2
{
namespace utilities
{

/// \brief An iterator over all elements in the unordered set.
template<typename Key, typename Bucket, typename Allocator>
class unordered_set_iterator : std::iterator_traits<Key>
{
private:
  using bucket_it = typename std::vector<Bucket>::const_iterator;
  using key_it_type = typename Bucket::const_iterator;

public:
  using value_type = Key;
  using reference = const Key&;
  using pointer = const Key*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  unordered_set_iterator() = default;

  unordered_set_iterator& operator++()
  {
    ++m_key_before_it;
    ++m_key_it;
    goto_next_bucket();
    return *this;
  }

  unordered_set_iterator operator++(int)
  {
    unordered_set_iterator copy(*this);
    ++(*this);
    return *copy;
  }

  reference operator*() const
  {
    return *m_key_it;
  }

  pointer operator->() const
  {
    return &(*m_key_it);
  }

  bool operator!=(const unordered_set_iterator& other) const
  {
    return m_key_it != other.m_key_it || m_bucket_it != other.m_bucket_it;
  }

  bool operator==(const unordered_set_iterator& other) const
  {
    return !(*this != other);
  }

  // This is the private interface.

  /// \brief Construct an iterator over all keys passed in this bucket and all remaining buckets.
  unordered_set_iterator(bucket_it it, bucket_it end, key_it_type before_it, key_it_type key) :
    m_bucket_it(it), m_bucket_end(end), m_key_before_it(before_it), m_key_it(key)
  {}

  /// \brief Construct the begin iterator (over all elements).
  unordered_set_iterator(bucket_it it, bucket_it end) :
    m_bucket_it(it), m_bucket_end(end), m_key_before_it((*it).before_begin()), m_key_it((*it).begin())
  {
    goto_next_bucket();
  }

  /// \brief Construct the end iterator
  explicit unordered_set_iterator(bucket_it it) :
    m_bucket_it(it)
  {}

  /// \returns A reference to the before key iterator.
  key_it_type& key_before_it() { return m_key_before_it; }

  /// \returns A reference to the key iterator.
  key_it_type& key_it() { return m_key_it; }

  /// \returns A reference to the bucket iterator.
  bucket_it& get_bucket_it() { return m_bucket_it; }

  /// \brief Iterate to the next non-empty bucket.
  void goto_next_bucket()
  {
    // Find the first bucket that is not empty.
    while(!(m_key_it != detail::EndIterator))
    {
      // Take the next bucket and reset the key iterator.
      ++m_bucket_it;

      if (m_bucket_it != m_bucket_end)
      {
        m_key_it = (*m_bucket_it).begin();
        m_key_before_it = (*m_bucket_it).before_begin();
      }
      else
      {
        // Reached the end of the buckets.
        break;
      }
    }

    // The current bucket contains elements or we are at the end.
    assert(m_bucket_it == m_bucket_end || m_key_it != detail::EndIterator);
  }

private:
  bucket_it m_bucket_it;
  bucket_it m_bucket_end;
  key_it_type m_key_before_it;
  key_it_type m_key_it; // Invariant: m_key_it != EndIterator.
};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_UNORDERED_SET_ITERATOR_H
