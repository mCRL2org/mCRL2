// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/sort_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_SORT_TRAVERSER_H
#define MCRL2_PBES_DETAIL_SORT_TRAVERSER_H

#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/pbes/detail/traverser.h"

namespace mcrl2 {

namespace pbes_system {

namespace traversal {

  /// \brief Adds pbes traversal functions to the data sort traverser.
  template <typename Derived>
  struct sort_traverser_base: public data::detail::sort_traverser<Derived>
  {
    typedef data::detail::sort_traverser<Derived> super;
    using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/pbes/detail/traverser.inc.h"
  };

  /// \brief
  template <typename Derived>
  struct sort_traverser: public sort_traverser_base<Derived>
  {
    typedef sort_traverser_base<Derived> super;
    using super::operator();
      
    // no need to change anything...
  };

  /// \brief Adds pbes traversal functions to the data selective sort traverser.
  template <typename Derived, typename AdaptablePredicate>
  class selective_sort_traverser_base: public data::detail::selective_traverser<Derived, AdaptablePredicate>
  {
    typedef data::detail::selective_traverser<Derived, AdaptablePredicate> super;
    using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/pbes/detail/traverser.inc.h"
  };

  template <typename Derived, typename AdaptablePredicate>
  struct selective_sort_traverser: public selective_sort_traverser_base<Derived, AdaptablePredicate>
  {
    typedef selective_sort_traverser_base<Derived, AdaptablePredicate> super;

    public:
      selective_sort_traverser()
      { }

      selective_sort_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace traversal

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_SORT_TRAVERSER_H
