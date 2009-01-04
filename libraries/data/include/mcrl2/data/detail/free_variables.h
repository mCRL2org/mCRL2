// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file free_variables.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_FREE_VARIABLES_H
#define MCRL2_DATA_DETAIL_FREE_VARIABLES_H

#include <algorithm>
#include <set>
#include "mcrl2/data/data.h"

namespace mcrl2 {

namespace data {

namespace detail {

/// Function object that adds a data variable to result if it doesn't exist
/// in the two ranges [first1, last1[ and [first2, last2[.
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
struct data_variable_collector
{
  InputIterator1 m_first1;
  InputIterator1 m_last1;
  InputIterator2 m_first2;
  InputIterator2 m_last2;
  OutputIterator m_dest;
    
  data_variable_collector(InputIterator1 first1,
                          InputIterator1 last1,
                          InputIterator2 first2,
                          InputIterator2 last2,
                          OutputIterator dest
                         )
    : 
      m_first1(first1),
      m_last1 (last1 ),
      m_first2(first2),
      m_last2 (last2 ),
      m_dest(dest)
  {}
  
  /// \brief FUNCTION_DESCRIPTION
  /// \param t A term
  /// \return RETURN_DESCRIPTION
  bool operator()(atermpp::aterm_appl t)
  {
    if (is_data_variable(t))
    {
      if (std::find(m_first1, m_last1, t) == m_last1
          && std::find(m_first2, m_last2, t) == m_last2
         )
      {
        *m_dest++ = data_variable(t);
      }
      return false;
    }
    return true;
  }
};

template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
data_variable_collector<InputIterator1, InputIterator2, OutputIterator>
make_data_variable_collector(InputIterator1 first1,
                             InputIterator1 last1,
                             InputIterator2 first2,
                             InputIterator2 last2,
                             OutputIterator dest
                            )
{
  return data_variable_collector<InputIterator1, InputIterator2, OutputIterator>(first1, last1, first2, last2, dest);
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_FREE_VARIABLES_H
