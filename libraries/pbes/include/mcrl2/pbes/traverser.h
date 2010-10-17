// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TRAVERSER_H
#define MCRL2_PBES_TRAVERSER_H

#include "mcrl2/data/traverser.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Traversal class for PBES data types
  template <typename Derived>
  class traverser: public data::traverser<Derived>
  {
    public:
      typedef data::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

#include "mcrl2/pbes/detail/traverser.inc.h"
  };

  template <typename Derived>
  class binding_aware_traverser_helper: public data::binding_aware_traverser<Derived>
  {
    public:
      typedef data::binding_aware_traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;
      using super::increase_bind_count;
      using super::decrease_bind_count;

#include "mcrl2/pbes/detail/traverser.inc.h"
  };

  template <typename Derived>
  class binding_aware_traverser: public binding_aware_traverser_helper<Derived>
  {
    public:
      typedef binding_aware_traverser_helper<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;
      using super::increase_bind_count;
      using super::decrease_bind_count;

      void operator()(pbes_equation const& x)
      {
        increase_bind_count(x.variable().parameters());
        super::operator()(x);
        decrease_bind_count(x.variable().parameters());
      }

      void operator()(exists const& x)
      {
        increase_bind_count(x.variables());
        super::operator()(x);
        decrease_bind_count(x.variables());
      }

      void operator()(forall const& x)
      {
        increase_bind_count(x.variables());
        super::operator()(x);
        decrease_bind_count(x.variables());
      }
  };

  /// \brief Selective traversal class for PBES data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, pbes_system::traverser>
  {
    public:
      typedef core::selective_traverser<Derived, AdaptablePredicate, pbes_system::traverser> super;
      using super::operator();
      using super::enter;
      using super::leave;

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

//  template <typename Derived, typename AdaptablePredicate>
//  class selective_binding_aware_traverser: public pbes_system::selective_traverser<Derived, AdaptablePredicate, pbes_system::binding_aware_traverser>
//  {
//    public:
//      typedef pbes_system::selective_traverser<Derived, AdaptablePredicate, pbes_system::binding_aware_traverser> super;
//      using super::enter;
//      using super::leave;
//      using super::operator();
//
//      selective_binding_aware_traverser()
//      { }
//
//      selective_binding_aware_traverser(AdaptablePredicate predicate): super(predicate)
//      { }
//  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TRAVERSER_H
