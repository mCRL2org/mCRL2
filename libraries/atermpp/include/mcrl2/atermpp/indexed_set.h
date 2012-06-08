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

#include <stack>
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{

/// \brief Indexed set. 
class indexed_set
{
  protected:
    size_t sizeMinus1;
    unsigned int max_load;
    size_t max_entries;
    std::vector<size_t> hashtable;
    std::vector < std::vector <aterm > > m_keys;
    std::stack < size_t > free_positions;
    std::size_t m_size;                   /**< the number of entries of the table */

    /* Find whether the key is already inserted in the hashtable.
       If no, insert n as the index for key. If yes return its already
       existing index */
    size_t hashPut(const aterm &key, size_t n);

    /* Double the size of the hashtable. */
    void hashResizeSet();

  public:
    /// Create a new indexed_set.
    /// \param initial_size The initial capacity of the set.
    /// \param max_load_pct The maximum load percentage.
    indexed_set(size_t initial_size = 100, unsigned int max_load_pct = 75);

    /// \brief Clear the hash table in the set.
    /// This function clears the hash table in the set, but does not release the memory.
    /// Using indexed_set_reset instead of indexed_set_destroy is preferable when indexed sets of
    /// approximately the same size are being used.
    void reset();

    /// \brief Enter elem into the set.
    /// This functions enters elem into the set. If elem was already in the set the previously
    /// assigned index of elem is returned, and the boolean is set to false. If elem did not yet occur in set a
    /// new number is assigned, and the boolean is set to true.  This number can either be the number of an
    /// element that has been removed, or, if such a number is not available, the lowest non used number
    /// is assigned to elem and returned. The lowest number that is used is 0.
    /// \param elem A term.
    /// \return A pair denoting the index of the element in the set, and a boolean denoting whether the term
    /// was already contained in the set.
    std::pair<size_t, bool> put(const aterm& elem);

    /// \brief Find the index of elem in set.
    /// The index assigned to elem is returned, except when elem is not in the set, in
    /// which case the return value is (size_t)-1, i.e. the largest number in size_t.
    /// \param elem An element of the set.
    /// \return The index of the element.
    ssize_t index(const aterm& elem) const;

    /// \brief Find the index of elem in set.
    /// The index assigned to elem is returned. When elem is not in the set, it
    /// will be added first.
    /// \param elem An element of the set.
    /// \return The index of the element.
    size_t operator[](const aterm& elem)
    {
      std::size_t result = index(elem);
      if (result == (std::size_t) -1)
      {
        std::pair<size_t, bool> p=put(elem);
        if(!p.second)
        {
          throw mcrl2::runtime_error("failed to add element to indexed set");
        }
      }
      return result;
    }

    /// \brief Retrieve the element at index in set.
    /// This function must be invoked with a valid index and it returns the elem assigned
    /// to this index. If it is invoked with an invalid index, effects are not predictable.
    /// \param index A positive number.
    /// \return The element in the set with the given index.
    aterm get(size_t index) const;

    /// \brief Remove elem from set.
    /// The elem is removed from the indexed set, and if a number was assigned to elem,
    /// it is freed to be reassigned to an element, that may be put into the set at some later instance.
    /// \param elem An element of the set.
    /// \return whether the element was successfully removed.
    bool remove(const aterm& elem);

    /// \brief Retrieve all elements in set.
    /// A list with all valid elements stored in the indexed set is returned.  The list is
    /// ordered from element with index 0 onwards.
    /// \return An ordered list containing the elements of the set.
    aterm_list keys() const; 
    
    /// \brief Returns the size of the indexed set.
    std::size_t size() const
    {
      return m_size;
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_INDEXED_SET_H
