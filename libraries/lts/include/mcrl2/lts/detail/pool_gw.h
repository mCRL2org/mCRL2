// Author(s): Jan Friso Groote, Anton Wijs
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

/* The data structure below is intended to recycle
 * elements of a given type, such that they do not have to be malloced and deleted
 * which is time consuming. There is a list of free elements to which deallocated
 * elements are stored, and from which allocated members are taken
 * when they are needed and available.
 *
 * it is assumed that the elements have a pointer to a next element in the pool,
 * and that they have functions rep_next(), set_rep_next() to get and change its
 * value.
 *
 * This list is only intended for the gw bisimulation checker. These
 * lists are non copiable, nor can they be constructed from
 * each other.  */

// pool to keep elements of type T
template < class T>
class pool
{
  protected:
		// deque storing the elements
		std::deque<T> pool_storage;
		// head of list of free elements
		T pool_storage_free_elements;
	
  public:
	  typedef typename std::deque<T>::iterator iterator;
		typedef typename std::deque<T>::const_iterator const_iterator;
	
	  pool()
		{
		}
	
		// create given number of new elements
		void add_elements(size_t nr_new_elems)
		{
			pool_storage.insert(pool_storage.begin(), nr_new_elems, T());
		}

    T* get_element()
    {
      if (pool_storage_free_elements.rep_next()==nullptr)
      {
        pool_storage.push_back(T());
        return &(pool_storage.back());
      }
      else
      {
        T* result=pool_storage_free_elements.rep_next();
        pool_storage_free_elements.set_rep_next(result->rep_next());
        result->rep_init();
        return result;
      }
    }

    void remove_element(T* e)
    {
      e->set_rep_next(pool_storage_free_elements.rep_next());
      pool_storage_free_elements.set_rep_next(e);
    }

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

    iterator begin()
    {
      return pool_storage.begin();
    }

    const_iterator begin() const
    {
      return pool_storage.begin();
    }

    iterator end()
    {
      return pool_storage.end();
    }
};

} // namespace detail

} // namespace lts

} // namespace mcrl2

#endif // REPOSITORY_GW
