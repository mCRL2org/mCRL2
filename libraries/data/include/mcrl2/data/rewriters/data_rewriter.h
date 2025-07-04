// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriters/data_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REWRITERS_DATA_REWRITER_H
#define MCRL2_DATA_REWRITERS_DATA_REWRITER_H

#include "mcrl2/data/builder.h"
#include "mcrl2/data/substitutions/no_substitution.h"

namespace mcrl2::data {

namespace detail {

template <typename DataRewriter, typename SubstitutionFunction>
data::data_expression data_rewrite(const data::data_expression& x, const DataRewriter& R, SubstitutionFunction& sigma)
{
  mCRL2log(log::trace) << "data_rewrite " << x << sigma << " -> " << R(x, sigma) << std::endl;
  return R(x, sigma);
}

template <typename DataRewriter>
data::data_expression data_rewrite(const data::data_expression& x, const DataRewriter& R, data::no_substitution&)
{
  mCRL2log(log::trace) << "data_rewrite " << x << "[]" << " -> " << R(x) << std::endl;
  return R(x);
}

/// \brief Applies a data rewriter to data expressions appearing in a term. It works both with and without a substitution.
template <template <class> class Builder, class Derived, class DataRewriter, class SubstitutionFunction = data::no_substitution>
struct add_data_rewriter: public Builder<Derived>
{
  using super = Builder<Derived>;
  using super::enter;
  using super::leave;
  using super::operator();

  const DataRewriter& R;
  SubstitutionFunction& sigma;

  add_data_rewriter(const DataRewriter& R_, SubstitutionFunction& sigma_)
    : R(R_), sigma(sigma_)
  {}

  data_expression operator()(const data::data_expression& x)
  {
    return data_rewrite(x, R, sigma);
  }

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = data_rewrite(x, R, sigma);
  }

};

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct data_rewriter_builder: public add_data_rewriter<data::data_expression_builder, Derived, DataRewriter, SubstitutionFunction>
{
  using super = add_data_rewriter<data::data_expression_builder, Derived, DataRewriter, SubstitutionFunction>;
  using super::enter;
  using super::leave;
  using super::operator();

  data_rewriter_builder(const DataRewriter& R, SubstitutionFunction& sigma)
    : super(R, sigma)
  {}
};

template <template <class, class, class> class Builder, class DataRewriter, class SubstitutionFunction>
struct apply_rewriter_builder: public Builder<apply_rewriter_builder<Builder, DataRewriter, SubstitutionFunction>, DataRewriter, SubstitutionFunction>
{
  using super = Builder<apply_rewriter_builder<Builder, DataRewriter, SubstitutionFunction>,
      DataRewriter,
      SubstitutionFunction>;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_rewriter_builder(const DataRewriter& datar, SubstitutionFunction& sigma)
    : super(datar, sigma)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class, class, class> class Builder, class DataRewriter, class SubstitutionFunction>
apply_rewriter_builder<Builder, DataRewriter, SubstitutionFunction>
make_apply_rewriter_builder(const DataRewriter& datar, SubstitutionFunction& sigma)
{
  return apply_rewriter_builder<Builder, DataRewriter, SubstitutionFunction>(datar, sigma);
}

} // namespace detail

/// \brief A rewriter that applies a data rewriter to data expressions in a term.
template <typename DataRewriter>
struct data_rewriter
{
  using term_type = data_expression;
  using variable_type = data::variable;

  const DataRewriter& R;

  data_rewriter(const DataRewriter& R_)
    : R(R_)
  {}

  data_expression operator()(const data_expression& x) const
  {
    data::no_substitution sigma;
    return detail::make_apply_rewriter_builder<detail::data_rewriter_builder>(R, sigma)(x);
  }

  template <typename SubstitutionFunction>
  data_expression operator()(const data_expression& x, SubstitutionFunction& sigma) const
  {
    return detail::make_apply_rewriter_builder<detail::data_rewriter_builder>(R, sigma)(x);
  }
};

} // namespace mcrl2::data



#endif // MCRL2_DATA_REWRITERS_DATA_REWRITER_H

