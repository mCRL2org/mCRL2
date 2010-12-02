// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/builder.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_BUILDER_H
#define MCRL2_DATA_BUILDER_H

#include "mcrl2/core/builder.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/multiple_possible_sorts.h"
#include "mcrl2/data/unknown_sort.h"

namespace mcrl2 {

namespace data {

  /// \brief Traversal class for state formula data types
  template <typename Derived>
  class data_expression_builder: public core::builder<Derived>
  {
    public:
      typedef core::builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/data/detail/data_expression_builder.inc.h"
  };

  /// \brief Traversal class for sort expressions
  template <typename Derived>
  class sort_expression_builder: public data_expression_builder<Derived>
  {
    public:
      typedef data_expression_builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/data/detail/sort_expression_builder.inc.h"
  };

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_BUILDER_H
