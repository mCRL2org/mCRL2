// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/rewriters/data_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_REWRITERS_DATA_REWRITER_H
#define MCRL2_LPS_REWRITERS_DATA_REWRITER_H

#include "mcrl2/data/rewriters/data_rewriter.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2 {

namespace lps {

namespace detail {

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct data_rewriter_builder: public data::detail::add_data_rewriter<lps::data_expression_builder, Derived, DataRewriter, SubstitutionFunction>
{
  typedef data::detail::add_data_rewriter<lps::data_expression_builder, Derived, DataRewriter, SubstitutionFunction> super;
  using super::enter;
  using super::leave;
  using super::operator();

  data_rewriter_builder(const DataRewriter& R, SubstitutionFunction& sigma)
    : super(R, sigma)
  {}
};

} // namespace detail

/// \brief A rewriter that applies a data rewriter to data expressions in a term.
template <typename DataRewriter>
struct data_rewriter
{
  typedef data::data_expression term_type;
  typedef data::variable variable_type;

  const DataRewriter& R;

  data_rewriter(const DataRewriter& R_)
    : R(R_)
  {}

  data::data_expression operator()(const data::data_expression& x) const
  {
    data::no_substitution sigma;
    return data::detail::make_apply_rewriter_builder<detail::data_rewriter_builder>(R, sigma)(x);
  }

  template <typename SubstitutionFunction>
  data::data_expression operator()(const data::data_expression& x, SubstitutionFunction& sigma) const
  {
    return data::detail::make_apply_rewriter_builder<detail::data_rewriter_builder>(R, sigma)(x);
  }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_REWRITERS_DATA_REWRITER_H

