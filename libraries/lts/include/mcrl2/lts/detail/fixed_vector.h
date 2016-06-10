// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
/// std::vector that guarantees that its iterators remain valid.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef FIXED_VECTOR_H
#define FIXED_VECTOR_H

#include <vector>
#include <cassert>

namespace mcrl2
{
namespace lts
{
namespace detail
{
namespace bisim_gjkw
{

template <class T>
class fixed_vector : public std::vector<T>
{
public:
    typedef typename std::vector<T>::value_type value_type;
    typedef typename std::vector<T>::size_type size_type;
    typedef typename std::vector<T>::iterator iterator;

    explicit fixed_vector()  :std::vector<T>()  {  }
    explicit fixed_vector(size_type n, const value_type& val = value_type())
        :std::vector<T>(n, val)
    {  }
    template <class InputIterator>
    fixed_vector (InputIterator first, InputIterator last)
        :std::vector<T>(first, last)
    {  }
    fixed_vector(const std::vector<T>& x)  :std::vector<T>(x)  {  }

    fixed_vector& operator=(const std::vector<T>& x)
    {
        assert(std::vector<T>::empty());
        std::vector<T>::operator=(x);
        return *this;
    }
    void resize(size_type n, value_type val = value_type())
    {
        assert(std::vector<T>::empty() || n <= std::vector<T>::capacity());
        std::vector<T>::resize(n, val);
    }
    void reserve(size_type n)
    {
        assert(std::vector<T>::empty());
        std::vector<T>::reserve(n);
    }
    void push_back(const value_type& val)
    {
        assert(std::vector<T>::empty() ||
                        std::vector<T>::size() < std::vector<T>::capacity());
        std::vector<T>::push_back(val);
    }
    iterator insert(iterator position, const value_type& val)
    {
        assert(std::vector<T>::empty() ||
                        std::vector<T>::size() < std::vector<T>::capacity());
        std::vector<T>::insert(position, val);
    }
    void insert(iterator position, size_type n, const value_type& val)
    {
        assert(std::vector<T>::empty() ||
                    std::vector<T>::size() + n <= std::vector<T>::capacity());
        std::vector<T>::insert(position, n, val);
    }
    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last)
    {
        assert(std::vector<T>::empty());
        std::vector<T>::insert(position, first, last);
    }
    void swap(std::vector<T>&)  {  assert(0);  }
};

} // end namespace bisim_gjkw
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

template <class T, class Alloc>
void swap(mcrl2::lts::detail::bisim_gjkw::fixed_vector<T>&,
                                                        std::vector<T,Alloc>&)
{
    assert(0);
}
template <class T, class Alloc>
void swap(std::vector<T,Alloc>&,
                            mcrl2::lts::detail::bisim_gjkw::fixed_vector<T>&)
{
    assert(0);
}

#endif // #ifndef FIXED_VECTOR_H
