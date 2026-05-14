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

#include "mcrl2/data/concepts.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/substitutions/no_substitution.h"

namespace mcrl2::data {

namespace detail {

template <typename DataRewriter, IsSubstitution SubstitutionFunction>
void data_rewrite(data_expression& result, const data_expression& x, const DataRewriter& R, SubstitutionFunction& sigma)
{
  R(result, x, sigma);
}

template <typename DataRewriter>
void data_rewrite(data_expression& result, const data_expression& x, const DataRewriter& R, no_substitution&)
{
  R(result, x);
}

/// \brief Applies a data rewriter to data expressions appearing in a term. It works both with and without a substitution.
template <template <class> class Builder, class Derived, class DataRewriter, IsSubstitution SubstitutionFunction = no_substitution>
struct add_data_rewriter: public Builder<Derived>
{
  using super = Builder<Derived>;
  using super::enter;
  using super::leave;
  // using super::operator();

  const DataRewriter& m_R;
  SubstitutionFunction& m_sigma;

  add_data_rewriter(const DataRewriter& R, SubstitutionFunction& sigma)
    : m_R(R), m_sigma(sigma)
  {}

  data_expression operator()(const data_expression& x)
  {
    data_expression result;
    data_rewrite(result, x, m_R, m_sigma);
    return result;
  }

  template <class T>
  void apply(T& result, const data_expression& x)
  {
    data_rewrite(atermpp::assign_cast<data_expression>(result), x, m_R, m_sigma);
  }

};

template <typename Derived, typename DataRewriter, IsSubstitution SubstitutionFunction>
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

template <template <class, class, class> class Builder, class DataRewriter, IsSubstitution SubstitutionFunction>
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

template <template <class, class, class> class Builder, class DataRewriter, IsSubstitution SubstitutionFunction>
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

  template <IsSubstitution SubstitutionFunction>
  data_expression operator()(const data_expression& x, SubstitutionFunction& sigma) const
  {
    return detail::make_apply_rewriter_builder<detail::data_rewriter_builder>(R, sigma)(x);
  }
};

} // namespace mcrl2::data



#endif // MCRL2_DATA_REWRITERS_DATA_REWRITER_H

