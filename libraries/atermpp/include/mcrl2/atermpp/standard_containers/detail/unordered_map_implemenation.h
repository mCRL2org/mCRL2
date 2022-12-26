// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/atermpp/standard_containers/detail/unordered_map_implemenation.h
/// \brief This file contains an implementation of the hash function 
///        to break circular header dependencies. 

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_MAP_IMPLEMENTATION_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_MAP_IMPLEMENTATION_H

#include "mcrl2/atermpp/standard_containers/unordered_map.h"
#include "mcrl2/atermpp/detail/thread_aterm_pool.h"

namespace atermpp
{
namespace utilities
{
template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  inline void unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::rehash(std::size_t new_size )
  { 
    if constexpr (ThreadSafe) detail::g_thread_term_pool().lock();
    super::rehash(new_size);
    if constexpr (ThreadSafe) detail::g_thread_term_pool().unlock();
  }

} // namespace utilities
} // namespace atermpp

#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_MAP_IMPLEMENTATION_H
