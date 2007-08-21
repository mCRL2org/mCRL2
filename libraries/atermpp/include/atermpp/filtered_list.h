// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/filtered_list.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_FILTERED_LIST_H
#define MCRL2_ATERMPP_FILTERED_LIST_H

#include "atermpp/aterm_list.h"
#include <boost/iterator/filter_iterator.hpp>

namespace atermpp {

  ///////////////////////////////////////////////////////////////////////////////
  // filtered_list
  /// \brief represents a filtered term_list
  ///
  template <typename List, typename Predicate>
  class filtered_list: public List
  {
    protected:
      const Predicate& m_predicate;
    
    public:
      typedef boost::filter_iterator<Predicate, typename List::const_iterator> iterator;

      filtered_list(List l, Predicate predicate)
        : List(l), m_predicate(predicate)
      {
      }

      /// Returns a const iterator pointing to the beginning of the filtered list.     
      ///
      iterator begin() const
      {
        return iterator(m_predicate, List::begin(), List::end());
      }

      /// Returns a const iterator pointing to the end of the filtered list.
      ///
      iterator end() const
      {
        return iterator(m_predicate, List::end(), List::end());
      }
  };

} // namespace atermpp

#endif // MCRL2_ATERMPP_FILTERED_LIST_H
