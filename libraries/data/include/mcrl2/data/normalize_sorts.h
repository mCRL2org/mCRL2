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

#include "mcrl2/data/builder.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

namespace data {

namespace detail {

  // TODO: move the code from data_specification to this class
  template <typename Derived>
  class sort_normalization_builder: public data::sort_expression_builder<Derived>
  {
    protected:
      const data_specification& m_data_spec;

    public:
      typedef data::sort_expression_builder<Derived> super; 
      using super::enter;
      using super::leave;
      using super::operator();
        
      sort_normalization_builder(const data_specification& data_spec)
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

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_NORMALIZE_SORTS_H
