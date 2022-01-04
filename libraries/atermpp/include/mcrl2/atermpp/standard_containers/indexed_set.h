// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_ATERMPP_INDEXED_SET_H
#define MCRL2_ATERMPP_INDEXED_SET_H

#include <mcrl2/atermpp/standard_containers/deque.h>

#include "mcrl2/utilities/indexed_set.h"

namespace atermpp
{

/// \brief A set that assigns each element an unique index, and protects its internal terms en masse.
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = std::allocator<Key>,
         bool ThreadSafe = false,
         typename KeyTable = atermpp::deque<Key > >
class indexed_set: public mcrl2::utilities::indexed_set<Key, Hash, Equals, Allocator, ThreadSafe, KeyTable>
{
  typedef mcrl2::utilities::indexed_set<Key, Hash, Equals, Allocator, ThreadSafe, KeyTable> super;
  
  public:
/// \brief Constructor of an empty indexed set. Starts with a hashtable of size 128.
   indexed_set()
   {}

/// \brief Constructor of an empty indexed set. Starts with a hashtable of size 128.
   indexed_set(std::size_t number_of_threads)
     : super(number_of_threads)
   {}

/// \brief Constructor of an empty index set. Starts with a hashtable of the indicated size. 
/// \param initial_hashtable_size The initial size of the hashtable.
/// \param hash The hash function.
/// \param equals The comparison function for its elements.
  indexed_set(std::size_t number_of_threads,
              std::size_t initial_hashtable_size,
              const typename super::hasher& hash = typename super::hasher(),
              const typename super::key_equal& equals = typename super::key_equal()) 
    : super(number_of_threads, initial_hashtable_size, hash, equals)
  {}
};

} // end namespace atermppp

#endif // MCRL2_ATERMPP_INDEXED_SET_H
