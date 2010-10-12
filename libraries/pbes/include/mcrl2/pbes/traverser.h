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
  struct binding_aware_traverser: public data::binding_aware_traverser<Derived>
  {
    typedef data::binding_aware_traverser<Derived> super;

    using super::operator();
    using super::enter;
    using super::leave;

#include "mcrl2/pbes/detail/traverser.inc.h"
  };

  /// \brief Selective traversal class for PBES data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, pbes::traverser>
  {
    typedef core::selective_traverser<Derived, AdaptablePredicate, pbes::traverser> super;

    public:

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TRAVERSER_H
