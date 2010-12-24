// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/sort_normalization_builder.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_SORT_NORMALIZATION_BUILDER_H
#define MCRL2_PROCESS_DETAIL_SORT_NORMALIZATION_BUILDER_H

#include "mcrl2/lps/detail/sort_normalization_builder.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

namespace detail {

  template <typename Derived>
  class sort_normalization_builder: public lps::detail::sort_normalization_builder<Derived>
  {
    public:
      typedef lps::detail::sort_normalization_builder<Derived> super;
  
      using super::enter;
      using super::leave;
      using super::operator();

      sort_normalization_builder(const data::data_specification& data_spec)
        : super(data_spec)
      {}

#include "mcrl2/process/detail/sort_expression_builder.inc.h"
  };          

  /**
   * Applies sort normalization to a process specification.
   **/
  inline
  void normalize_sorts(process_specification& x)
  {
    core::apply_builder_arg1<sort_normalization_builder, data::data_specification> builder(x.data());
    builder(x);
  }

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_SORT_NORMALIZATION_BUILDER_H
