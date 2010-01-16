// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/sort_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_SORT_TRAVERSER_H
#define MCRL2_LPS_DETAIL_SORT_TRAVERSER_H

#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/lps/detail/traverser.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief 
  template <typename Derived>
  struct sort_traverser: public data::detail::sort_traverser<Derived>
  {
    typedef data::detail::sort_traverser<Derived> super;

    using super::operator();

    /// \brief Traverses an action label
    void operator()(const action_label& l)
    {
      super::operator()(l.sorts());
    }
  };

  template <typename Derived, typename AdaptablePredicate>
  class selective_sort_traverser: public data::detail::selective_traverser<Derived, AdaptablePredicate>
  {
    typedef data::detail::selective_traverser<Derived, AdaptablePredicate> super;

    public:

      selective_sort_traverser()
      { }

      selective_sort_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_SORT_TRAVERSER_H
