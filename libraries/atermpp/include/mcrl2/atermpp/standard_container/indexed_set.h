// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_ATERMPP_INDEXED_SET_H
#define MCRL2_ATERMPP_INDEXED_SET_H

#include <mcrl2/atermpp/standard_container/deque.h>

#include "mcrl2/utilities/indexed_set.h"

namespace atermpp
{

/// \brief A set that assigns each element an unique index, and protects its internal terms en masse.
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = std::allocator<Key>,
         bool ThreadSafe = false,
         typename KeyTable = std::deque< Key, std::allocator<Key> > >   // This can also be atermpp::deque. 
class indexed_set: public mcrl2::utilities::indexed_set<Key, Hash, Equals, Allocator, ThreadSafe, atermpp::deque< Key, std::allocator<atermpp::detail::reference_aterm<Key> > > >
{};

} // end namespace atermppp

#endif // MCRL2_ATERMPP_INDEXED_SET_H
