// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
#define MCRL2_PBES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H

#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

pbes_expression quantifiers_inside(const pbes_expression& x);
pbes_expression quantifiers_inside_forall(const std::set<data::variable>& variables, const pbes_expression& x);
pbes_expression quantifiers_inside_exists(const std::set<data::variable>& variables, const pbes_expression& x);

inline
std::set<data::variable> make_variable_set(const data::variable_list& x)
{
  return std::set<data::variable>(x.begin(), x.end());
}

struct quantifiers_inside_builder: public pbes_expression_builder<quantifiers_inside_builder>
{
  typedef pbes_expression_builder<quantifiers_inside_builder> super;
  using super::apply;

  pbes_expression apply(const forall& x)
  {
    auto const& phi = x.body();
    auto W = make_variable_set(x.variables());
    return quantifiers_inside_forall(W, apply(phi));
  }

  pbes_expression apply(const exists& x)
  {
    auto const& phi = x.body();
    auto W = make_variable_set(x.variables());
    return quantifiers_inside_exists(W, apply(phi));
  }
};

struct quantifiers_inside_forall_builder: public pbes_expression_builder<quantifiers_inside_forall_builder>
{
  typedef pbes_expression_builder<quantifiers_inside_forall_builder> super;
  using super::apply;

  const std::set<data::variable>& V;

  quantifiers_inside_forall_builder(const std::set<data::variable>& V_)
    : V(V_)
  {}

  pbes_expression apply(const not_& x)
  {
    auto const& phi = x.operand();
    return not_(quantifiers_inside_exists(V, phi));
  }

  pbes_expression apply(const or_& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    typedef core::term_traits<pbes_system::pbes_expression> tr;

    std::vector<pbes_expression> X;
    utilities::detail::split(x, std::back_inserter(X), tr::is_or, tr::left, tr::right);
    std::vector<std::set<data::variable>> FV;
    for (const pbes_expression& x_i: X)
    {
      FV.push_back(set_intersection(find_free_variables(x_i), V));
    }
    auto i = std::min_element(FV.begin(), FV.end(),
            [&](const std::set<data::variable>& x, const std::set<data::variable>& y)
            {
              return x.size() < y.size();
            }
           );

    const std::set<data::variable>& W = *i;
    std::vector<pbes_expression> X1;
    std::vector<pbes_expression> X2;
    for (std::size_t j = 0; j < X.size(); j++)
    {
      if (std::includes(W.begin(), W.end(), FV[j].begin(), FV[j].end()))
      {
        X2.push_back(X[j]);
      }
      else
      {
        X1.push_back(X[j]);
      }
    }
    return make_forall(data::variable_list(W.begin(), W.end()),
                       or_(quantifiers_inside_forall(set_difference(V, W), join_or(X1.begin(), X1.end())),
                           join_or(X2.begin(), X2.end()))
                      );
  }

  pbes_expression apply(const imp& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    auto const& phi = x.left();
    auto const& psi = x.right();
    auto W = set_intersection(set_intersection(V, find_free_variables(phi)), find_free_variables(psi));
    return make_forall(data::variable_list(W.begin(), W.end()),
                       imp(quantifiers_inside_exists(set_difference(V, W), phi),
                           quantifiers_inside_forall(set_difference(V, W), psi)
                          )
                      );
  }

  pbes_expression apply(const forall& x)
  {
    using utilities::detail::set_union;
    std::set<data::variable> W = make_variable_set(x.variables());
    return quantifiers_inside_forall(set_union(V, W), x.body());
  }

  // default case
  template <typename T>
  pbes_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    auto W = set_intersection(V, find_free_variables(x));
    return make_forall(data::variable_list(W.begin(), W.end()), x);
  }

  pbes_expression apply(const exists& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const data::data_expression& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    return apply_default(x);
  }
};

struct quantifiers_inside_exists_builder: public pbes_expression_builder<quantifiers_inside_exists_builder>
{
  typedef pbes_expression_builder<quantifiers_inside_exists_builder> super;
  using super::apply;

  const std::set<data::variable>& V;

  quantifiers_inside_exists_builder(const std::set<data::variable>& V_)
    : V(V_)
  {}

  pbes_expression apply(const not_& x)
  {
    auto const& phi = x.operand();
    return not_(quantifiers_inside_forall(V, phi));
  }

  pbes_expression apply(const and_& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    typedef core::term_traits<pbes_system::pbes_expression> tr;

    std::vector<pbes_expression> X;
    utilities::detail::split(x, std::back_inserter(X), tr::is_and, tr::left, tr::right);
    std::vector<std::set<data::variable>> FV;
    for (const pbes_expression& x_i: X)
    {
      FV.push_back(set_intersection(find_free_variables(x_i), V));
    }
    auto i = std::min_element(FV.begin(), FV.end(),
                              [&](const std::set<data::variable>& x, const std::set<data::variable>& y)
                              {
                                  return x.size() < y.size();
                              }
    );

    const std::set<data::variable>& W = *i;
    std::vector<pbes_expression> X1;
    std::vector<pbes_expression> X2;
    for (std::size_t j = 0; j < X.size(); j++)
    {
      if (std::includes(W.begin(), W.end(), FV[j].begin(), FV[j].end()))
      {
        X2.push_back(X[j]);
      }
      else
      {
        X1.push_back(X[j]);
      }
    }
    return make_exists(data::variable_list(W.begin(), W.end()),
                       and_(quantifiers_inside_exists(set_difference(V, W), join_and(X1.begin(), X1.end())),
                            join_and(X2.begin(), X2.end()))
    );
  }

  pbes_expression apply(const imp& x)
  {
    auto const& phi = x.left();
    auto const& psi = x.right();
    return imp(quantifiers_inside_forall(V, phi), quantifiers_inside_exists(V, psi));
  }

  pbes_expression apply(const exists& x)
  {
    using utilities::detail::set_union;
    std::set<data::variable> W = make_variable_set(x.variables());
    return quantifiers_inside_exists(set_union(V, W), x.body());
  }

  // default case
  template <typename T>
  pbes_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    auto W = set_intersection(V, find_free_variables(x));
    return make_exists(data::variable_list(W.begin(), W.end()), x);
  }

  pbes_expression apply(const forall& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const data::data_expression& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    return apply_default(x);
  }
};

inline
pbes_expression quantifiers_inside(const pbes_expression& x)
{
  quantifiers_inside_builder f;
  return f.apply(x);
}

inline
pbes_expression quantifiers_inside_forall(const std::set<data::variable>& variables, const pbes_expression& x)
{
  quantifiers_inside_forall_builder f(variables);
  return f.apply(x);
}

inline
pbes_expression quantifiers_inside_exists(const std::set<data::variable>& variables, const pbes_expression& x)
{
  quantifiers_inside_exists_builder f(variables);
  return f.apply(x);
}

} // namespace detail

/// \brief A rewriter that applies one point rule quantifier elimination to a PBES.
class quantifiers_inside_rewriter
{
  public:
    /// \brief The term type
    typedef pbes_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      return detail::quantifiers_inside(x);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_QUANTIFIERS_INSIDE_REWRITER_H
