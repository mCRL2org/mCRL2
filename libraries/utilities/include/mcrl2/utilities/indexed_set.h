// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_INDEXED_SET_H
#define MCRL2_UTILITIES_INDEXED_SET_H

#include <deque>

#include "mcrl2/utilities/unordered_map.h"
#include "mcrl2/utilities/block_allocator.h"

namespace mcrl2
{
namespace utilities
{

// TODO: As it stands Equals, Allocator and ThreadSafe are not used. 
//       Either remove or make useful.
//
/// \brief A set that assigns each element an unique index.
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = std::allocator<Key>,
         bool ThreadSafe = false>
class indexed_set
{
private:
  std::vector<std::size_t> m_hashtable;
  // std::deque < Key, Allocator > m_keys;   The block allocator seems incompatible with a deque. 
  std::deque < Key > m_keys;

  // Two auxiliary functions in this class. 
  std::size_t put_in_hashtable(const Key& key, std::size_t n);
  inline void resize_hashtable();

public:
  
  typedef Key key_type;
  typedef std::size_t size_type;
  typedef std::pair<const key_type, size_type> value_type;
  typedef Equals key_compare;

  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;

  typedef typename std::deque < Key >::iterator iterator;
  typedef typename std::deque < Key >::const_iterator const_iterator;

  typedef typename std::deque < Key >::reverse_iterator reverse_iterator;
  typedef typename std::deque < Key >::const_reverse_iterator const_reverse_iterator;

  typedef std::ptrdiff_t difference_type;
  
  /// \brief Value returned when an element does not exist in the set.
  /// \return Value indicating non existing element, equal to std::numeric_limits<std::size_t>::max(). 
  static const size_type npos = std::numeric_limits<std::size_t>::max();

  // The interface of an unordered_map without the ability to remove individual elements.

  /// \brief Constructor of an empty indexed set. Starts with a hashtable of size 128.
  indexed_set();

  /// \brief Constructor of an empty index set. Starts with a hashtable of the indicated size. 
  /// \parameter initial_hashtable_size The initial size of the hashtable. 
  indexed_set(std::size_t initial_hashtable_size);

  /// \brief Returns a reference to the mapped value.
  /// \detail Returns an invalid value, larger or equal than the size of the indexed set, if there is no element with the given key. 
  size_type index(const Key& key) const;

  /// \brief Returns a reference to the mapped value.
  /// \detail Returns an out_of_range exception if there is no element with the given key. 
  /// \param index The position in the indexed set.
  /// \return The value at position index.
  const key_type& at(const size_type index) const;

  /// \brief Operator that provides a const reference at the position indicated by index.
  /// \param index The position in the indexed set.
  /// \return The value at position index.
  const key_type& operator[](const size_type index) const;

  /// \brief Forward iterator which runs through the elements from the lowest to the largest number.
  /// \detail Complexity is constant per operation. 
  iterator begin() 
  { 
    return m_keys.begin(); 
  }

  /// \brief End of the forward iterator.
  iterator end()
  { 
    return m_keys.end(); 
  }

  /// \brief const_iterator going through the elements in the set numbered from zero upwards. 
  const_iterator cbegin() const
  { 
    return m_keys.cbegin(); 
  }

  /// \brief End of the forward const_iterator. 
  const_iterator cend() const 
  { 
    return m_keys.cend(); 
  }

  /// \brief Reverse iterator going through the elements in the set from the largest to the smallest index. 
  iterator rbegin() 
  { 
    return m_keys.rbegin(); 
  }

  /// \brief End of the reverse iterator. 
  iterator rend()
  { 
    return m_keys.rend(); 
  }

  /// \brief Reverse const_iterator going through the elements from the highest to the lowest numbered element. 
  const_iterator crbegin() const
  { 
    return m_keys.crbegin(); 
  }

  /// \brief End of the reverse const_iterator. 
  const_iterator crend() const 
  { 
    return m_keys.crend(); 
  }

  /// \brief Clears the indexed set by removing all its elements. It is not guaranteed that the memory is released too. 
  void clear();

  // size_type count(const Key& key) const;

  /// \brief Insert a key in the indexed set and return its index. 
  /// \detail If the element was already in the set, the resulting bool is true, and the existing index is returned.
  ///         Otherwise, the key is inserted in the set, and the next available index is assigned to it. 
  /// \param  key The key to be inserted in the set.
  /// \return The index of the key and a boolean indicating whether the element was actually inserted. 
  std::pair<size_type, bool> insert(const Key& key);

  /// \brief Provides an iterator to the stored key in the indexed set.
  /// \param key The key that is sought.
  /// \return An iterator to the key, otherwise end().
  const_iterator find(const Key& key) const;

  /// \brief The number of elements in the indexed set.
  /// \return The number of elements in the indexed set. 
  size_type size() const
  { 
    return m_keys.size();
  }
};

/// \brief A specialization for large indexed sets that uses the block_allocator internally (by default).
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = mcrl2::utilities::block_allocator<Key>,
         bool ThreadSafe = false>
using indexed_set_large = indexed_set<Key, Hash, Equals, Allocator, ThreadSafe>;

} // end namespace utilities
} // end namespace mcrl2

#include "mcrl2/utilities/detail/indexed_set.h"


#endif // MCRL2_UTILITIES_INDEXED_SET_H
