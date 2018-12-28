// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_UNORDERED_SET_ITERATOR_H
#define MCRL2_UTILITIES_UNORDERED_SET_ITERATOR_H

#include "mcrl2/utilities/spinlock.h"
#include "mcrl2/utilities/detail/bucket_list.h"

#include <vector>

namespace mcrl2
{
namespace utilities
{

/// \brief An iterator over all elements in the unordered set.
template<typename Key, typename Bucket, typename Allocator, bool Constant = false>
class unordered_set_iterator : std::iterator_traits<Key>
{
public:
  using tag = std::input_iterator_tag;

  using bucket_it = typename std::conditional<Constant,
    typename std::vector<Bucket>::const_iterator,
    typename std::vector<Bucket>::iterator>::type;
  using key_it_type = typename std::conditional<Constant,
    typename Bucket::const_iterator,
    typename Bucket::iterator>::type;

  /// \brief Construct an iterator over all keys passed in this bucket and all remaining buckets.
  unordered_set_iterator(bucket_it it, bucket_it end, key_it_type before_it, key_it_type key) :
    m_key_before_it(before_it), m_key_it(key), m_bucket_it(it), m_bucket_end(end)
  {
    goto_next_bucket();
  }

  /// \brief Construct an iterator over all keys passed in this bucket and all remaining buckets.
  unordered_set_iterator(bucket_it it, bucket_it end, key_it_type key) :
    m_key_it(key), m_bucket_it(it), m_bucket_end(end)
  {
    m_key_before_it = (*m_bucket_it).before_begin();
    goto_next_bucket();
  }

  /// \brief Construct the end iterator
  unordered_set_iterator(bucket_it it) :
    m_bucket_it(it)
  {}

  /// \brief Construct an iterator over the keys in a bucket.
  unordered_set_iterator(key_it_type it) :
    m_key_it(it)
  {}

  unordered_set_iterator& operator++()
  {
    ++m_key_it;
    ++m_key_before_it;
    goto_next_bucket();
    return *this;
  }

  template<bool _Constant = Constant>
  typename std::enable_if<!_Constant, Key&>::type operator*()
  {
    return *m_key_it;
  }

  template<bool _Constant = Constant>
  typename std::enable_if<_Constant, const Key&>::type operator*() const
  {
    return *m_key_it;
  }

  bool operator!=(const unordered_set_iterator& other)
  {
    return m_key_it != other.m_key_it || m_bucket_it != other.m_bucket_it;
  }

  /// \returns A reference to the before key iterator.
  key_it_type& key_before_it() { return m_key_before_it; }

  /// \returns A reference to the key iterator.
  key_it_type& key_it() { return m_key_it; }

  bucket_it& get_bucket_it() { return m_bucket_it; }

  Bucket& bucket() { return *m_bucket_it; }

private:
  /// \brief Iterate to the next non-empty bucket.
  void goto_next_bucket()
  {
    // Find the first bucket that is not empty.
    while(!(m_key_it != key_it_type::EndIterator))
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
    assert(m_bucket_it == m_bucket_end || m_key_it != key_it_type::EndIterator);
  }

  key_it_type m_key_before_it;
  key_it_type m_key_it; // Invariant: m_key_it != EndIterator.
  bucket_it m_bucket_it;
  bucket_it m_bucket_end;
};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_UNORDERED_SET_ITERATOR_H
