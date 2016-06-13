// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_simplify_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFY_REWRITER_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFY_REWRITER_H

#include "mcrl2/data/rewriters/simplify_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h"
#include "mcrl2/pbes/detail/stategraph_split.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
pbes_expression stategraph_not(const pbes_expression& x)
{
  typedef core::term_traits<data::data_expression> tt;
  typedef core::term_traits<pbes_expression> tr;
  if (is_data(x))
  {
    auto const& x1 = atermpp::down_cast<data::data_expression>(x);
    if (data::sort_bool::is_not_application(x1))
    {
      return tt::not_arg(x1);
    }
    else if (data::is_not_equal_to_application(x1))
    {
      auto const& left  = tt::left(x1);
      auto const& right = tt::right(x1);
      return data::equal_to(left, right);
    }
    else if (data::is_equal_to_application(x1))
    {
      auto const& left  = tt::left(x1);
      auto const& right = tt::right(x1);
      return data::not_equal_to(left, right);
    }
    else
    {
      return data::sort_bool::not_(x1);
    }
  }
  return tr::not_(x);
}

inline
pbes_expression smart_and(const pbes_expression& x, const pbes_expression& y)
{
  if (is_data(x) && is_data(y))
  {
    return data::sort_bool::and_(atermpp::down_cast<data::data_expression>(x), atermpp::down_cast<data::data_expression>(y));
  }
  return and_(x, y);
}

inline
pbes_expression smart_or(const pbes_expression& x, const pbes_expression& y)
{
  if (is_data(x) && is_data(y))
  {
    return data::sort_bool::or_(atermpp::down_cast<data::data_expression>(x), atermpp::down_cast<data::data_expression>(y));
  }
  return or_(x, y);
}

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct stategraph_simplify_builder: public simplify_quantifiers_data_rewriter_builder<Derived, DataRewriter, SubstitutionFunction>
{
  typedef simplify_quantifiers_data_rewriter_builder<Derived, DataRewriter, SubstitutionFunction> super;
  using super::apply;

  typedef core::term_traits<data::data_expression> tt;
  typedef core::term_traits<pbes_expression> tr;

  /// \brief Constructor.
  /// \param rewr A data rewriter
  stategraph_simplify_builder(const DataRewriter& R, SubstitutionFunction& sigma)
    : super(R, sigma)
  { }

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

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

  // returns the argument of a not expression (pbes or data)
  const pbes_expression& not_arg(const pbes_expression& x)
  {
    if (is_data(x))
    {
      return atermpp::down_cast<pbes_expression>(*data::application(x).begin());
    }
    else
    {
      return atermpp::down_cast<not_>(x).operand();
    }
  }

  pbes_expression stategraph_join_or(const std::vector<pbes_expression>& terms) const
  {
    const pbes_expression& F = false_();
    return utilities::detail::join(terms.begin(), terms.end(), smart_or, F);
  }

  pbes_expression stategraph_join_and(const std::vector<pbes_expression>& terms) const
  {
    const pbes_expression& T = true_();
    return utilities::detail::join(terms.begin(), terms.end(), smart_and, T);
  }

  // apply de Morgan rules
  pbes_expression post_process(const pbes_expression& x)
  {
    pbes_expression result = x;

    if (is_universal_not(x))
    {
      const pbes_expression& arg = not_arg(x);

      // replace !(x1 \/ ... \/ xn) by !x1 /\ ... /\ !xn
      if (is_universal_or(arg))
      {
        std::vector<pbes_expression> terms;
        detail::stategraph_split_or(arg, terms);
        for (pbes_expression& term: terms)
        {
          term = stategraph_not(term);
        }
        result = stategraph_join_and(terms);
      }
      // replace !(x1 /\ ... /\ xn) by !x1 \/ ... \/ !xn
      else if (is_universal_and(arg))
      {
        std::vector<pbes_expression> terms;
        detail::stategraph_split_and(arg, terms);
        for (pbes_expression& term: terms)
        {
          term = stategraph_not(term);
        }
        result = stategraph_join_or(terms);
      }
      else
      {
        result = stategraph_not(arg);
      }
    }
    mCRL2log(log::debug2, "stategraph") << "  simplify-postprocess " << x << " -> " << result << std::endl;
    return result;
  }

  pbes_expression apply(const data::data_expression& x)
  {
    return post_process(super::apply(data::simplify(x)));
  }

  pbes_expression apply(const not_& x)
  {
    return post_process(super::apply(x));
  }

  pbes_expression apply(const and_& x)
  {
    return post_process(super::apply(x));
  }

  pbes_expression apply(const or_& x)
  {
    return post_process(super::apply(x));
  }

  pbes_expression apply(const imp& x)
  {
    return derived().apply(or_(not_(x.left()), x.right()));
  }

  pbes_expression apply(const forall& x)
  {
    return post_process(super::apply(x));
  }

  pbes_expression apply(const exists& x)
  {
    return post_process(super::apply(x));
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    return post_process(super::apply(x));
  }
};

/// \brief A rewriter that simplifies expressions that simplifies quantifiers.
template <typename DataRewriter>
class stategraph_simplify_rewriter
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
    stategraph_simplify_rewriter(const DataRewriter& rewriter)
      : m_rewriter(rewriter)
    {}

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      data::no_substitution sigma;
      return detail::make_apply_rewriter_builder<stategraph_simplify_builder>(m_rewriter, sigma).apply(x);
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
    {
      return detail::make_apply_rewriter_builder<stategraph_simplify_builder>(m_rewriter, sigma).apply(x);
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFY_REWRITER_H
