// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/normalize_sorts.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_NORMALIZE_SORTS_H
#define MCRL2_DATA_NORMALIZE_SORTS_H

#include <functional>
#include "mcrl2/data/builder.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

namespace data {

namespace detail {
  
  struct normalize_sorts_function: public std::unary_function<data::sort_expression, data::sort_expression>
  {
    const data_specification& m_data_spec;
    
    normalize_sorts_function(const data_specification& data_spec)
      : m_data_spec(data_spec)
    {}

    sort_expression operator()(const sort_expression& x)
    {
      return m_data_spec.normalise_sorts(x);
    }
  };

} // namespace detail

  template <typename T>
  void normalize_sorts(T& x,
                       const data::data_specification& data_spec,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      )
  {
    core::make_update_apply_builder<data::sort_expression_builder>(data::detail::normalize_sorts_function(data_spec))(x);
  }

  template <typename T>
  T normalize_sorts(const T& x,
                    const data::data_specification& data_spec,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                   )
  {
    return core::make_update_apply_builder<data::sort_expression_builder>(data::detail::normalize_sorts_function(data_spec))(x);
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_NORMALIZE_SORTS_H
