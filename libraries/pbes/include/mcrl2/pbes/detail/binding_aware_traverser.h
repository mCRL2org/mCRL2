// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/binding_aware_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_BINDING_AWARE_TRAVERSER_H
#define MCRL2_PBES_DETAIL_BINDING_AWARE_TRAVERSER_H

#include "mcrl2/pbes/detail/traverser.h"
#include "mcrl2/data/detail/binding_aware_traverser.h"

namespace mcrl2 {

namespace pbes_system {

namespace traversal {

  // Adds pbes traversal functions to the data binding aware traverser.
  template <typename Derived>
  struct binding_aware_traverser_base: public data::detail::binding_aware_traverser<Derived>
  {
    typedef data::detail::binding_aware_traverser<Derived> super;
    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/pbes/detail/traverser.inc.h"
  };

  /// \brief Binding aware traverser.
  template <typename Derived>
  struct binding_aware_traverser: public binding_aware_traverser_base<Derived>
  {
    typedef binding_aware_traverser_base<Derived> super;

    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

    /// \brief Traverses a forall quantifier
    /// \param x A forall quantifier
    void operator()(const forall& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }

    /// \brief Traverses a forall quantifier
    /// \param x An exists quantifier
    void operator()(const exists& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }
  };

  // Adds pbes traversal functions to the data selective binding aware traverser.
  template <typename Derived, typename AdaptablePredicate>
  class selective_binding_aware_traverser_base: public data::detail::selective_binding_aware_traverser<Derived, AdaptablePredicate>
  {
    typedef data::detail::selective_binding_aware_traverser<Derived, AdaptablePredicate> super;
    using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/pbes/detail/traverser.inc.h"
  };

  template <typename Derived, typename AdaptablePredicate>
  class selective_binding_aware_traverser: public selective_binding_aware_traverser_base<Derived, AdaptablePredicate>
  {
    typedef selective_binding_aware_traverser_base<Derived, AdaptablePredicate> super;
    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

    public:
      selective_binding_aware_traverser()
      { }

      selective_binding_aware_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace traversal

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_BINDING_AWARE_TRAVERSER_H
