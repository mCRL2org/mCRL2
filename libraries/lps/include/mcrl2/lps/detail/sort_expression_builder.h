// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/sort_expression_builder.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_SORT_EXPRESSION_BUILDER_H
#define MCRL2_LPS_DETAIL_SORT_EXPRESSION_BUILDER_H

#include "mcrl2/data/detail/sort_expression_builder.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  template <typename Derived>
  class sort_expression_builder: public data::detail::sort_expression_builder<Derived>
  {
    public:
      typedef data::detail::sort_expression_builder<Derived> super;
  
      using super::enter;
      using super::leave;
      using super::operator();

#include "mcrl2/lps/detail/sort_expression_builder.inc.h"
  };          

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_SORT_EXPRESSION_BUILDER_H
