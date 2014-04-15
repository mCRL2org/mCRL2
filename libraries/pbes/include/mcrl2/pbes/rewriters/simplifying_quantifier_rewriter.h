// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/simplifying_quantifier_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_SIMPLIFYING_QUANTIFIER_REWRITER_H
#define MCRL2_PBES_REWRITERS_SIMPLIFYING_QUANTIFIER_REWRITER_H

#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/utilities/detail/optimized_logic_operators.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename Derived, typename DataRewriter>
struct simplify_quantifier_builder: public pbes_expression_builder<Derived>
{
  typedef pbes_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  typedef core::term_traits<pbes_expression> tr;

  const DataRewriter& R;

  simplify_quantifier_builder(const DataRewriter& R_)
    : R(R_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  pbes_expression operator()(const data::data_expression& x)
  {
    return R(x);
  }

  pbes_expression operator()(const true_&)
  {
    return tr::true_();
  }

  pbes_expression operator()(const false_&)
  {
    return tr::false_();
  }

  pbes_expression operator()(const not_& x)
  {
    return utilities::optimized_not(super::operator()(x.operand()));
  }

  pbes_expression operator()(const and_& x)
  {
    auto left = super::operator()(x.left());
    auto right = super::operator()(x.right());
    return utilities::optimized_and(left, right);
  }

  pbes_expression operator()(const or_& x)
  {
    auto left = super::operator()(x.left());
    auto right = super::operator()(x.right());
    return utilities::optimized_or(left, right);
  }

  pbes_expression operator()(const imp& x)
  {
    auto left = super::operator()(x.left());
    auto right = super::operator()(x.right());
    return utilities::optimized_imp(left, right);
  }

  pbes_expression operator()(const forall& x)
  {
    pbes_expression result;
    pbes_expression body = super::operator()(x.body());
    auto const& variables = x.variables();

    if (variables.empty())
    {
      result = tr::true_();
    }
    else if (tr::is_not(body))
    {
      result = utilities::optimized_not(utilities::optimized_exists(variables, core::down_cast<not_>(body).operand(), true));
    }
    if (tr::is_and(body))
    {
      auto const& left = core::down_cast<and_>(body).left();
      auto const& right = core::down_cast<and_>(body).right();
      result = utilities::optimized_and(utilities::optimized_forall(variables, left, true), utilities::optimized_forall(variables, right, true));
    }
    else if (tr::is_or(body))
    {
      auto const& left = core::down_cast<or_>(body).left();
      auto const& right = core::down_cast<or_>(body).right();
      data::variable_list lv = tr::set_intersection(variables, tr::free_variables(left));
      data::variable_list rv = tr::set_intersection(variables, tr::free_variables(right));
      if (lv.empty())
      {
        result = utilities::optimized_or(left, utilities::optimized_forall_no_empty_domain(rv, right, true));
      }
      else if (rv.empty())
      {
        result = utilities::optimized_or(right, utilities::optimized_forall_no_empty_domain(lv, left, true));
      }
      else
      {
        result = utilities::optimized_forall(variables, body, true);
      }
    }
    else
    {
      result = utilities::optimized_forall(variables, body, true);
    }
    return result;
  }

  pbes_expression operator()(const exists& x)
  {
    pbes_expression result;
    pbes_expression body = super::operator()(x.body());
    auto const& variables = x.variables();

    if (variables.empty())
    {
      result = tr::false_();
    }
    else if (tr::is_not(body))
    {
      result = utilities::optimized_not(utilities::optimized_forall(variables, core::down_cast<not_>(body).operand(), true));
    }
    if (tr::is_or(body))
    {
      auto const& left = core::down_cast<or_>(body).left();
      auto const& right = core::down_cast<or_>(body).right();
      result = utilities::optimized_and(utilities::optimized_exists(variables, left, true), utilities::optimized_forall(variables, right, true));
    }
    else if (tr::is_and(body))
    {
      auto const& left = core::down_cast<and_>(body).left();
      auto const& right = core::down_cast<and_>(body).right();
      data::variable_list lv = tr::set_intersection(variables, tr::free_variables(left));
      data::variable_list rv = tr::set_intersection(variables, tr::free_variables(right));
      if (lv.empty())
      {
        result = utilities::optimized_or(left, utilities::optimized_exists_no_empty_domain(rv, right, true));
      }
      else if (rv.empty())
      {
        result = utilities::optimized_or(right, utilities::optimized_exists_no_empty_domain(lv, left, true));
      }
      else
      {
        result = utilities::optimized_exists(variables, body, true);
      }
    }
    else
    {
      result = utilities::optimized_exists(variables, body, true);
    }
    return result;
  }

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    std::vector<data::data_expression> d;
    auto const& e = x.parameters();
    for (auto i = e.begin(); i != e.end(); ++i)
    {
      d.push_back(atermpp::aterm_cast<data::data_expression>(derived()(*i)));
    }
    return propositional_variable_instantiation(x.name(), data::data_expression_list(d.begin(), d.end()));
  }
};

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct simplify_quantifier_with_substitution_builder: public simplify_quantifier_builder<Derived, DataRewriter>
{
  typedef simplify_quantifier_builder<Derived, DataRewriter> super;
  using super::enter;
  using super::leave;
  using super::operator();

  SubstitutionFunction& sigma;

  simplify_quantifier_with_substitution_builder(const DataRewriter& R_, SubstitutionFunction& sigma_)
    : super(R_), sigma(sigma_)
  {}

  pbes_expression operator()(const data::data_expression& x)
  {
    return R(x, sigma);
  }
};

template <template <class, class> class Builder, class DataRewriter>
class apply_simplify_builder: public Builder<apply_simplify_builder<Builder, DataRewriter>, DataRewriter>
{
    typedef Builder<apply_simplify_builder<Builder, DataRewriter>, DataRewriter> super;

  public:
    using super::enter;
    using super::leave;
    using super::operator();

    apply_simplify_builder(const DataRewriter& datar)
      : super(datar)
    {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class, class> class Builder, class DataRewriter>
apply_simplify_builder<Builder, DataRewriter>
make_apply_simplify_builder(const DataRewriter& datar)
{
  return apply_simplify_builder<Builder, DataRewriter>(datar);
}

template <template <class, class, class> class Builder, class DataRewriter, class SubstitutionFunction>
class apply_simplify_with_substitution_builder: public Builder<apply_simplify_with_substitution_builder<Builder, DataRewriter, SubstitutionFunction>, DataRewriter, SubstitutionFunction>
{
    typedef Builder<apply_simplify_with_substitution_builder<Builder, DataRewriter, SubstitutionFunction>, DataRewriter, SubstitutionFunction> super;

  public:
    using super::enter;
    using super::leave;
    using super::operator();

    apply_simplify_with_substitution_builder(const DataRewriter& datar, const SubstitutionFunction& sigma)
      : super(datar, sigma)
    {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class, class, class> class Builder, class DataRewriter, class SubstitutionFunction>
apply_simplify_with_substitution_builder<Builder, DataRewriter, SubstitutionFunction>
make_apply_simplify_with_substitution_builder(const DataRewriter& datar, SubstitutionFunction& sigma)
{
  return apply_simplify_with_substitution_builder<Builder, DataRewriter, SubstitutionFunction>(datar, sigma);
}

} // namespace detail

/// \brief A rewriter that simplifies quantifiers.
template <typename DataRewriter>
class simplifying_quantifier_rewriter
{
  protected:
    /// \brief The data rewriter
    const DataRewriter& m_rewriter;

    // TODO: this is a temporary hack, to deal with the data_true <-> pbes_true problems in the rewriter
    pbes_expression convert_true_false(const pbes_expression& x) const
    {
      typedef core::term_traits<pbes_expression> tr;
      if (tr::is_true(x))
      {
        return data::sort_bool::true_();
      }
      if (tr::is_false(x))
      {
        return data::sort_bool::false_();
      }
      return x;
    }

  public:
    /// \brief The term type
    typedef pbes_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Constructor
    /// \param rewriter A data rewriter
    simplifying_quantifier_rewriter(const DataRewriter& rewriter)
      : m_rewriter(rewriter)
    {}

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      return convert_true_false(detail::make_apply_simplify_builder<detail::simplify_quantifier_builder>(m_rewriter)(x));
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
    {
      return convert_true_false(detail::apply_simplify_with_substitution_builder<detail::simplify_quantifier_with_substitution_builder, DataRewriter, SubstitutionFunction>(m_rewriter, sigma)(x));
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_SIMPLIFYING_QUANTIFIER_REWRITER_H
