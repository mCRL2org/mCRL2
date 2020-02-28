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

#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/join.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

pbes_expression quantifiers_inside(const pbes_expression& x);
pbes_expression quantifiers_inside_forall(const std::set<data::variable>& variables, const pbes_expression& x);
pbes_expression quantifiers_inside_exists(const std::set<data::variable>& variables, const pbes_expression& x);

inline
std::tuple<pbes_expression, pbes_expression> compute_Phi_Psi(const std::vector<pbes_expression>& X, const std::set<data::variable>& V)
{
  using utilities::detail::set_difference;
  using utilities::detail::set_intersection;

  std::vector<std::set<data::variable>> vars; // free variables
  for (const pbes_expression& x_j: X)
  {
    vars.push_back(set_intersection(find_free_variables(x_j), V));
  }
  auto j = std::min_element(vars.begin(), vars.end(),
                            [&](const std::set<data::variable>& x, const std::set<data::variable>& y)
                            {
                              return x.size() < y.size();
                            }
  );
  const std::set<data::variable>& Z = *j;

  std::vector<pbes_expression> phi;
  std::vector<pbes_expression> psi;
  for (std::size_t i = 0; i < X.size(); i++)
  {
    if (std::includes(Z.begin(), Z.end(), vars[i].begin(), vars[i].end()))
    {
      psi.push_back(X[i]);
    }
    else
    {
      phi.push_back(X[i]);
    }
  }
  pbes_expression Phi = join_or(phi.begin(), phi.end());
  pbes_expression Psi = join_or(psi.begin(), psi.end());
  return { Phi, Psi };
}

struct quantifiers_inside_builder: public pbes_expression_builder<quantifiers_inside_builder>
{
  typedef pbes_expression_builder<quantifiers_inside_builder> super;
  using super::apply;

  pbes_expression apply(const forall& x)
  {
    pbes_expression const& phi = x.body();
    std::set<data::variable> W = data::detail::make_variable_set(x.variables());
    return quantifiers_inside_forall(W, apply(phi));
  }

  pbes_expression apply(const exists& x)
  {
    pbes_expression const& phi = x.body();
    std::set<data::variable> W = data::detail::make_variable_set(x.variables());
    return quantifiers_inside_exists(W, apply(phi));
  }
};

struct quantifiers_inside_forall_builder: public data_expression_builder<quantifiers_inside_forall_builder>
{
  typedef data_expression_builder<quantifiers_inside_forall_builder> super;
  using super::apply;

  const std::set<data::variable>& V;

  explicit quantifiers_inside_forall_builder(const std::set<data::variable>& V_)
      : V(V_)
  {}

  static pbes_expression make_forall(const data::variable_list& vars, const pbes_expression& body)
  {
    return vars.empty() ? body : forall(vars, body);
  }

  // default case
  template <typename T>
  pbes_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<data::variable> W = set_intersection(V, find_free_variables(x));
    return make_forall(data::variable_list(W.begin(), W.end()), x);
  }

  pbes_expression apply(const forall& x)
  {
    using utilities::detail::set_union;
    std::set<data::variable> W = data::detail::make_variable_set(x.variables());
    return quantifiers_inside_forall(set_union(V, W), x.body());
  }

  pbes_expression apply(const not_& x)
  {
    const auto& phi = x.operand();
    return not_(quantifiers_inside_exists(V, phi));
  }

  pbes_expression apply(const and_& x)
  {
    const pbes_expression& phi = x.left();
    const pbes_expression& psi = x.right();
    return and_(quantifiers_inside_forall(V, phi), quantifiers_inside_forall(V, psi));
  }

  pbes_expression apply(const or_& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    typedef core::term_traits<pbes_expression> tr;

    std::vector<pbes_expression> X;
    utilities::detail::split(x, std::back_inserter(X), tr::is_or, tr::left, tr::right);
    const auto [Phi, Psi] = compute_Phi_Psi(X, V);
    if (is_false(Phi) || is_false(Psi))
    {
      return forall(data::detail::make_variable_list(V), x);
    }
    std::set<data::variable> vars_Phi = find_free_variables(Phi);
    std::set<data::variable> vars_Psi = find_free_variables(Psi);
    return make_forall(data::detail::make_variable_list(set_intersection(V, set_intersection(vars_Phi, vars_Psi))),
                       optimized_or(
                           quantifiers_inside_forall(set_difference(set_intersection(V, vars_Phi), vars_Psi), Phi),
                           quantifiers_inside_forall(set_difference(set_intersection(V, vars_Psi), vars_Phi), Psi)
                       )
    );
  }

  pbes_expression apply(const imp& x)
  {
    return quantifiers_inside_forall(V, or_(not_(x.left()), x.right()));
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const exists& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const data::data_expression& x)
  {
    return data::detail::quantifiers_inside_forall(V, x);
  }
};

struct quantifiers_inside_exists_builder: public pbes_expression_builder<quantifiers_inside_exists_builder>
{
  typedef pbes_expression_builder<quantifiers_inside_exists_builder> super;
  using super::apply;

  const std::set<data::variable>& V;

  explicit quantifiers_inside_exists_builder(const std::set<data::variable>& V_)
      : V(V_)
  {}

  static pbes_expression make_exists(const data::variable_list& vars, const pbes_expression& body)
  {
    return vars.empty() ? body : exists(vars, body);
  }

  // default case
  template <typename T>
  pbes_expression apply_default(const T& x)
  {
    using utilities::detail::set_intersection;
    std::set<data::variable> W = set_intersection(V, find_free_variables(x));
    return make_exists(data::variable_list(W.begin(), W.end()), x);
  }

  pbes_expression apply(const exists& x)
  {
    using utilities::detail::set_union;
    std::set<data::variable> W = data::detail::make_variable_set(x.variables());
    return quantifiers_inside_exists(set_union(V, W), x.body());
  }

  pbes_expression apply(const not_& x)
  {
    const auto& phi = x.operand();
    return not_(quantifiers_inside_forall(V, phi));
  }

  pbes_expression apply(const or_& x)
  {
    const pbes_expression& phi = x.left();
    const pbes_expression& psi = x.right();
    return or_(quantifiers_inside_exists(V, phi), quantifiers_inside_exists(V, psi));
  }

  pbes_expression apply(const and_& x)
  {
    using utilities::detail::set_difference;
    using utilities::detail::set_intersection;
    typedef core::term_traits<pbes_expression> tr;

    std::vector<pbes_expression> X;
    utilities::detail::split(x, std::back_inserter(X), tr::is_and, tr::left, tr::right);
    const auto [Phi, Psi] = compute_Phi_Psi(X, V);
    if (is_true(Phi) || is_true(Psi))
    {
      return exists(data::detail::make_variable_list(V), x);
    }
    std::set<data::variable> vars_Phi = find_free_variables(Phi);
    std::set<data::variable> vars_Psi = find_free_variables(Psi);
    return make_exists(data::detail::make_variable_list(set_intersection(V, set_intersection(vars_Phi, vars_Psi))),
                       optimized_and(
                           quantifiers_inside_exists(set_difference(set_intersection(V, vars_Phi), vars_Psi), Phi),
                           quantifiers_inside_exists(set_difference(set_intersection(V, vars_Psi), vars_Phi), Psi)
                       )
    );
  }

  pbes_expression apply(const imp& x)
  {
    return quantifiers_inside_exists(V, and_(not_(x.left()), x.right()));
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const forall& x)
  {
    return apply_default(x);
  }

  pbes_expression apply(const data::data_expression& x)
  {
    return data::detail::quantifiers_inside_exists(V, x);
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

/// \brief A rewriter that pushes quantifiers inside in a PBES expression.
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
