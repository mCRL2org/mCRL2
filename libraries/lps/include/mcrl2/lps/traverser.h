// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_TRAVERSER_H
#define MCRL2_LPS_TRAVERSER_H

#include "mcrl2/data/traverser.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

  /// \brief Traversal class for LPS data types
  template <typename Derived>
  class traverser: public data::traverser<Derived>
  {
    public:
      typedef data::traverser<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

#include "mcrl2/lps/detail/traverser.inc.h"
  };

  template <typename Derived>
  class binding_aware_traverser: public data::binding_aware_traverser<Derived>
  {
    public:
      typedef data::binding_aware_traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

#include "mcrl2/lps/detail/traverser.inc.h"
  };

  /// \brief Selective traversal class for BES data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, lps::traverser>
  {
    public:
      typedef core::selective_traverser<Derived, AdaptablePredicate, lps::traverser> super;
      using super::enter;
      using super::leave;
      using super::operator();

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

//  template <typename Derived, typename AdaptablePredicate>
//  class selective_binding_aware_traverser: public data::detail::selective_traverser<Derived, AdaptablePredicate, lps::binding_aware_traverser>
//  {
//    public:
//      typedef data::detail::selective_traverser<Derived, AdaptablePredicate, lps::binding_aware_traverser> super;     
//      using super::enter;
//      using super::leave;
//      using super::operator();
//
//#include "mcrl2/lps/detail/traverser.inc.h"
//  };

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TRAVERSER_H
