// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/filtered_list.h
/// \brief Filtered list, based on boost filter_iterator.

#ifndef MCRL2_ATERMPP_FILTERED_LIST_H
#define MCRL2_ATERMPP_FILTERED_LIST_H

#include "mcrl2/atermpp/aterm_list.h"
#include <boost/iterator/filter_iterator.hpp>

namespace atermpp {

  /// \brief Represents a filtered list. The range [begin(), end()[ is filtered
  /// according to a predicate.
  template <typename List, typename Predicate>
  class filtered_list: public List
  {
    protected:
      /// A predicate.
      const Predicate& m_predicate;

    public:
      /// The iterator type of the filtered list.
      typedef boost::filter_iterator<Predicate, typename List::const_iterator> iterator;

      /// \brief Constructor.
      /// \param l A list.
      /// \param predicate A predicate.
      filtered_list(List l, Predicate predicate)
        : List(l), m_predicate(predicate)
      {
      }

      /// \brief Returns a const iterator pointing to the beginning of the filtered list.
      /// \return A const iterator pointing to the beginning of the filtered list.
      iterator begin() const
      {
        return iterator(m_predicate, List::begin(), List::end());
      }

      /// \brief Returns a const iterator pointing to the end of the filtered list.
      /// \return A const iterator pointing to the end of the filtered list.
      iterator end() const
      {
        return iterator(m_predicate, List::end(), List::end());
      }
  };

} // namespace atermpp

#endif // MCRL2_ATERMPP_FILTERED_LIST_H
