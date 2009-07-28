// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_sort_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_SORT_TRAVERSER_H
#define MCRL2_LPS_DETAIL_LPS_SORT_TRAVERSER_H

#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/lps/detail/lps_data_traverser.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for applying a substitution to LPS data types.
  template < typename Derived >
  struct lps_sort_traverser : public lps::detail::lps_data_traverser< Derived, data::detail::sort_traverser >
  {
    typedef lps::detail::lps_data_traverser< Derived, data::detail::sort_traverser > super;

    using super::operator();

    /// \brief Traverses an action label
    void operator()(const action_label& l)
    {
      (*this)(l.sorts());
    }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_SORT_TRAVERSER_H
