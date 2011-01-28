// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/normalize_sorts.h
/// \brief This file contains a forward declaration for the file mcrl2/data/normalize_sorts.h,
//         which has a circular header dependency with mcrl2/data/data_specification.h. To break
//         the dependency the functions normalize_sorts are declared here, including its default
//         parameters. The impl

#ifndef MCRL2_DATA_NORMALIZE_SORTS_FWD_H
#define MCRL2_DATA_NORMALIZE_SORTS_FWD_H

#include <functional>
#include "mcrl2/data/builder.h"
// #include "mcrl2/data/data_specification.h"

namespace mcrl2 {

namespace data {

  class data_specification;

  template <typename T>
  void normalize_sorts(T& x,
                       const data::data_specification& data_spec,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      );

  template <typename T>
  T normalize_sorts(const T& x,
                    const data::data_specification& data_spec,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                   );

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_NORMALIZE_SORTS_FWD_H
