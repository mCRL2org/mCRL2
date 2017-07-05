// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/indexed_set.h
/// \brief Indexed set.

#ifndef MCRL2_ATERMPP_INDEXED_SET_H
#define MCRL2_ATERMPP_INDEXED_SET_H

#include <vector>
#include <stack>
#include <cassert>
#include "mcrl2/atermpp/detail/aterm_implementation.h"

namespace atermpp
{

/// \brief Indexed set. 
template <class ELEMENT>
class indexed_set
{
  protected:
    std::size_t sizeMinus1;
    unsigned int max_load;
    std::size_t nr_of_insertions_until_next_rehash;
    std::vector<std::size_t> hashtable;
    std::deque <ELEMENT > m_keys;
    std::stack < std::size_t > free_positions; 

    /* Find whether the key is already inserted in the hashtable.
       If no, insert n as the index for key. If yes return its already
       existing index */
    std::size_t put_in_hashtable(const ELEMENT& key, std::size_t n);

    /* Double the size of the hashtable. */
    void resize_hashtable();

  public:
    /// \brief A constant that if returned as an index means that the index does not exist.
    //         In general this means that a requested element is not in the set.
    static const std::size_t npos=static_cast<std::size_t>(-1);

    /// Create a new indexed_set.
    /// \param initial_size The initial capacity of the set.
    /// \param max_load_pct The maximum load percentage.
    indexed_set(std::size_t initial_size = 100, unsigned int max_load_pct = 75);

    /// \brief Clear the hash table in the set.
    /// This function clears the hash table in the set, but does not release the memory.
    /// Using indexed_set_reset instead of indexed_set_destroy is preferable when indexed sets of
    /// approximately the same size are being used.
    void clear();

    /// \brief Enter an element with the indicated key into the set.
    /// \details This functions enters an element with the indicated key into the set. If elem was already in the set the previously
    /// assigned index of \a key is returned, and the boolean is set to false. If \a key did not yet occur in set a
    /// new number is assigned, and the boolean is set to true.  This number can either be the number of an
    /// element that has been removed, or, if such a number is not available, the lowest non used number
    /// is assigned to elem and returned. The lowest number that is used is 0.
    /// \param[in] key An element to be put in the set.
    /// \return A pair denoting the index of the element in the set, and a boolean denoting whether the term
    /// was already contained in the set.
    std::pair<std::size_t, bool> put(const ELEMENT& key);

    /// \brief Find the index of elem in set.
    /// The index assigned to elem is returned, except when elem is not in the set, in
    /// which case the return value is indexed_set::npos, i.e. the largest number in std::size_t.
    /// \param elem An element of the set.
    /// \return The index of the element.
    ssize_t index(const ELEMENT& elem) const;

    /// \brief Find the index of elem in set.
    /// The index assigned to elem is returned. When elem is not in the set, it
    /// will be added first.
    /// \param elem An element of the set.
    /// \return The index of the element.
    std::size_t operator[](const ELEMENT& elem)
    {
      std::size_t result = index(elem);
      if (result == npos)
      {
        std::pair<std::size_t, bool> p=put(elem);
        if(!p.second)
        {
          throw std::runtime_error("failed to add element to indexed set");
        }
        result = p.first;
      }
      return result;
    }

    /// \brief Retrieve the element at index in set.
    /// This function must be invoked with a valid index and it returns the elem assigned
    /// to this index. If it is invoked with an invalid index, effects are not predictable.
    /// \param index A positive number.
    /// \return The element in the set with the given index.
    const ELEMENT& get(std::size_t index) const;

    /// \brief Indicates whether a certain index is defined.
    /// \param index A positive number.
    /// \return The element in the set with the given index.
    bool defined(std::size_t index) const;

    /// \brief Remove an element from set. 
    /// \details The element with the indicated key is removed from the indexed set, and if a number was assigned to it, 
    /// this number is freed to be reassigned to another element, that may be put into the set at some later instance. 
    /// \param key The key of the element that is removed. 
    /// \return whether the element was successfully removed. 
    bool erase(const ELEMENT& key); 

    /// \brief Returns the size of the indexed set.
    std::size_t size() const
    {
      return m_keys.size()-free_positions.size();
    }
};

} // namespace atermpp

#include "mcrl2/atermpp/detail/indexed_set.h"


#endif // MCRL2_ATERMPP_INDEXED_SET_H
