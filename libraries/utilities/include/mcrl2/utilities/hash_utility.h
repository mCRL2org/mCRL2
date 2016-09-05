// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief This file contains a specialisation for hashes on pairs.
 *        This is not a part of the standard, although it ought to be part.
 *        Once it has become a part of the standard, this can be removed.
 * \author Jan Friso Groote
 */

#ifndef MCRL2_UTILITIES_HASH_UTILITY_H
#define MCRL2_UTILITIES_HASH_UTILITY_H

#include <vector>

namespace mcrl2
{

namespace utilities
{

namespace detail
{

inline size_t hash_combine(const size_t h1, const size_t h2)
{
  // This hash function is inpired by boost.
  return h1 ^  (h2 + 0x9e3779b9 + (h1<<6) + (h1>>2));
}


} //end namespace detail
} //end namespace utilities
} //end namespace mcrl2

namespace std
{

template <class X>
struct hash< std::vector < X > >
{
  std::size_t operator()(const std::vector < X >& v) const
  {
    hash<X> hasher;
    // This follows boost.
    size_t hash=0;
    for(const X& x: v)
    {
      hash ^=  hasher(x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
    }
    return hash;
  }
};

} // namespace std


#endif // MCRL2_UTILITIES_HASH_UTILITY_H


