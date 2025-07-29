// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/fixed_vector.h
///
/// \brief a vector class that cannot be moved while it is not empty
///
/// \details The stuttering equivalence algorithm by Groote / Jansen / Keiren /
/// Wijs is implemented using a large number of iterators that should not be
/// invalidated.  The `fixed_vector` class provides a data structure based on
/// std::vector that guarantees that its iterators remain valid.  All methods
/// that could reallocate the storage (and therefore invalidate the iterators)
/// are disabled, except clear(), which can be called just before destructing
/// the fixed_vector.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef FIXED_VECTOR_H
#define FIXED_VECTOR_H

#include <vector>
#include <cassert>

namespace mcrl2::lts::detail
{

template <class T>
class fixed_vector : public std::vector<T>
// class fixed_vector : private std::vector<T>   The elements of vector should not be made public. However, with this, the 
//                                               destructor of an iterator does not become visible on windows leading to compilation issues. 
{
public:
    // only reveal as much of the interface of std::vector<T> as is needed:
    using typename std::vector<T>::iterator;
    using typename std::vector<T>::const_iterator;
    using typename std::vector<T>::size_type;
    using std::vector<T>::begin;
    using std::vector<T>::cbegin;
    using std::vector<T>::cend;
    using std::vector<T>::end;
    using std::vector<T>::front;
    using std::vector<T>::back;
    using std::vector<T>::size;
    using std::vector<T>::clear;
    using std::vector<T>::empty;
    using std::vector<T>::data;

    // define pointer access to what is basically &*end().
    // the compiler will optimize the addition away.
          T* data_end()         {  return data() + size();  }
    const T* data_end()  const  {  return data() + size();  }
    const T* data_cend() const  {  return data() + size();  }

    explicit fixed_vector(size_type n)  :std::vector<T>(n)  {  }
    explicit fixed_vector(size_type n, T init)  :std::vector<T>(n, init)  {  }

#ifdef NDEBUG
    using std::vector<T>::operator[];
#else
    // operator[] calls std::vector<T>::at because the latter checks bounds.
    T& operator[](size_type n)  {  return std::vector<T>::at(n);  }
    const T& operator[](size_type n) const  {  return std::vector<T>::at(n);  }
#endif
};

} // end namespace detail
// end namespace lts
// end namespace mcrl2

#endif // #ifndef FIXED_VECTOR_H
