// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TRAVERSER_H
#define MCRL2_MODAL_FORMULA_TRAVERSER_H

#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/data/traverser.h"

namespace mcrl2 {

namespace action_formulas {

  /// \brief Traversal class for action formula data types
  template <typename Derived>
  class traverser: public data::traverser<Derived>
  {
    public:
      typedef data::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/action_formula_traverser.inc.h"
  };

} // namespace action_formulas

namespace regular_formulas {

  /// \brief Traversal class for regular formula data types
  template <typename Derived>
  class traverser: public data::traverser<Derived>
  {
    public:
      typedef data::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/regular_formula_traverser.inc.h"
  };

} // namespace regular_formulas

namespace state_formulas {

  /// \brief Traversal class for state formula data types
  template <typename Derived>
  class traverser: public data::traverser<Derived>
  {
    public:
      typedef data::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

      void operator()(const mcrl2::regular_formulas::regular_formula& x)
      {}

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/state_formula_traverser.inc.h"
  };

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TRAVERSER_H
