// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/is_untyped.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_IS_UNTYPED_H
#define MCRL2_DATA_DETAIL_IS_UNTYPED_H

#include "mcrl2/data/traverser.h"

namespace mcrl2 {

namespace data {

namespace detail {

struct is_untyped_traverser: public sort_expression_traverser<is_untyped_traverser>
{
  typedef sort_expression_traverser<is_untyped_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;

  is_untyped_traverser()
    : result(false)
  {}

  void apply(const data::untyped_identifier& )
  {
    result = true;
  }

  void apply(const data::untyped_identifier_assignment& )
  {
    result = true;
  }

  void apply(const data::untyped_sort& )
  {
    result = true;
  }

  void apply(const data::untyped_possible_sorts& )
  {
    result = true;
  }

  void apply(const data::untyped_set_or_bag_comprehension& )
  {
    result = true;
  }

  void apply(const data::structured_sort_constructor_argument& )
  {
    result = true;
  }

  void apply(const data::structured_sort_constructor& )
  {
    result = true;
  }
};

inline
bool is_untyped(const data_expression& x)
{
  is_untyped_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_IS_UNTYPED_H
