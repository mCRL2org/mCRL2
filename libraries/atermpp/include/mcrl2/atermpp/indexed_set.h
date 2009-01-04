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

#include <boost/shared_ptr.hpp>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{
  /// \cond INTERNAL_DOCS
  struct indexed_set_deleter
  {
    /// \brief Function call operator
    /// \param s An ATerm indexed set
    void operator()(ATermIndexedSet s)
    {
      ATindexedSetDestroy(s);
    }
  }; 
  /// \endcond
  
  /// \brief Indexed set. N.B. Copies of an indexed_set refer to the same object.
  class indexed_set
  {
   protected:
     /// The wrapped ATermTable.
     boost::shared_ptr<_ATermTable> m_set;

   public:
      /// Create a new indexed_set.
      /// \param initial_size The initial capacity of the set.
      /// \param max_load_pct The maximum load percentage.
      indexed_set(unsigned int initial_size = 100, unsigned int max_load_pct = 75)
        : m_set(ATindexedSetCreate(initial_size, max_load_pct), indexed_set_deleter())
      {}
      
      /// \brief Clear the hash table in the set.
      /// This function clears the hash table in the set, but does not release the memory.
      /// Using indexed_set_reset instead of indexed_set_destroy is preferable when indexed sets of
      /// approximately the same size are being used.
      void reset()
      {
        ATindexedSetReset(m_set.get());
      }
      
      /// \brief Enter elem into the set.
      /// This functions enters elem into the set. If elem was already in the set the previously
      /// assigned index of elem is returned, and new is set to false. If elem did not yet occur in set a
      /// new number is assigned, and new is set to true.  This number can either be the number of an
      /// element that has been removed, or, if such a number is not available, the lowest non used number
      /// is assigned to elem and returned. The lowest number that is used is 0.
      /// \param elem A term.
      /// \return A pair denoting the index of the element in the set, and a boolean denoting whether the term
      /// was already contained in the set.
      std::pair<long, bool> put(aterm elem)
      {
        ATbool b;
        long l = ATindexedSetPut(m_set.get(), elem, &b);
        return std::make_pair(l, b == ATtrue);
      }
      
      /// \brief Find the index of elem in set.
      /// The index assigned to elem is returned, except when elem is not in the set, in
      /// which case the return value is a negative number.
      /// \param elem An element of the set.
      /// \return The index of the element.
      long index(aterm elem)
      {
        return ATindexedSetGetIndex(m_set.get(), elem);
      }
      
      /// \brief Retrieve the element at index in set.
      /// This function must be invoked with a valid index and it returns the elem assigned
      /// to this index. If it is invoked with an invalid index, effects are not predictable.
      /// \param index A positive number.
      /// \return The element in the set with the given index.
      aterm get(long index)
      {
        return ATindexedSetGetElem(m_set.get(), index);
      }
      
      /// \brief Remove elem from set.
      /// The elem is removed from the indexed set, and if a number was assigned to elem,
      /// it is freed to be reassigned to an element, that may be put into the set at some later instance.
      /// \param elem An element of the set.
      void remove(aterm elem)
      {
        ATindexedSetRemove(m_set.get(), elem);
      }
      
      /// \brief Retrieve all elements in set.
      /// A list with all valid elements stored in the indexed set is returned.  The list is
      /// ordered from element with index 0 onwards.
      /// \return An ordered list containing the elements of the set.
      aterm_list elements()
      {
        return aterm_list(ATindexedSetElements(m_set.get()));
      }
  };

} // namespace atermpp

#endif // MCRL2_ATERMPP_INDEXED_SET_H
