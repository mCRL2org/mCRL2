// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/data_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_DATA_REWRITER_H
#define MCRL2_PBES_REWRITERS_DATA_REWRITER_H

#include "mcrl2/pbes/builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct NoSubst
{
};

inline
std::ostream& operator<<(std::ostream& out, const NoSubst&)
{
  return out << "[]";
}

template <typename DataRewriter, typename SubstitutionFunction>
data::data_expression data_rewrite(const data::data_expression& x, const DataRewriter& R, SubstitutionFunction& sigma)
{
  mCRL2log(log::debug2) << "data_rewrite " << x << sigma << " -> " << R(x, sigma) << std::endl;
  return R(x, sigma);
}

template <typename DataRewriter>
data::data_expression data_rewrite(const data::data_expression& x, const DataRewriter& R, NoSubst&)
{
  mCRL2log(log::debug2) << "data_rewrite " << x << "[]" << " -> " << R(x) << std::endl;
  return R(x);
}

template <template <class> class Builder, class Derived, class DataRewriter, class SubstitutionFunction = NoSubst>
struct add_data_rewriter: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  typedef core::term_traits<pbes_expression> tr;

  const DataRewriter& R;
  SubstitutionFunction& sigma;

  add_data_rewriter(const DataRewriter& R_, SubstitutionFunction& sigma_)
    : R(R_), sigma(sigma_)
  {}

  pbes_expression operator()(const data::data_expression& x)
  {
    return data_rewrite(x, R, sigma);
  }

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    std::vector<data::data_expression> d;
    auto const& e = x.parameters();
    for (auto i = e.begin(); i != e.end(); ++i)
    {
      d.push_back(data_rewrite(*i, R, sigma));
    }
    return propositional_variable_instantiation(x.name(), data::data_expression_list(d.begin(), d.end()));
  }
};

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct data_rewriter_builder: public add_data_rewriter<pbes_expression_builder, Derived, DataRewriter, SubstitutionFunction>
{
  typedef add_data_rewriter<pbes_expression_builder, Derived, DataRewriter, SubstitutionFunction> super;
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
  typedef Builder<apply_rewriter_builder<Builder, DataRewriter, SubstitutionFunction>, DataRewriter, SubstitutionFunction> super;
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
  typedef pbes_expression term_type;
  typedef data::variable variable_type;

  const DataRewriter& R;

  data_rewriter(const DataRewriter& R_)
    : R(R_)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    detail::NoSubst sigma;
    return detail::make_apply_rewriter_builder<detail::data_rewriter_builder>(R, sigma)(x);
  }

  template <typename SubstitutionFunction>
  pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
  {
    return detail::make_apply_rewriter_builder<detail::data_rewriter_builder>(R, sigma)(x);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_DATA_REWRITER_H
