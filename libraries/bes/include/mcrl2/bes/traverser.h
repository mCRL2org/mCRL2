// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_BES_TRAVERSER_H
#define MCRL2_BES_TRAVERSER_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/core/traverser.h"
#include "mcrl2/core/selective_traverser.h"

namespace mcrl2 {

namespace bes {

  /// \brief Traversal class for BES data types
  template <typename Derived>
  class traverser: public core::traverser<Derived>
  {
    public:
      typedef core::traverser<Derived> super;

      using super::enter;
      using super::leave;
      using super::operator();

      void operator()(const core::identifier_string& s)
      { }

      void operator()(const fixpoint_symbol& s)
      { }

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/bes/detail/traverser.inc.h"
  };

  /// \brief Selective traversal class for BES data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, bes::traverser>
  {
    public:
      typedef core::selective_traverser<Derived, AdaptablePredicate, bes::traverser> super;

      using super::enter;
      using super::leave;
      using super::operator();

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_TRAVERSER_H
