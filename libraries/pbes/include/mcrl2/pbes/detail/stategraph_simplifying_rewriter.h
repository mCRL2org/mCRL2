// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_simplifying_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFYING_REWRITER_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFYING_REWRITER_H

#include "mcrl2/pbes/rewriters/simplifying_quantifier_rewriter.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename Derived, typename DataRewriter>
struct stategraph_simplify_quantifier_builder: public simplify_quantifier_builder<Derived, DataRewriter>
{
  typedef simplify_quantifier_builder<Derived, DataRewriter> super;
  using super::enter;
  using super::leave;
  using super::operator();

  typedef core::term_traits<pbes_expression> tr;

  /// \brief Constructor.
  /// \param rewr A data rewriter
  stategraph_simplify_quantifier_builder(const DataRewriter& rewr)
    : super(rewr)
  { }

  bool is_data_not(const pbes_expression& x) const
  {
    return data::is_data_expression(x) && data::sort_bool::is_not_application(x);
  }

  bool is_data_and(const pbes_expression& x) const
  {
    return data::is_data_expression(x) && data::sort_bool::is_and_application(x);
  }

  bool is_data_or(const pbes_expression& x) const
  {
    return data::is_data_expression(x) && data::sort_bool::is_or_application(x);
  }

  // returns the argument of a data not
  data::data_expression not_arg(const data::data_expression& x)
  {
    return *data::application(x).begin();
  }

  // replace !(y || z) by !y && !z
  // replace !(y && z) by !y || !z
  // replace !(y => z) by y || !z
  // replace y => z by !y || z
  pbes_expression post_process(const pbes_expression& x)
  {
    pbes_expression result = x;
    if (tr::is_not(x))
    {
      const pbes_expression& t = tr::not_arg(x);
      if (tr::is_or(t)) // x = !(y && z)
      {
        pbes_expression y = utilities::optimized_not(tr::left(t));
        pbes_expression z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_and(y, z);
      }
      else if (tr::is_and(t)) // x = !(y || z)
      {
        pbes_expression y = utilities::optimized_not(tr::left(t));
        pbes_expression z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_or(y, z);
      }
      else if (tr::is_imp(t)) // x = !(y => z)
      {
        pbes_expression y = tr::left(t);
        pbes_expression z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_or(y, z);
      }
      else if (is_data_not(t)) // x = !val(!y)
      {
        const data::data_expression& z = atermpp::aterm_cast<const data::data_expression>(t);
        data::data_expression y = not_arg(z);
        result = y;
      }
    }
    else if (tr::is_imp(x)) // x = y => z
    {
      pbes_expression y = utilities::optimized_not(tr::left(x));
      pbes_expression z = tr::right(x);
      result = utilities::optimized_or(y, z);
    }
    return result;
  }

  // replaces data operators !, ||, && by their pbes equivalent
  pbes_expression preprocess(const data::data_expression& x)
  {
    typedef core::term_traits<data::data_expression> tt;
    if (is_data_not(x))
    {
      const data::data_expression& y = atermpp::aterm_cast<const data::data_expression>(x);
      return tr::not_(not_arg(y));
    }
    else if (is_data_and(x))
    {
      const data::data_expression& y = atermpp::aterm_cast<const data::data_expression>(x);
      return tr::and_(tt::left(y), tt::right(y));
    }
    else if (is_data_or(x))
    {
      const data::data_expression& y = atermpp::aterm_cast<const data::data_expression>(x);
      return tr::or_(tt::left(y), tt::right(y));
    }
    return x;
  }

  // replace the data expression y != z by !(y == z)
  pbes_expression operator()(const data::data_expression& x)
  {
    // if x has one of the operators !, ||, && at the top level, handle it by the PBES simplification
    pbes_expression y = preprocess(x);
    if (y != x)
    {
      return super::operator()(y);
    }
    typedef core::term_traits<data::data_expression> tt;
    pbes_expression result = super::operator()(x);
    const data::data_expression& t = atermpp::aterm_cast<const data::data_expression>(result);
    if (data::is_not_equal_to_application(t)) // result = y != z
    {
      data::data_expression y = tt::left(t);
      data::data_expression z = tt::right(t);
      result = tr::not_(data::equal_to(y, z));
    }
    return post_process(result);
  }

  pbes_expression operator()(const true_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const false_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const not_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const and_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const or_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const imp& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const forall& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const exists& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    return post_process(super::operator()(x));
  }
};

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct stategraph_simplify_quantifier_with_substitution_builder: public stategraph_simplify_quantifier_builder<Derived, DataRewriter>
{
  typedef stategraph_simplify_quantifier_builder<Derived, DataRewriter> super;
  using super::enter;
  using super::leave;
  using super::operator();

  SubstitutionFunction& sigma;

  stategraph_simplify_quantifier_with_substitution_builder(const DataRewriter& R_, SubstitutionFunction& sigma_)
    : super(R_), sigma(sigma_)
  {}

  pbes_expression operator()(const data::data_expression& x)
  {
    return R(x, sigma);
  }
};

/// \brief A rewriter that simplifies expressions that simplifies quantifiers.
template <typename DataRewriter>
class stategraph_simplifying_rewriter
{
  protected:
    /// \brief The data rewriter
    const DataRewriter& m_rewriter;

  public:
    /// \brief The term type
    typedef pbes_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Constructor
    /// \param rewriter A data rewriter
    stategraph_simplifying_rewriter(const DataRewriter& rewriter)
      : m_rewriter(rewriter)
    {}

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      return detail::make_apply_simplify_builder<stategraph_simplify_quantifier_builder>(m_rewriter)(x);
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
    {
      return detail::make_apply_simplify_with_substitution_builder<stategraph_simplify_quantifier_with_substitution_builder>(m_rewriter, sigma)(x);
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFYING_REWRITER_H
