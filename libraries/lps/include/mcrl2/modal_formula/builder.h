// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/builder.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_BUILDER_H
#define MCRL2_MODAL_FORMULA_BUILDER_H

#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2 {

namespace action_formulas {

  /// \brief Traversal class for action formulas
  template <typename Derived>
  class action_formula_builder: public core::builder<Derived>
  {
    public:
      typedef core::builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/action_formula_builder.inc.h"
  };

  /// \brief Traversal class for data expressions
  template <typename Derived>
  class data_expression_builder: public lps::data_expression_builder<Derived>
  {
    public:
      typedef lps::data_expression_builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/action_formula_data_expression_builder.inc.h"
  };

  /// \brief Traversal class for sort expressions
  template <typename Derived>
  class sort_expression_builder: public lps::sort_expression_builder<Derived>
  {
    public:
      typedef lps::sort_expression_builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/action_formula_sort_expression_builder.inc.h"
  };

} // namespace action_formulas

namespace regular_formulas {

  /// \brief Traversal class for regular formulas
  template <typename Derived>
  class regular_formula_builder: public core::builder<Derived>
  {
    public:
      typedef core::builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/regular_formula_builder.inc.h"
  };

  /// \brief Traversal class for data expressions
  template <typename Derived>
  class data_expression_builder: public action_formulas::data_expression_builder<Derived>
  {
    public:
      typedef action_formulas::data_expression_builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/regular_formula_data_expression_builder.inc.h"
  };

  /// \brief Traversal class for sort expressions
  template <typename Derived>
  class sort_expression_builder: public action_formulas::sort_expression_builder<Derived>
  {
    public:
      typedef action_formulas::sort_expression_builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/regular_formula_sort_expression_builder.inc.h"
  };

} // namespace regular_formulas

namespace state_formulas {


  /// \brief Traversal class for state formulas
  template <typename Derived>
  class state_formula_builder: public core::builder<Derived>
  {
    public:
      typedef core::builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/state_formula_builder.inc.h"
  };

  /// \brief Traversal class for data expressions
  template <typename Derived>
  class data_expression_builder: public regular_formulas::data_expression_builder<Derived>
  {
    public:
      typedef regular_formulas::data_expression_builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/state_formula_data_expression_builder.inc.h"
  };

  /// \brief Traversal class for sort expressions
  template <typename Derived>
  class sort_expression_builder: public regular_formulas::sort_expression_builder<Derived>
  {
    public:
      typedef regular_formulas::sort_expression_builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/state_formula_sort_expression_builder.inc.h"
  };

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_BUILDER_H
