// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SHUFFLE_H_INCLUDED

#include <algorithm>
#include <vector>
#include <cstdlib>

/* Randomly shuffle the given vector.  This is similar to std::random_shuffle
   on a vector,  except that this uses rand() while the random source for
   std::random_shuffle is unspecified. */
template<class T>
static void shuffle_vector(std::vector<T> &v)
{
    size_t n = v.size();
    for (size_t i = 0; i < n; ++i)
    {
        std::swap(v[i], v[i + rand()%(n - i)]);
    }
}

#endif /* ndef SHUFFLE_H_INCLUDED */
