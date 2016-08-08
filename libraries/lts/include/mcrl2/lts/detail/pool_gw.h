// Author(s): Jan Friso Groote, Anton Wijs, David N. Jansen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/pool_gw.h


#ifndef REPOSITORY_GW
#define REPOSITORY_GW
#include <string>
#include <deque>
#include <cassert>
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

/// \brief pool to keep elements of type T
/// \details The data structure below is intended to recycle elements of a
/// given type, such that they do not have to be malloced and deleted, which is
/// time consuming. There is a list of free elements to which deallocated
/// elements are stored, and from which allocated members are taken when they
/// are needed and available.
///
/// It is assumed that the elements have a pointer to a next element in the
/// pool, and that they have functions rep_next(), set_rep_next() to get and
/// change its value. Also, once an element from the pool is reused, the method
/// rep_init() is called to initialise it (meant to be a replacement of a
/// constructor).
///
/// These lists are non copiable, nor can they be constructed from each other.
template < class T>
class pool
{
  protected:
    /// \brief deque storing the elements
		std::deque<T> pool_storage;
    /// \brief head of forward list of free elements
		T* pool_storage_free_elements;
	
  public:
	  typedef typename std::deque<T>::iterator iterator;
		typedef typename std::deque<T>::const_iterator const_iterator;

    /// constructor
	  pool()
		  : pool_storage_free_elements(nullptr)
		{
		}
	
    /// \brief create given number of new elements
		void add_elements(size_t nr_new_elems)
		{
			pool_storage.insert(pool_storage.begin(), nr_new_elems, T());
		}

    /// \brief create one new element and return a pointer to it
    /// \details The parameters to this function are either forwarded to a
    /// constructor or to rep_init().  These methods are assumed to
    /// (re)initialise the new element.
    template <class... Args>
    T* get_element(Args&&... args)
    {
      if (nullptr == pool_storage_free_elements)
      {
        pool_storage.emplace_back(std::forward<Args>(args)...);
        return &(pool_storage.back());
      }
      else
      {
        T* result = pool_storage_free_elements;
        pool_storage_free_elements = result->rep_next();
        result->rep_init(std::forward<Args>(args)...);
        return result;
      }
    }

    /// \brief delete one element (add it to the free list)
    void remove_element(T* e)
    {
      e->set_rep_next(pool_storage_free_elements);
      pool_storage_free_elements = e;
    }

    /// \brief delete all elements (including those in free list)
    void clear()
    {
      pool_storage_free_elements = nullptr;
      pool_storage.clear();
    }
  private:
    // Copy constructor is not possible
    pool(const pool&)
    {
      assert(0);
    }

    // Assignment of lists is not possible
    pool operator=(const pool&)
    {
      assert(0);
    }
  public:
    /// \brief iterator to the first element in the pool
    /// \details Iterating through the pool using these iterators also visits
    /// the elements that are in the free list.
    iterator begin()
    {
      return pool_storage.begin();
    }

    /// \brief const iterator to the first element in the pool
    /// \details Iterating through the pool using these iterators also visits
    /// the elements that are in the free list.
    const_iterator begin() const
    {
      return pool_storage.begin();
    }

    /// \brief iterator past the last element in the pool
    /// \details Iterating through the pool using these iterators also visits
    /// the elements that are in the free list.
    iterator end()
    {
      return pool_storage.end();
    }
};

} // namespace detail

} // namespace lts

} // namespace mcrl2

#endif // REPOSITORY_GW
