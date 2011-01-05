// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/normalize_sorts.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_NORMALIZE_SORTS_H
#define MCRL2_PBES_NORMALIZE_SORTS_H

#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  template <typename Derived>
  struct sort_normalization_builder: public data::detail::sort_normalization_builder<Derived>
  {
    typedef data::detail::sort_normalization_builder<Derived> super;  
    using super::enter;
    using super::leave;
    using super::operator();

    sort_normalization_builder(const data::data_specification& data_spec)
      : super(data_spec)
    {}

#include "mcrl2/pbes/detail/sort_expression_builder.inc.h"
  };

} // namespace detail

  template <typename T>
  void normalize_sorts(T& x,
                       const data::data_specification& data_spec,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      )
  {
    core::make_apply_builder_arg1<detail::sort_normalization_builder>(data_spec)(x);
  }

  template <typename T>
  T normalize_sorts(const T& x,
                    const data::data_specification& data_spec,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                   )
  {
    return core::make_apply_builder_arg1<detail::sort_normalization_builder>(data_spec)(x);
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_NORMALIZE_SORTS_H
